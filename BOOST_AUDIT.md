# BOOST_AUDIT.md

> **Status (branch feature/modernize-cpp17): COMPLETE.** Every Boost
> dependency below has been removed from `src/`, `daemon/`, `examples/` and
> `test/`; the build no longer finds or links Boost (verified: `ldd` of
> `build-mod/daemon/bonefish` shows zero Boost libraries). Replacements:
> standalone Asio 1.30.2 (vendored), `std::future/promise` (no `.then()` /
> `when_all` was in fact used anywhere), `std::error_code`, lambdas for
> `boost::bind`, hand-rolled CLI parser for program_options, std-based
> trace formatter for `boost::format`/BOOST_PP, `std::regex` for
> USE_BOOST_REGEX. The inventory below is kept as the migration record.

Inventory of Boost usage in this tree (verified by grep/reading sources), with
replacement notes. See MODERNIZATION.md for a migration ordering.

## Direct Boost usage in first-party code

### Boost.Asio (the dominant dependency)

| API | Where |
|---|---|
| `io_service` (member/ref) | `daemon/daemon.hpp:24,45,53`; `src/bonefish/router/wamp_router.hpp:20,47`; `wamp_router_impl`, `dealer/wamp_dealer.hpp:28,48`, `rawsocket/tcp_listener`, `rawsocket/uds_listener`, `native/native_connection.hpp:99`, `websocket/websocket_server_impl`, `examples/integration.cpp:27,43` |
| `io_service::work` | `daemon/daemon.cpp:106`, `daemon.hpp:54` (keeps `io_service.run()` alive) |
| `signal_set` (SIGINT/SIGTERM) | `daemon/daemon.cpp:108,177` |
| `ip::tcp`, `local::stream_protocol` | tcp/uds listeners, `daemon/daemon.cpp` |
| `boost::asio::buffer`, async read/write, error codes | `rawsocket/tcp_connection.hpp:24-25,70,78`, `uds_connection.hpp`, `rawsocket_connection.hpp:24-26` |
| `deadline_timer` + `boost::posix_time::milliseconds` | `dealer/wamp_dealer_invocation.hpp:54,86`; handler at `dealer/wamp_dealer.cpp:501-515` (`boost::asio::error::operation_aborted`) |
| `posix` / socketpair plumbing for `native/` | `native/native_connection.cpp` |

### Boost.Thread / future

`native/native_endpoint.hpp:21-23` sets `BOOST_THREAD_PROVIDES_FUTURE`,
`..._FUTURE_CONTINUATION`, `..._FUTURE_WHEN_ALL_WHEN_ANY` before
`<boost/thread/future.hpp>`; `native_endpoint_future/_promise` aliases
(lines 38, 44). `native/native_server_impl.cpp:91` uses
`boost::promise<void>` + `boost::copy_exception` (needs `boost/thread`,
links `libboost_thread`, `CMakeLists.txt:7`).
Note the `.then()` continuation API has no std::future equivalent.

### Boost.ProgramOptions

`daemon/bonefish.cpp:20` — all CLI parsing.

### Boost.Bind

`daemon/daemon.cpp` (`boost::bind(&daemon::termination_signal_handler, ...)`,
lines 108/124/177). Only direct use in first-party code (websocketpp uses its
own `lib::bind` which maps to `std::bind` under C++11).

### Boost.System / error codes

`boost::system::error_code` is the callback currency of every socket handler
(e.g. `rawsocket_connection.hpp:117,149-166`, all listener/connection
classes). Header `<boost/system/error_code.hpp>`.

### Boost.Format + Preprocessor

`trace/trace.hpp:20-23`: `boost::format` + `BOOST_PP_VARIADIC_SIZE` drive the
`BONEFISH_TRACE` macro (`%1%`-style format strings are pervasive in the code
base — every `.cpp` in router/broker/dealer/rawsocket emits them).

### Boost.Exception

`native/native_server_impl.cpp:31` — for `boost::copy_exception` used with
boost::promise (lines 66, 73, 95, 102).

### Boost.Regex (conditional, dead on this toolchain)

`CMakeLists.txt:27-30` adds `regex` to components and defines
`USE_BOOST_REGEX` only for GCC < 4.9; `utility/wamp_uri.cpp:19-30` then uses
`boost::regex`, else `std::regex`. With g++ 13 the std path is used; Boost is
never linked.

## Transitive Boost usage (third-party submodules)

- **websocketpp 0.3.0-dev** (`third-party/websocketpp`, default
  `asio_no_tls` config — see `src/bonefish/websocket/websocket_config.hpp` and
  the leak at `src/bonefish/session/wamp_session.hpp:30-31`): uses
  `boost::asio` everywhere (`io_service`, `ip::tcp`, `steady_timer`, strands,
  `io_service::work`) and `boost::system::error_code`. This is the component
  that pins Boost < 1.66 (constructor pattern at
  `websocketpp/transport/asio/endpoint.hpp:194,663,830`).
- **msgpack-c** (pinned 1.3.0-54): vendors `boost/predef` +
  `boost/preprocess` headers internally; no compiled Boost.
- `test/functional/websocket/client.cpp:32` defines Asio headers via
  `boost/asio/impl/src.hpp` (websocketpp client config).

## Build-system coupling

- `CMakeLists.txt:7` — `set(BOOST_COMPONENTS program_options system thread)`
  (+`regex` on GCC<4.9); `find_package(Boost REQUIRED ...)` line 60;
  `Boost_USE_STATIC_LIBS ON` by default (`:52-56`).
- CMake's `FindBoost` module is used (no Boost-provided CMake config at these
  versions); CMake ≥3.30 deprecates it (CMP0167) — works but warns.
- Empirical version ceiling: Boost 1.65.1 OK; ≥1.66 breaks the websocketpp TU
  (executor-era asio dropped the service/reference_wrapper ctor forms); 1.71
  additionally fails to build on glibc ≥2.34.

## Replacement summary

| Boost feature | Modern replacement | Notes |
|---|---|---|
| `io_service`, `io_service::work` | `io_context`, `executor_work_guard` (Asio ≥1.66) | mechanical rename ~30 sites |
| `deadline_timer` + `posix_time` | `steady_timer` + `std::chrono` | 2 sites (dealer timeout) |
| `boost::system::error_code` | `std::error_code` | Asio 1.66+ already aliases to it; keep header |
| `boost::bind` | lambdas | 3 sites in daemon.cpp |
| `boost::future/promise` + `.then()` | `std::future` loses `.then()`/`when_all`; keep Boost.Thread or vendor a small continuation type | native API design decision |
| `boost::format` + `BOOST_PP_VARIADICS` | `std::format` (C++20) or fmt; variadic macro via `__VA_OPT__` | touches every trace call (`%1%` → `{}`) |
| `boost::program_options` | CLI11/cxxopts/argparse | single file (`daemon/bonefish.cpp`) |
| `boost::regex` branch | delete (`std::regex` path already default) | removes GCC<4.9 logic |
| websocketpp asio | bump websocketpp (0.8.x) → then Boost ≥1.66 or standalone Asio / C++20 coroutines | prerequisite for all asio changes |
