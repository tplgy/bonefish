#!/usr/bin/env python3
"""End-to-end tests for the Bonefish WAMP daemon over all transports.

Starts the built daemon (rawsocket TCP + websocket + unix socket), then runs
full WAMP v2 flows against it:
  * RawSocket/messagepack over TCP and over a unix socket
  * WebSocket/JSON
  * CALL -> INVOCATION -> YIELD -> RESULT echo round trip
  * SUBSCRIBE + PUBLISH -> EVENT
  * Large-payload (512 KiB args) round trip (rawsocket framing regression)

Exit code 0 == all tests passed.
"""

import argparse
import asyncio
import json
import os
import socket
import struct
import subprocess
import sys
import tempfile
import time

import msgpack

HELLO, WELCOME, ABORT = 1, 2, 3
PUBLISH, PUBLISHED = 16, 17
SUBSCRIBE, SUBSCRIBED, UNSUBSCRIBE, UNSUBSCRIBED, EVENT = 32, 33, 34, 35, 36
CALL, RESULT = 48, 50
REGISTER, REGISTERED, INVOCATION, INTERRUPT, YIELD = 64, 65, 68, 69, 70

REALM = "default"
DETAILS = {"roles": {r: {"features": {}} for r in ("caller", "callee", "publisher", "subscriber")}}
LARGE = "x" * (512 * 1024)


class Client:
    """Minimal async WAMP v2 client. send(raw msg list); expects replies."""

    def __init__(self, reader, writer):
        self.reader = reader
        self.writer = writer
        self.inbox = asyncio.Queue()
        self._reader_task = None

    def start(self):
        self._reader_task = asyncio.ensure_future(self._read_loop())

    async def stop(self):
        if self._reader_task:
            self._reader_task.cancel()
        self.writer.close()

    async def _recv_message(self):
        raise NotImplementedError

    def _send_message(self, msg):
        raise NotImplementedError

    async def _read_loop(self):
        try:
            while True:
                msg = await self._recv_message()
                if msg is None:
                    break
                await self.inbox.put(msg)
        except asyncio.CancelledError:
            pass
        except Exception as exc:  # noqa: BLE001
            await self.inbox.put(("__closed__", str(exc)))

    async def expect(self, typecode, timeout=10.0):
        msg = await asyncio.wait_for(self.inbox.get(), timeout)
        if msg and msg[0] != typecode:
            raise AssertionError(f"expected message type {typecode}, got {msg!r}")
        return msg

    def _drain_pending(self):
        while not self.inbox.empty():
            self.inbox.get_nowait()


class RawSocketClient(Client):
    """RawSocket + messagepack. Handshake: 0x7F, max-msg exponent<<4 | serializer."""

    async def handshake(self):
        # byte1 low nibble = serializer (0x2 = msgpack); byte1 high nibble =
        # max-message-length exponent; bytes 2-3 reserved.
        self.writer.write(struct.pack("!BBBB", 0x7F, 0x02, 0x00, 0x00))
        await self.writer.drain()
        caps = await self.reader.readexactly(4)
        b = struct.unpack("!I", caps)[0]
        if (b & 0x000F0000) != 0x00020000:
            raise AssertionError(f"router did not select msgpack serializer: {b:#010x}")

    async def _recv_message(self):
        header = await self.reader.readexactly(4)
        (length,) = struct.unpack("!I", header)
        payload = await self.reader.readexactly(length)
        return msgpack.unpackb(payload, raw=False)

    def _send_message(self, msg):
        payload = msgpack.packb(msg, use_bin_type=False)
        self.writer.write(struct.pack("!I", len(payload)) + payload)


class WebSocketClient(Client):
    """WebSocket + JSON (wamp.2.json subprotocol)."""

    def __init__(self, ws):
        super().__init__(None, ws)
        self._ws = ws

    async def _recv_message(self):
        raw = await self._ws.recv()
        return json.loads(raw)

    def _send_message(self, msg):
        async def _do():
            await self._ws.send(json.dumps(msg))
        asyncio.ensure_future(_do())

    async def stop(self):
        await self._ws.close()


async def run_flow(client, label):
    """Full WAMP v2 flow on a single session."""
    client.start()

    # HELLO -> WELCOME
    client._send_message([HELLO, REALM, DETAILS])
    welcome = await client.expect(WELCOME)
    assert welcome[1] > 0, f"{label}: bad session id"

    # SUBSCRIBE -> SUBSCRIBED
    client._send_message([SUBSCRIBE, 1, {}, "test.topic"])
    subscribed = await client.expect(SUBSCRIBED)
    subscription_id = subscribed[2]

    # REGISTER -> REGISTERED (echo procedure)
    client._send_message([REGISTER, 2, {}, "test.echo"])
    registered = await client.expect(REGISTERED)
    registration_id = registered[2]

    # CALL -> INVOCATION -> YIELD -> RESULT (small payload)
    client._send_message([CALL, 3, {}, "test.echo", ["hello"]])
    invocation = await client.expect(INVOCATION)
    assert invocation[2] == registration_id
    assert invocation[4] == ["hello"]
    client._send_message([YIELD, invocation[1], {}, invocation[4]])
    result = await client.expect(RESULT)
    assert result[1] == 3 and result[3] == ["hello"], f"{label}: bad RESULT {result!r}"

    # PUBLISH -> EVENT
    client._send_message([PUBLISH, 4, {}, "test.topic", ["event-payload"]])
    event = await client.expect(EVENT)
    assert event[1] == subscription_id
    assert event[4] == ["event-payload"], f"{label}: bad EVENT {event!r}"

    # Large-payload round trip (rawsocket framing regression, ~512 KiB args)
    client._send_message([CALL, 5, {}, "test.echo", [LARGE]])
    invocation = await client.expect(INVOCATION, timeout=30)
    assert len(invocation[4][0]) == len(LARGE), f"{label}: truncated invocation args"
    assert invocation[4][0] == LARGE
    client._send_message([YIELD, invocation[1], {}, invocation[4]])
    result = await client.expect(RESULT, timeout=30)
    assert result[3][0] == LARGE, f"{label}: truncated result args"

    # Large PUBLISH -> EVENT
    client._send_message([PUBLISH, 6, {}, "test.topic", [LARGE]])
    event = await client.expect(EVENT, timeout=30)
    assert event[4][0] == LARGE, f"{label}: truncated event args"

    await client.stop()


async def connect_rawsocket_tcp(port):
    reader, writer = await asyncio.open_connection("127.0.0.1", port)
    client = RawSocketClient(reader, writer)
    await client.handshake()
    return client


async def connect_rawsocket_uds(path):
    reader, writer = await asyncio.open_unix_connection(path)
    client = RawSocketClient(reader, writer)
    await client.handshake()
    return client


async def connect_websocket(port):
    import websockets
    ws = await websockets.connect(
        f"ws://127.0.0.1:{port}/ws", subprotocols=["wamp.2.json"])
    if ws.subprotocol != "wamp.2.json":
        raise AssertionError(f"unexpected subprotocol {ws.subprotocol}")
    return WebSocketClient(ws)


def daemon_stderr():
    """If BF_E2E_STDERR is set, capture daemon stderr there (for sanitizer
    scans); otherwise discard it."""
    path = os.environ.get("BF_E2E_STDERR")
    if path:
        return open(path, "a")
    return subprocess.DEVNULL


def check_sanitizer(output, label):
    for marker in ("ERROR: AddressSanitizer", "ERROR: LeakSanitizer",
                   "runtime error:", "SUMMARY: AddressSanitizer"):
        assert marker not in output, f"{marker} in {label} stderr:\n{output}"


def free_port():
    s = socket.socket()
    s.bind(("127.0.0.1", 0))
    port = s.getsockname()[1]
    s.close()
    return port


def wait_port(port, timeout=10.0):
    deadline = time.monotonic() + timeout
    while time.monotonic() < deadline:
        try:
            s = socket.create_connection(("127.0.0.1", port), 0.5)
            s.close()
            return True
        except OSError:
            time.sleep(0.1)
    return False


async def test_daemon(binary, use_debug):
    raw_port, ws_port = free_port(), free_port()
    sockdir = tempfile.mkdtemp(prefix="bonefish-e2e-")
    sock_path = os.path.join(sockdir, "rawsocket.sock")

    cmd = [binary, "-r", REALM, "-t", str(raw_port), "-w", str(ws_port),
           "-u", sock_path]
    if use_debug:
        cmd.append("-d")
    proc = subprocess.Popen(cmd, stdout=subprocess.DEVNULL,
                            stderr=daemon_stderr())
    try:
        if not wait_port(raw_port) or not wait_port(ws_port):
            raise AssertionError("daemon did not open listening sockets")

        for connector, label in (
                (lambda: connect_rawsocket_tcp(raw_port), "rawsocket-tcp"),
                (lambda: connect_rawsocket_uds(sock_path), "rawsocket-uds"),
                (lambda: connect_websocket(ws_port), "websocket-json")):
            client = await connector()
            await run_flow(client, label)
            print(f"PASS daemon {label} (large payload included)")
    finally:
        proc.terminate()
        try:
            proc.wait(timeout=5)
        except subprocess.TimeoutExpired:
            proc.kill()
        os.unlink(sock_path)
        os.rmdir(sockdir)


async def test_embedded(binary):
    """examples/integration: embedded router over rawsocket TCP 8888."""
    proc = subprocess.Popen([binary], stdout=subprocess.DEVNULL,
                            stderr=daemon_stderr())
    try:
        if not wait_port(8888):
            raise AssertionError("integration example did not open port 8888")
        client = await connect_rawsocket_tcp(8888)
        client.start()
        client._send_message([HELLO, "default", DETAILS])
        welcome = await client.expect(WELCOME)
        assert welcome[1] > 0
        await client.stop()
        print("PASS embedded integration example (HELLO/WELCOME)")
    finally:
        proc.terminate()
        try:
            proc.wait(timeout=5)
        except subprocess.TimeoutExpired:
            proc.kill()


async def test_daemon_options(binary):
    """CLI regression: --help, unknown option, and disabled serializers."""
    # --help exits 1 and prints usage (original program_options behavior).
    out = subprocess.run([binary, "--help"], capture_output=True, text=True)
    assert out.returncode == 1, "help should exit 1"
    for flag in ("--realm", "--websocket-port", "--rawsocket-port",
                 "--rawsocket-path", "--no-json", "--no-msgpack", "--debug"):
        assert flag in out.stdout, f"help missing {flag}"
    print("PASS cli --help")

    out = subprocess.run([binary, "--bogus-option"], capture_output=True, text=True)
    assert out.returncode == 1 and "Unrecognized options" in out.stdout, \
        f"bad option not handled: {out.stdout[:200]}"
    print("PASS cli bad option")

    # Serializer options apply to the websocket transport (rawsocket is
    # msgpack-only by design). --no-msgpack must reject wamp.2.msgpack but
    # still accept wamp.2.json, and vice versa for --no-json.
    async def ws_rejected(ws_port, subprotocol):
        import websockets
        try:
            ws = await websockets.connect(
                f"ws://127.0.0.1:{ws_port}/ws", subprotocols=[subprotocol])
        except Exception:
            return True
        await ws.close()
        return False

    async def ws_accepted(ws_port, subprotocol):
        import websockets
        ws = await websockets.connect(
            f"ws://127.0.0.1:{ws_port}/ws", subprotocols=[subprotocol])
        accepted = ws.subprotocol == subprotocol
        await ws.close()
        return accepted

    for disable_flag, rejected_sub, accepted_sub in (
            ("--no-msgpack", "wamp.2.msgpack", "wamp.2.json"),
            ("--no-json", "wamp.2.json", "wamp.2.msgpack")):
        ws_port = free_port()
        proc = subprocess.Popen([binary, "-r", REALM, "-w", str(ws_port),
                                 disable_flag], stdout=subprocess.DEVNULL,
                                stderr=daemon_stderr())
        try:
            if not wait_port(ws_port):
                raise AssertionError(f"daemon with {disable_flag} did not listen")
            assert await ws_rejected(ws_port, rejected_sub), \
                f"{rejected_sub} accepted despite {disable_flag}"
            assert await ws_accepted(ws_port, accepted_sub), \
                f"{accepted_sub} rejected despite only {disable_flag} set"
            print(f"PASS cli {disable_flag} (websocket subprotocol selection)")
        finally:
            proc.terminate()
            try:
                proc.wait(timeout=5)
            except subprocess.TimeoutExpired:
                proc.kill()

    # RawSocket negotiates msgpack, but with --no-msgpack no msgpack
    # serializer is registered: the handshake must be rejected outright
    # (regression: this previously segfaulted the daemon on first message).
    raw_port = free_port()
    proc = subprocess.Popen([binary, "-r", REALM, "-t", str(raw_port),
                             "--no-msgpack"], stdout=subprocess.DEVNULL,
                            stderr=daemon_stderr())
    try:
        if not wait_port(raw_port):
            raise AssertionError("daemon with --no-msgpack did not listen")
        reader, writer = await asyncio.open_connection("127.0.0.1", raw_port)
        writer.write(struct.pack("!BBBB", 0x7F, 0x02, 0x00, 0x00))
        await writer.drain()
        caps = await asyncio.wait_for(reader.readexactly(4), 5)
        ser = (struct.unpack("!I", caps)[0] & 0x000F0000) >> 16
        assert ser != 0x02, "handshake accepted msgpack despite --no-msgpack"
        writer.close()
        time.sleep(1.0)
        assert proc.poll() is None, "daemon crashed on --no-msgpack handshake"
        print("PASS rawsocket --no-msgpack rejects handshake, daemon survives")
    finally:
        proc.terminate()
        try:
            proc.wait(timeout=5)
        except subprocess.TimeoutExpired:
            proc.kill()


async def amain():
    parser = argparse.ArgumentParser()
    parser.add_argument("--daemon", required=True, help="path to bonefish binary")
    parser.add_argument("--integration", help="path to integration example")
    args = parser.parse_args()

    await test_daemon(args.daemon, use_debug=False)
    await test_daemon(args.daemon, use_debug=True)
    await test_daemon_options(args.daemon)
    if args.integration:
        await test_embedded(args.integration)

    err_path = os.environ.get("BF_E2E_STDERR")
    if err_path and os.path.exists(err_path):
        with open(err_path) as f:
            check_sanitizer(f.read(), "daemon")

    print("ALL E2E TESTS PASSED")


if __name__ == "__main__":
    asyncio.run(amain())
