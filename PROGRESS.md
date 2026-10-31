# PROGRESS.md — Bonefish modernization (branch: feature/modernize-cpp17)

## Plan (decided, updated during execution)
- Goal: zero Boost. Standalone Asio 1.30.2 (vendored submodule, header-only)
  replaces Boost.Asio; std::future/promise replace boost futures (no
  `.then()`/`when_all` in the tree, so std::future is sufficient);
  std::error_code; lambdas for boost::bind; hand-rolled CLI parser for
  program_options; trace.hpp keeps `%N%` semantics implemented with std;
  std::regex for wamp_uri (USE_BOOST_REGEX branch deleted).
- C++20 is the minimum standard (GCC 11+ / Clang 14+; CMake >= 3.16).
- websocketpp submodule: 378437ae (0.3.0-dev) -> origin/develop tip bce2586
  (C++20-clean, standalone-Asio compatible). rapidjson/msgpack-c/cppcodec/
  json-msgpack stay pinned (no Boost linkage; msgpack 1.x API kept, vendored
  checkout is 1.3.0-dev with object_handle API).

## Milestones
- [x] M0 baseline: docs, build green w/ Boost 1.65.1 (`build/`), E2E green.
- [x] M1 vendored third-party/asio (1.30.2) + websocketpp develop bce2586.
- [x] M2 CMake: min 3.16, C++20, `asio` INTERFACE target, ASIO_STANDALONE,
      SYSTEM third-party includes, enable_testing(), test subdirs.
- [x] M3 source: asio namespace/io_context/steady_timer, std::error_code.
- [x] M4 source: std::future, std-based trace, lambdas, CLI parser, regex.
- [x] M5 bug fixes + regression tests:
      - RawSocket framing UB (reserve -> resize) `rawsocket_connection.hpp`
      - native disconnect inverted weak_ptr + missing throw
      - `wamp_register_message::marshal()` dropped m_procedure
      - null-serializer segfault with `--no-msgpack` (rawsocket handshake now
        rejects; both transports guard) — found by the E2E suite
      - unit tests: messages wire format, wamp_uri, trace format, native
        disconnect (test/unit/)
- [x] M6 E2E suite (test/e2e/e2e_driver.py) wired into CTest: rawsocket TCP +
      UDS + websocket-JSON full pub/sub + RPC flows incl. 512 KiB payload,
      debug mode, CLI/option regressions, embedded example. All 5 ctest tests
      pass in `build-mod` and under ASan+UBSan in `build-asan` (daemon stderr
      captured and scanned: no sanitizer reports).
- [x] M7 docs updated + MODERNIZATION_RESULT.md.
- [x] M8 release readiness:
      - CMake package export: `bonefish::bonefish` + self-contained header
        install (vendored third-party headers + ASIO_STANDALONE);
        FindBonefish.cmake forwards to config mode.
      - `test_consumer` CTest: independent `find_package(bonefish)` project
        (`test/consumer/`) built against a stage-install; inherits compiler/
        toolchain/sanitizer flags.
      - GCC 11 verified for real: official jammy g++ 11.4.0-1ubuntu1~22.04.3
        debs extracted, built via `test/portability/gcc11-toolchain.cmake`;
        all 6 ctest tests green (also against jammy libstdc++ 12.3.0 runtime;
        max ABI requirement GLIBCXX_3.4.29).
      - GCC 13: Debug + Release + ASan/UBSan all 6/6.
      - Boost audit final: no matches in src/CMake, no boost symbols in
        libbonefish.a or linked binaries.
      - CI: `.github/workflows/ci.yml` (ubuntu-22.04/24.04 x Debug/Release +
        ASan/UBSan job).

## Verified environment
- Ubuntu 24.04, g++ 13.3.0. NO sudo; cmake via /tmp/opencode/venv (3.31.6).
- Ubuntu 22.04 validated without a container: jammy GCC 11 .deb packages
  extracted to /tmp/opencode/gcc11 (see M8 + BUILD.md "Portability").
- Legacy Boost 1.65.1 build (`build/`) exists only as the pre-modernization
  baseline; it no longer matches the modernized sources (expected).

## Current blocker / next step
- No blocker. Possible follow-ups (out of scope): implement AUTHENTICATE /
  CANCEL / INTERRUPT (protocol work); replace stale test/smoke scripts.
