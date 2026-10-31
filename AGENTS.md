# AGENTS.md

Operational guide for AI agents working in this repository. Everything below was
verified in this container (Ubuntu 24.04, g++ 13.3.0) unless marked otherwise.

## What this repo is

Bonefish — an open-source C++11 WAMP router (broker + dealer, WAMP v2) by
Topology LP, Apache-2.0. It builds as a static library (`src/`), a standalone
daemon (`daemon/`), an in-process/embedded example (`examples/integration.cpp`),
and an interactive websocket test client (`test/functional/websocket/`).
There are **no unit tests** and no CTest registration.

See the companion documents: `OVERVIEW.md` (architecture), `BUILD.md` (verified
build steps + Boost compatibility matrix), `BOOST_AUDIT.md` (Boost usage
inventory), `MODERNIZATION.md` (upgrade plan and known bugs).

## Environment constraints (verified)

- `cmake` is not installed and there is no working `sudo`. Use a venv:
  `python3 -m venv /tmp/opencode/venv && /tmp/opencode/venv/bin/pip install "cmake==3.31.6"`
  Then use `/tmp/opencode/venv/bin/cmake`. CMake 4.x cannot work: the project
  declares `cmake_minimum_required(VERSION 2.8)` (`CMakeLists.txt:1`) and uses
  the now-removed `FindBoost` module.
- No system Boost. Boost must be built in user space (see `BUILD.md`).
- **Boost version is pinned by third-party code, not by us.** The vendored
  websocketpp (0.3.0-dev, `third-party/websocketpp`) passes
  `lib::ref(*io_service)` to asio socket/acceptor/timer/work constructors. That
  only compiles with Boost < 1.66. Verified: Boost 1.65.1 works; 1.71 fails to
  build (glibc `PTHREAD_STACK_MIN`); 1.73/1.74/1.85 headers compile but break
  websocketpp with `no matching function for call to ...
  basic_stream_socket(std::reference_wrapper<...>)`. Do not bump Boost without
  also bumping websocketpp (see MODERNIZATION.md).

## First steps

```bash
git submodule update --init --recursive
git submodule status   # all five must show no leading +/- (correct SHA checked out)
```

Gotcha: `third-party/websocketpp` may end up checked out at a commit that does
not match the recorded gitlink; `git -C third-party/websocketpp checkout
378437aecdcb1dfe62096ffd5d944bf1f640ccc3` if status shows a mismatch.

## Build (verified working)

```bash
export PATH=/tmp/opencode/venv/bin:$PATH
cmake -S . -B build -DBOOST_ROOT=<boost-1.65.1-prefix> \
      -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS="-Wno-error"
cmake --build build -j
```

- `-Wno-error` is required only because the project's `-Wall -Werror`
  (`CMakeLists.txt:41`) turns ~37 third-party header warnings into errors
  (msgpack-c `-Wclass-memaccess`, websocketpp/boost deprecated
  `std::binary_function`, boost 1.65 `-Wparentheses` in `assert_arg`). Bonefish's
  own sources compile clean under `-Werror` (verified with Boost 1.65.1 on
  gcc-13). Prefer flag override over editing third-party or production sources.
- Outputs: `build/src/libbonefish.a`, `build/daemon/bonefish`,
  `build/examples/integration`, `build/test/functional/websocket/client`.

## Running & verifying (all verified end-to-end)

```bash
./build/daemon/bonefish -r default -t 18888        # rawsocket TCP
./build/daemon/bonefish -r default -w 18889        # websocket
./build/daemon/bonefish -r default -u /path/sock   # rawsocket over unix socket
```

Wire protocol essentials (conform to current WAMP v2 spec — verified against
the IETF draft and live against this router):

- RawSocket handshake: 4 bytes `0x7F LLLL SSSS` big-endian; serializer `0x2`
  = msgpack. Then every message: 4-byte big-endian length prefix + payload.
- Websocket: subprotocol `wamp.2.json` / `wamp.2.msgpack` (both registered by
  default; `--no-json` / `--no-msgpack` disable).
- Message codes: HELLO=1, WELCOME=2, ABORT=3, ERROR=8, PUBLISH=16,
  PUBLISHED=17, SUBSCRIBE=32, SUBSCRIBED=33, EVENT=36, CALL=48, RESULT=50,
  REGISTER=64, REGISTERED=65, INVOCATION=68, YIELD=70.
- Field orders: `[HELLO, Realm, Details]`, `[WELCOME, Session, Details]`,
  `[INVOCATION, Request, Registration, Details, Args]`,
  `[EVENT, Subscription, Publication, Details, Args]`.
- Details map must contain `"roles"`: `{"roles": {"caller": {"features": {}}, ...}}`.

End-to-end flows verified working: SUBSCRIBE/SUBSCRIBED, PUBLISH/EVENT,
REGISTER/REGISTERED, CALL → INVOCATION → YIELD → RESULT, on all three rawsocket
TCP / unix-socket / websocket transports, plus the embedded example binary.
A working Python msgpack test client used for verification:
`/tmp/opencode/rawsocket_e2e.py` (session artifact; recreate from the notes
above if missing).

## Code map & conventions

- `daemon/` — standalone server: `bonefish.cpp` (main, boost::program_options),
  `daemon.{hpp,cpp}` (one `boost::asio::io_service`, work object
  `daemon.cpp:106`, SIGINT/SIGTERM `signal_set`), `daemon_options.cpp`.
- `src/bonefish/` — library. Notable subsystems: `router/` (wamp_router,
  broker/dealer attach), `broker/`, `dealer/` (call timeout via
  `deadline_timer` in `dealer/wamp_dealer_invocation.hpp:54,86`),
  `rawsocket/`, `websocket/`, `native/` (in-process socketpair API with
  boost::future), `serialization/` (msgpack 1.x API + rapidjson/json-msgpack),
  `messages/` (hand-rolled WAMP message classes), `trace/` (BONEFISH_TRACE,
  boost::format), `identifiers/` (sequential/random generators),
  `roles/`, `session/`, `utility/` (URI validation).
- Style: 4-space indent, `m_` member prefix, `snake_case`, one class per
  file with `.hpp`-only classes where small (rawsocket connection, messages),
  handler + weak_ptr/shared_from_this async idiom throughout.
- `wamp_session.hpp:30-31` includes `websocketpp/server.hpp`, so **every** TU
  that touches sessions needs websocketpp headers on the include path.
- Trace debugging: `BONEFISH_TRACE` macro; enable at runtime with `-d`/`--debug`
  (or `bonefish::trace::set_enabled(true)` in embedded use; `BONEFISH_TRACE` env
  var in the example).

## Do / avoid

- Do not modify `third-party/*` submodules or production sources to make the
  build pass; use `-DCMAKE_CXX_FLAGS=-Wno-error` and correct Boost version.
- Do not assume `test/smoke/*.py` work: they are Python 2 era autobahn
  scripts and were not exercised here.
- `AUTHENTICATE`, `CANCEL` (and `INTERRUPT` sending) are not implemented in the
  message processor — see MODERNIZATION.md before touching auth behavior.
