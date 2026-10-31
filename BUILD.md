# BUILD.md

Modern (post-C++20-modernization) build. Everything below was executed in
this container (Ubuntu 24.04, g++ 13.3.0, bash) unless stated otherwise.
The pre-modernization Boost-based build is documented in git history
(`0bfcc8b Baseline before C++17 modernization`).

## Requirements (current tree)

- CMake >= 3.31 here (project declares `cmake_minimum_required(VERSION 3.16)`;
  CMake 4.x also works since `FindBoost` is gone, but was not tested).
- GCC 11+ / Clang 14+ (C++20). Verified with g++ 13.3.0 (Ubuntu 24.04) and
  with the official Ubuntu 22.04 g++ 11.4.0 packages (see *Portability*).
- **No Boost required.** All third-party deps are git submodules:

```
asio        1.30.2        (third-party/asio, header-only, standalone)
websocketpp develop bce2586 (C++20-clean, standalone-Asio aware)
msgpack-c   e59f6e67      (1.3.0-54; vendored checkout has object_handle API)
rapidjson   f54b0e47 (1.1.0), cppcodec 61d9b044, json-msgpack 55a6d772
```

## Submodules

```bash
git submodule update --init --recursive
git submodule status   # no leading +/- on any line
```

## Configure + build (verified)

```bash
cmake -S . -B build-mod -DCMAKE_BUILD_TYPE=Debug
cmake --build build-mod -j
```

The modern tree compiles clean with `-Wall -Werror` (third-party includes are
`SYSTEM`-qualified so their warnings are suppressed). No `-Wno-error`, no
`BOOST_ROOT`, no external libraries.

Build products: `build-mod/src/libbonefish.a`, `build-mod/daemon/bonefish`
(target `bonefish_bin`), `build-mod/examples/integration`,
`build-mod/test/functional/websocket/client`.

Install: `cmake --install build-mod --prefix <prefix>`. The install exports a
self-contained CMake package (all bonefish headers plus the vendored
header-only deps; `ASIO_STANDALONE` is part of the exported interface):

```cmake
find_package(bonefish REQUIRED)          # -DCMAKE_PREFIX_PATH=<prefix>
target_link_libraries(my_app PRIVATE bonefish::bonefish)
```

`cmake/Modules/FindBonefish.cmake` still works and forwards to the config
package when installed. `test/consumer/` is a minimal independent project
run by the `test_consumer` CTest against a stage-install of the build tree.

## Tests (verified green, unit + E2E, plain and under sanitizers)

```bash
cd build-mod && ctest --output-on-failure
```

6 tests: 4 unit (`test/unit/`: message wire format, WAMP URI validation,
trace formatter, native connect/disconnect), `e2e_all`
(`test/e2e/e2e_driver.py`: daemon over RawSocket TCP, RawSocket UDS, and
WebSocket/JSON — full HELLO/SUBSCRIBE/PUBLISH/REGISTER/CALL flows incl. a
512 KiB payload; CLI option regressions incl. `--no-msgpack`/`--no-json`
subprotocol selection; embedded `examples/integration`), and `test_consumer`
(independent `find_package(bonefish)` project built against the
stage-install). The driver needs
python3 with `msgpack` and `websockets` (installed in a venv here).

Sanitizer run (verified clean):

```bash
cmake -S . -B build-asan -DCMAKE_BUILD_TYPE=Debug \
      -DCMAKE_CXX_FLAGS="-fsanitize=address,undefined -fno-omit-frame-pointer -g" \
      -DCMAKE_EXE_LINKER_FLAGS="-fsanitize=address,undefined"
cmake --build build-asan -j
cd build-asan && ASAN_OPTIONS=detect_leaks=0 ctest --output-on-failure
```

E2E daemon stderr is discarded by default; set `BF_E2E_STDERR=<file>` to
capture it — the driver then fails the test if any sanitizer report appears.

## Running the daemon

```bash
./build-mod/daemon/bonefish -r default -t 18888        # rawsocket TCP (msgpack only)
./build-mod/daemon/bonefish -r default -w 18889        # websocket (json+msgpack)
./build-mod/daemon/bonefish -r default -u /path/sock   # rawsocket unix socket
```

`--no-json` / `--no-msgpack` control the **websocket** subprotocols; RawSocket
is msgpack-only by design and now rejects the handshake (0x7F100000) when
msgpack is disabled instead of crashing (fixed).

## Portability: GCC 11 / Ubuntu 22.04 (verified)

GitHub Actions CI (`.github/workflows/ci.yml`) covers ubuntu-22.04 (GCC 11)
and ubuntu-24.04 (GCC 13), Debug and Release, plus an ASan+UBSan job.

Locally (verified in this 24.04 container without a 22.04 container:
`test/portability/gcc11-toolchain.cmake` drives a build with the official
jammy `g++-11` 11.4.0-1ubuntu1~22.04.3 `.deb` packages extracted to a
prefix):

```bash
# extract g++-11/gcc-11/cpp-11/libgcc-11-dev/libstdc++-11-dev +
# jammy libstdc++6/libgcc-s1 debs into $GCC11_ROOT (dpkg -x)
GCC11_ROOT=$GCC11_ROOT cmake -S . -B build-gcc11 \
    -DCMAKE_TOOLCHAIN_FILE=test/portability/gcc11-toolchain.cmake \
    -DCMAKE_BUILD_TYPE=Debug
cmake --build build-gcc11 -j
cd build-gcc11 && ctest --output-on-failure    # 6/6 green
```

GCC 11's libstdc++ has no `<format>` — the tree deliberately uses none of
the GCC-11-incomplete C++20 library features (`std::format`, ranges,
`std::span`, `jthread`, ...). Binaries need at most `GLIBCXX_3.4.29`, i.e.
they run on stock 22.04.

## Not run / legacy

- `test/smoke/*.py`: Python 2-era autobahn scripts, not exercised.
- `test/functional/websocket/client`: interactive client, compiles, not a
  regression test.
