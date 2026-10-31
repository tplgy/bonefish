# MODERNIZATION_RESULT.md

Outcome of the C++20 / zero-Boost modernization on branch
`feature/modernize-cpp17` (base: `0bfcc8b`).

## What changed

| Area | Before | After |
|---|---|---|
| Language standard | C++11 (`cmake_minimum_required(2.8)`) | **C++20**, CMake >= 3.16, `CXX_EXTENSIONS OFF` |
| Async core | Boost.Asio (`io_service`, `deadline_timer`) | standalone **Asio 1.30.2** (vendored submodule, header-only, `ASIO_STANDALONE`), `io_context`, `steady_timer` |
| Futures | `boost::future/promise` (+ BOOST_THREAD macros) | `std::future/promise` (no `.then`/`when_all` was used in-tree) |
| Errors | `boost::system::error_code` | `std::error_code` |
| Callbacks | `boost::bind` | lambdas |
| CLI | `boost::program_options` | hand-rolled parser in `daemon/bonefish.cpp` (same flags/exit codes) |
| Trace | `boost::format` + BOOST_PP varargs | std `%N%` formatter in `trace/trace.hpp` |
| URI validation | `boost::regex` (+ `USE_BOOST_REGEX`) | `std::regex` |
| websocketpp | 378437ae (0.3.0-dev, 2015) | develop tip **bce2586** (C++20-clean template-id fix, standalone Asio) |
| Boost anywhere | required (program_options/system/thread/...) | **gone** — `ldd build-mod/daemon/bonefish` has zero Boost libs |

Unchanged: wire protocol, message classes, public API shapes (`asio::` types
in headers replace `boost::asio::`), transport topology, msgpack-c/rapidjson/
cppcodec/json-msgpack pins.

## Bugs fixed (each with a regression test)

1. RawSocket framing UB: `reserve()` → `resize()` before async read
   (`rawsocket_connection.hpp`) — 512 KiB E2E payload under ASan is green.
2. Native disconnect on unknown endpoint constructed `std::invalid_argument`
   without throwing (end-iterator deref UB) — now throws
   (`native_server_impl.cpp:163`).
3. Native disconnect inverted `weak_ptr` check made every disconnect of a
   live endpoint fail — fixed (`native_server_impl.cpp:93`).
4. `wamp_register_message::marshal()` omitted `m_procedure` (3 of 4 fields;
   violates `[REGISTER, Request, Options, Procedure]`) — fixed
   (`wamp_register_message.hpp:91`).
5. **Segfault** (SIGSEGV): daemon with `--no-msgpack` + a rawsocket client
   negotiated the handshake then dereferenced the null msgpack serializer in
   `on_message`. Fixed: rawsocket handshake rejects (0x7F100000) when msgpack
   is disabled, and both rawsocket/websocket `on_message` guard against a
   missing serializer.
6. Documented/locked behavior: `--no-json`/`--no-msgpack` apply to the
   websocket subprotocol negotiation only; RawSocket is msgpack-only
   (pre-existing "code debt" item 6 in MODERNIZATION.md).

## Test coverage (new)

- `test/unit/` (no framework, single header `bonefish_test.hpp`):
  `test_messages` (wire codes/field orders + msgpack round-trips),
  `test_wamp_uri`, `test_trace`, `test_native_disconnect`.
- `test/e2e/e2e_driver.py` (python3 + msgpack + websockets): daemon over
  RawSocket TCP, RawSocket UDS, WebSocket/JSON — full HELLO/SUBSCRIBE/
  PUBLISH/REGISTER/CALL/YIELD flows incl. large payloads; CLI `--help`,
  bad-option, `--no-msgpack`/`--no-json` subprotocol selection, rawsocket
  msgpack-only rejection; embedded `examples/integration`. Optional
  `BF_E2E_STDERR` capture fails on any sanitizer report.
- Registered with CTest (`enable_testing()` + `test/{unit,e2e}`).

## Verification evidence (this container: Ubuntu 24.04, g++ 13.3.0, CMake 3.31.6)

- `cmake --build build-mod -j` clean under `-Wall -Werror`.
- `ctest`: 5/5 pass.
- ASan+UBSan build (`build-asan`): 5/5 pass, daemon stderr captured,
  zero sanitizer reports.
- `ldd build-mod/daemon/bonefish | grep -ci boost` → 0.

## Reproducing on Ubuntu 22.04 (GCC 11) / 24.04 (GCC 13)

```bash
# apt install g++ cmake git python3 python3-venv   (cmake >= 3.16 distro pkg is fine)
git clone --recurse-submodules <repo> && cd bonefish
python3 -m venv .venv && .venv/bin/pip install msgpack websockets
source .venv/bin/activate          # so CMake finds the python with the deps
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
cd build && ctest --output-on-failure
```

Note: this container could not run Ubuntu 22.04/GCC 11 directly. The tree
uses no C++20 features beyond what GCC 11 implements (no concepts-heavy code,
no ranges/`std::format`, no `std::span`/`jthread`), and Asio 1.30.2 /
websocketpp develop both support C++20 on GCC 11 upstream, so GCC 11 is
expected to build; treat as the first thing to check in CI.

## Open items (out of scope)

- AUTHENTICATE / CANCEL / INTERRUPT (MODERNIZATION.md item 4) — protocol
  feature work.
- `wamp_hello_details.hpp::marshal` still `logic_error` (item 5) — only a
  trap for client-side message users.
- CI wiring (GitHub Actions matrix 22.04/24.04 + sanitizers) — the ctest
  suites are ready to plug in.
- `test/smoke/*.py` are stale Python-2 autobahn scripts; consider replacing
  with the new e2e driver.
