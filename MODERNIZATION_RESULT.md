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

## Verification evidence

### Ubuntu 24.04 host, g++ 13.3.0, CMake 3.31.6

- Debug (`build-mod`): build clean under `-Wall -Werror`; `ctest` 6/6.
- Release (`build-rel`): `ctest` 6/6.
- ASan+UBSan (`build-asan`): `ctest` 6/6; daemon stderr captured with
  `BF_E2E_STDERR`, zero sanitizer reports.

### Ubuntu 22.04 toolchain, g++ 11.4.0 (verified via jammy packages)

No container runtime (docker/podman) or user namespaces are available in this
container, so 22.04 was validated by extracting the *official Ubuntu 22.04
packages* (`g++-11`/`gcc-11`/`libgcc-11-dev`/`libstdc++-11-dev`
11.4.0-1ubuntu1~22.04.3 plus jammy `libstdc++6`/`libgcc-s1` 12.3.0) and
building through `test/portability/gcc11-toolchain.cmake`
(`GCC11_ROOT=<prefix> cmake -S . -B build-gcc11
-DCMAKE_TOOLCHAIN_FILE=test/portability/gcc11-toolchain.cmake`):

- A probe confirmed GCC 11's libstdc++ has **no `<format>`**; the tree
  includes no `std::format`/`std::span`/`std::ranges`/`jthread`/concepts
  headers, so no compatibility shims were needed.
- `cmake --build build-gcc11 -j`: clean on the first attempt, zero warnings
  under `-Wall -Werror`.
- `ctest` in `build-gcc11`: 6/6.
- Re-run with jammy's runtime forced via `LD_LIBRARY_PATH`
  (`libstdc++.so.6.0.30`): 6/6; daemon/consumer binaries require at most
  `GLIBCXX_3.4.29` → runtime-ABI compatible with stock Ubuntu 22.04.

### Independent-project consumption

- The install now exports a CMake package: `bonefish::bonefish` with all
  headers plus the vendored third-party headers (standalone Asio,
  websocketpp, msgpack-c, rapidjson, cppcodec, json-msgpack) and
  `ASIO_STANDALONE`, so the package is self-contained; `FindBonefish.cmake`
  forwards to the config package when present.
- CTest `test_consumer`: stage-installs the build tree, then configures,
  builds and runs `test/consumer` (an independent `find_package(bonefish)`
  project) with the parent build's compiler/toolchain/sanitizer flags.
  Passes on GCC 13 (Debug/Release), GCC 11, and under ASan+UBSan.

### Boost audit (final)

- `grep -rn "boost/" src daemon examples test` → no matches.
- `grep -i boost` in all `CMakeLists.txt` + `cmake/` → no matches.
- `nm -C build-*/src/libbonefish.a | grep -ci boost` → 0;
  `ldd` of daemon/consumer: no Boost libs.

## CI

- `.github/workflows/ci.yml`: matrix over ubuntu-22.04/24.04 × Debug/Release
  (recursive submodules, `ctest --output-on-failure`) plus an ASan+UBSan job
  on 24.04 with `BF_E2E_STDERR` sanitizer scanning.

## Open items (out of scope)

- AUTHENTICATE / CANCEL / INTERRUPT (MODERNIZATION.md item 4) — protocol
  feature work.
- `wamp_hello_details.hpp::marshal` still `logic_error` (item 5) — only a
  trap for client-side message users.
- `test/smoke/*.py` are stale Python-2 autobahn scripts; consider replacing
  with the new e2e driver.
