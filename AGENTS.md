# AGENTS.md

Operational guide for AI agents working in this repository. Everything below was
verified in this container (Ubuntu 24.04, g++ 13.3.0) unless marked otherwise.

## What this repo is

Bonefish — an open-source C++20 WAMP router (broker + dealer, WAMP v2) by
Topology LP, Apache-2.0. It builds as a static library (`src/`), a standalone
daemon (`daemon/`), an in-process/embedded example (`examples/integration.cpp`),
and an interactive websocket test client (`test/functional/websocket/`).
The tree was modernized off Boost to standalone Asio (branch history: see
`MODERNIZATION_RESULT.md`). There are unit tests and an end-to-end driver, all
registered with CTest.

See the companion documents: `OVERVIEW.md` (architecture), `BUILD.md` (verified
build/test steps), `BOOST_AUDIT.md` (migration record), `MODERNIZATION.md`
(upgrade plan + known bugs), `MODERNIZATION_RESULT.md` (outcome), `PROGRESS.md`
(milestone log).

## Environment constraints (verified)

- `cmake` may be absent and there is no working `sudo` here. Use a venv:
  `python3 -m venv /tmp/opencode/venv && /tmp/opencode/venv/bin/pip install "cmake==3.31.6"`.
  Project requires CMake >= 3.16 (any distro CMake works).
- **No Boost needed.** All dependencies are git submodules under
  `third-party/` (asio 1.30.2, websocketpp develop bce2586, msgpack-c,
  rapidjson, cppcodec, json-msgpack). Do not reintroduce Boost.
- Tests that use the network need python3 with `msgpack` and `websockets`
  (venv ok); CMake's `find_package(Python3)` picks it up and registers the
  e2e test only if found.

## First steps

```bash
git submodule update --init --recursive
git submodule status   # all six must show no leading +/- (correct SHA checked out)
```

## Build (verified working)

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j
```

Clean under `-Wall -Werror` (third-party includes are SYSTEM-qualified; no
`-Wno-error`, no external libraries). Outputs: `build/src/libbonefish.a`,
`build/daemon/bonefish`, `build/examples/integration`,
`build/test/functional/websocket/client`.

`cmake --install` exports a self-contained `bonefish::bonefish` package (all
headers + vendored third-party headers + `ASIO_STANDALONE`). Portability:
`test/portability/gcc11-toolchain.cmake` (with `GCC11_ROOT=<prefix>`) builds
with stock Ubuntu 22.04 g++ 11.4.0 — verified green. CI:
`.github/workflows/ci.yml` (22.04/24.04 × Debug/Release + ASan/UBSan).

## Testing (all verified green, incl. under ASan+UBSan)

```bash
cd build && ctest --output-on-failure   # 4 unit + e2e_all + test_consumer
```

- `test/unit/`: message wire format, WAMP URI, trace formatter, native
  connect/disconnect. Framework: none (`test/unit/bonefish_test.hpp`).
- `test/e2e/e2e_driver.py`: starts the daemon itself; rawsocket TCP + unix
  socket + websocket-JSON full flows (incl. 512 KiB payload), CLI option
  regressions, embedded example. Set `BF_E2E_STDERR=<file>` to capture daemon
  stderr; the driver then fails on any sanitizer report.
- `test_consumer` (`test/consumer/` + `run.cmake`): stage-installs the build
  tree and configures/builds/runs an **independent** `find_package(bonefish)`
  project against `bonefish::bonefish`. Inherits the parent compiler,
  toolchain (`GCC11_ROOT` env is forwarded) and sanitizer flags.
- Sanitizer recipe is in BUILD.md.

## Running & verifying

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
  default; `--no-json` / `--no-msgpack` disable). These flags affect websocket
  only — rawsocket is msgpack-only and its handshake is rejected (0x7F100000)
  when msgpack serialization is disabled.
- Message codes: HELLO=1, WELCOME=2, ABORT=3, ERROR=8, PUBLISH=16,
  PUBLISHED=17, SUBSCRIBE=32, SUBSCRIBED=33, EVENT=36, CALL=48, RESULT=50,
  REGISTER=64, REGISTERED=65, INVOCATION=68, YIELD=70.
- Field orders: `[HELLO, Realm, Details]`, `[WELCOME, Session, Details]`,
  `[REGISTER, Request, Options, Procedure]`,
  `[INVOCATION, Request, Registration, Details, Args]`,
  `[EVENT, Subscription, Publication, Details, Args]`.
- Details/options maps must be msgpack maps (message setters reject nil).

## Code map & conventions

- `daemon/` — standalone server: `bonefish.cpp` (main + hand-rolled CLI
  parser), `daemon.{hpp,cpp}` (one `asio::io_context`,
  `asio::executor_work_guard`, SIGINT/SIGTERM `signal_set`), `daemon_options.cpp`.
- `src/bonefish/` — library. Notable subsystems: `router/` (wamp_router,
  broker/dealer attach), `broker/`, `dealer/` (call timeout via
  `asio::steady_timer` in `dealer/wamp_dealer_invocation.hpp`), `rawsocket/`,
  `websocket/`, `native/` (in-process API with `std::future`),
  `serialization/` (msgpack 1.x/`object_handle` API + rapidjson/json-msgpack),
  `messages/` (hand-rolled WAMP message classes), `trace/` (BONEFISH_TRACE,
  std-based `%N%` formatter), `identifiers/`, `roles/`, `session/`,
  `utility/` (URI validation, `std::regex`).
- Style: 4-space indent, `m_` member prefix, `snake_case`, one class per
  file with `.hpp`-only classes where small, handler + weak_ptr /
  shared_from_this async idiom throughout.
- `wamp_session.hpp` includes `websocketpp/server.hpp`, so **every** TU that
  touches sessions needs websocketpp headers on the include path.
- Trace debugging: `BONEFISH_TRACE` macro; enable at runtime with `-d`/`--debug`
  (or `bonefish::trace::set_enabled(true)` in embedded use).

## Do / avoid

- Do not modify `third-party/*` submodules to make the build pass; they are
  pinned deliberately (websocketpp develop bce2586 is what makes C++20 +
  standalone Asio compile; asio 1.30.2 matches it).
- Do not reintroduce Boost: no `boost/` includes, no Boost targets anywhere
  (`grep -rn "boost/" src daemon examples test` must stay empty).
- Do not assume `test/smoke/*.py` work: they are Python 2 era autobahn
  scripts and were not exercised.
- `AUTHENTICATE`, `CANCEL` (and `INTERRUPT` sending) are not implemented in the
  message processor — see MODERNIZATION.md before touching auth behavior.
