# PROGRESS.md — Bonefish modernization (branch: feature/modernize-cpp17)

## Plan (decided)
- Goal: zero Boost. Standalone Asio (header-only) replaces Boost.Asio;
  std::future/promise replace boost futures (verified: no `.then()`/`when_all`
  in the tree, so std::future is sufficient); std::error_code; lambdas for
  boost::bind; hand-rolled CLI parser for program_options; trace.hpp keeps
  `%N%` semantics but implemented with std (no boost::format/BOOST_PP);
  USE_BOOST_REGEX branch deleted.
- websocketpp submodule: bump 378437ae (0.3.0-dev) -> master (C++17 + modern
  Boost/standalone-Asio compatible). Submodules rapidjson/msgpack-c/cppcodec/
  json-msgpack stay pinned (no Boost linkage; msgpack 1.x API kept).
- CMake: min 3.16, C++17, target-based, ctest enabled, unit tests added
  (no external framework).
- Bug fixes (MODERNIZATION.md 1-3) with regression tests; #4 (AUTHENTICATE/
  CANCEL) left as-is (protocol feature work, not a bug fix).

## Milestones
- [x] M0 baseline: docs, build green w/ Boost 1.65.1, E2E green (this session
      pre-task, see BUILD.md). Branch + PROGRESS.md.
- [ ] M1 fetch third-party/asio + websocketpp master; .gitmodules update
- [ ] M2 CMake modernization (+ ASIO_STANDALONE, ctest)
- [ ] M3 source: asio namespace/io_context/steady_timer, std::error_code
- [ ] M4 source: std::future, trace, bind, program_options, regex
- [ ] M5 bug fixes + regression tests
- [ ] M6 E2E re-run (rawsocket msgpack / ws json / uds / embedded) + ASan/UBSan pass
- [ ] M7 docs update + MODERNIZATION_RESULT.md

## Verified environment
- Ubuntu 24.04, g++ 13.3.0. NO sudo; cmake via /tmp/opencode/venv (3.31.6).
- Boost 1.65.1 in /tmp/opencode/boost (baseline only). Ubuntu 22.04 NOT
  available in this container - repro instructions to go in MODERNIZATION_RESULT.md.
- E2E scripts: /tmp/opencode/rawsocket_e2e.py (+ ws/uds snippets in history;
  plan to move them into test/e2e/ for repeatability).

## Current blocker / next step
- NEXT: M1 clone asio + websocketpp master.
