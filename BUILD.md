# BUILD.md

Everything below was executed in this container (Ubuntu 24.04, g++ 13.3.0,
bash) unless stated otherwise. Paths under `/tmp/opencode` are this session's
scratch locations — substitute your own.

## Toolchain (verified)

| Tool | Status |
|---|---|
| g++ 13.3.0, make, python3, git | present |
| cmake | **absent**; installed via `python3 -m venv /tmp/opencode/venv && /tmp/opencode/venv/bin/pip install "cmake==3.31.6"` |
| clang, ninja, system boost | absent; no `sudo` available |
| CMake 4.x | cannot configure this project (`cmake_minimum_required(VERSION 2.8)` at `CMakeLists.txt:1`, removed `FindBoost`) — use CMake 3.x |

## Submodules

```bash
git submodule update --init --recursive
git submodule status
```

All five must be at the recorded gitlink SHAs (no leading `+`). If
`third-party/websocketpp` is detached at a different commit:
`git -C third-party/websocketpp checkout 378437aecdcb1dfe62096ffd5d944bf1f640ccc3`.

Pinned third-party versions (`git submodule status`):

```
json-msgpack 55a6d772   cppcodec 61d9b044   rapidjson f54b0e47 (1.1.0)
websocketpp  378437ae   (0.3.0-dev, 2015)   msgpack-c e59f6e67 (1.3.0-54)
```

`msgpack-c` at that SHA vendors `boost/predef` + `boost/preprocess` headers and
uses the **msgpack 1.x C++ API** (`msgpack::unpacked`, etc. — see
`src/bonefish/serialization/msgpack_serializer.cpp`).

## Boost: version is pinned by third-party code

The vendored websocketpp 0.3.0-dev constructs asio objects as
`acceptor(lib::ref(*io_service))` / `work(lib::ref(...))` /
`resolver(lib::ref(...))`
(`third-party/websocketpp/websocketpp/transport/asio/endpoint.hpp:194,663,830`;
`connection.hpp:465`). Old service-based asio accepted that; the
executor-based asio from **Boost 1.66+** rejects it.

Verified compatibility matrix (headers + `b2` build with g++ 13.3,
static libs, then compile of `websocketpp/server.hpp` TU and/or full build):

| Boost | Result |
|---|---|
| 1.58.0 | used by the old Travis CI config (`.travis.yml`); untested here |
| **1.65.1** | **works — full build + all runtime tests green (used below)** |
| 1.71.0 | fails to *build*: `boost/thread/pthread/thread_data.hpp` `#if PTHREAD_STACK_MIN > 0` vs glibc 2.34+ dynamic `PTHREAD_STACK_MIN` (fixed upstream in 1.72) |
| 1.73.0 | builds, but websocketpp TU fails: `no matching function for call to basic_stream_socket(std::reference_wrapper<io_context>)` (plus `basic_socket_acceptor`, `basic_waitable_timer`, `work`) |
| 1.74.0 | same failure as 1.73 (reference_wrapper is not accepted by these ctors in 1.74 either) |
| 1.85.0 | same failure |

Conclusion: **use Boost 1.58–1.65** with this tree. Bumping Boost requires
bumping websocketpp (see MODERNIZATION.md).

### Building Boost 1.65.1 (verified, ~5 min)

```bash
cd /tmp/opencode && wget -q https://archives.boost.io/release/1.65.1/source/boost_1_65_1.tar.bz2
tar xjf boost_1_65_1.tar.bz2 && cd boost_1_65_1
./bootstrap.sh --with-libraries=program_options,system,thread,chrono,atomic,date_time
./b2 -j18 -d0 --with-program_options --with-system --with-thread --with-chrono \
     --with-atomic --with-date_time link=static variant=release threading=multi \
     install --prefix=/tmp/opencode/boost
```

Install prefix must be clean between Boost versions (b2 install does not
remove the previous headers; a polluted `/tmp/opencode/boost` with 1.74
headers + 1.65 libs produced confusing `io_context` errors until reinstalled
into an empty prefix).

## Configure + build (verified)

```bash
export PATH=/tmp/opencode/venv/bin:$PATH
cmake -S . -B build -DBOOST_ROOT=/tmp/opencode/boost \
      -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS="-Wno-error"
cmake --build build -j18
```

Configure output (verbatim): `Found Boost: /tmp/opencode/boost/include
(found version "1.65.1") found components: program_options system thread ...`
plus `find_package(RT)` (`cmake/Modules/FindRT.cmake`) and Threads.

Why `-Wno-error` is needed: the project compiles `-Wall -Werror
-Wno-unused-variable -std=c++11` (`CMakeLists.txt:41`); modern GCC turns ~37
warnings inside **third-party/Boost headers** into errors (msgpack-c
`-Wclass-memaccess` ×16, websocketpp/boost deprecated `binary_function` ×12,
boost 1.65 `assert_arg` `-Wparentheses` ×8, boost sign-compare/format-overflow
×5). Bonefish's own sources are warning-clean under `-Werror` with this combo
— no source edits required.

Build products: `build/src/libbonefish.a`, `build/daemon/bonefish`
(target `bonefish_bin`), `build/examples/integration`,
`build/test/functional/websocket/client`.

Options: `-Dshared=ON` (also flips Boost/RT static→shared,
`CMakeLists.txt:52-58`), `-DCMAKE_BUILD_TYPE=Release|Debug`, clang-only
`-Dstdlib=libstdc++|libc++`. GCC < 4.9 would add `boost_regex` and define
`USE_BOOST_REGEX` (`CMakeLists.txt:27-30`).

Install: `cmake --install build` (library to `lib/`, headers under `include/`,
`src/CMakeLists.txt:140-144`); downstream projects can use
`cmake/Modules/FindBonefish.cmake`.

## Runtime verification (all run, all passed)

1. Daemon: `./build/daemon/bonefish --help` works. RawSocket/msgpack E2E
   against `-t 18888`: HELLO→WELCOME, SUBSCRIBE→SUBSCRIBED,
   REGISTER→REGISTERED, CALL→INVOCATION→YIELD→RESULT, PUBLISH→EVENT
   (script: `/tmp/opencode/rawsocket_e2e.py`, msgpack via venv pip).
2. WebSocket/JSON E2E against `-w 18889` using python `websockets` with
   subprotocol `wamp.2.json`: SUBSCRIBED + EVENT received.
3. RawSocket over Unix socket (`-u path`) E2E: same flows pass.
4. Embedded `build/examples/integration` (listeners: TCP 8888, UDS
   `/tmp/bonefish.sock`, WS 9999): rawsocket E2E passed against it.

## Smoke / functional tests (not run)

- `test/smoke/{broker,dealer}/*.py` are Python 2-era autobahn/twisted scripts;
  no autobahn in this env — not exercised.
- `build/test/functional/websocket/client` is an interactive websocketpp
  tutorial-style client (compiles; not a regression test).
- There are no unit tests and no `enable_testing()`/CTest anywhere in the tree.
