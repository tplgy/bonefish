# MODERNIZATION.md

> **Status (branch feature/modernize-cpp17): executed.** Steps 1–3 below are
> done: websocketpp repinned to develop (bce2586), standalone Asio 1.30.2
> vendored, zero Boost, C++20, ctest added. Bugs 1, 2, 3 and 6 are fixed with
> regression tests (plus two more found by the new test suite:
> `wamp_register_message::marshal()` dropped the procedure, and a
> null-serializer segfault behind `--no-msgpack`). Bug 4 (AUTHENTICATE /
> CANCEL / INTERRUPT) remains open — protocol feature work. See
> MODERNIZATION_RESULT.md for the outcome and test evidence.

Upgrade analysis and plan. All items verified by reading the code; the three
confirmed bugs have exact locations. See BOOST_AUDIT.md for the Boost/Asio
inventory and BUILD.md for the dependency ceiling.

## Why the tree is stuck (and what unblocks it)

Everything Boost-modernizing hinges on one blocker: **websocketpp 0.3.0-dev
(2015) is incompatible with Boost ≥ 1.66** (it passes `lib::ref(*io_service)`
to asio socket/acceptor/timer/work ctors — see
BUILD.md matrix). Until websocketpp is bumped, Boost is frozen at 1.58–1.65
and no io_context/std::error_code migration is possible.

Good news first: the wire layer is *not* broken. Message codes (`PUBLISH=16,
SUBSCRIBE=32, EVENT=36, ...` in `messages/wamp_message_type.hpp`) and field
orders (`[HELLO, Realm, Details]`, `[INVOCATION, Request, Registration,
Details, Args]`) match the current WAMP v2 spec; end-to-end flows were
verified live (BUILD.md).

## Confirmed bugs / code debt (fix early, they are cheap)

1. **Uninitialized-buffer read path** — `rawsocket/rawsocket_connection.hpp:265`
   `m_message_buffer.reserve(message_length)` then
   `async_read(m_message_buffer.data(), message_length, ...)` (line 278).
   `reserve` does not change `size()`; the socket writes into capacity behind
   `data()` while the vector's size stays 0 — technically UB.
   Fix: `resize(message_length)`.
2. **Missing throw** — `native/native_server_impl.cpp:164`:
   `std::invalid_argument("server endpoint does not exist");` is constructed
   and discarded; execution falls through to `itr->second` with an
   end-iterator (UB). Fix: `throw`.
3. **Inverted weak_ptr check** — `native/native_server_impl.cpp:94-95`:
   `if (shared_this) { disconnected->set_exception("disconnect failed"); ... }`
   fails the promise when the server object *is* alive; the branch was
   intended for the lock-failure (null) case. Fix: `if (!shared_this)`.
4. **Unimplemented protocol branches** — `common/wamp_message_processor.cpp:49,60`:
   `AUTHENTICATE` and `CANCEL` are empty `break` cases; `INTERRUPT` is never
   sent. Consequences: auth is silently impossible, and a caller cannot cancel
   an in-flight call (the dealer keeps the invocation alive until YIELD or
   timeout). Decide: implement (advanced-profile feature) or ABORT with a
   `wamp.error.not_implemented`-style reason.
5. `messages/wamp_hello_details.hpp`: `marshal` throws `logic_error`
   ("not implemented") — harmless for the router, but a trap for embedded
   users building clients on the message layer.
6. `rawsocket_server_impl.cpp:201-208` silently forces msgpack for rawsocket
   (JSON rawsocket requests are rejected) while the daemon advertises
   `--no-msgpack`; a JSON-only daemon still fails rawsocket clients.
   Either wire the serializer choice or document it.

## Dependency upgrades (order matters)

### Step 1 — submodules (prerequisite for all C++ modernization)

- **websocketpp 378437a (0.3.0-dev) → v0.8.2** (`0.3.0` was never released;
  0.8.2 is the maintained head). API deltas in config/server classes are
  modest; the vendored `asio_no_tls` config used by
  `src/bonefish/websocket/websocket_config.hpp` still exists. This lifts the
  Boost <1.66 ceiling.
- **msgpack-c e59f6e67 (1.3.0-54) → 6.0.0**. The pinned tree uses the 1.x API
  (`msgpack::unpacked`, `item.get()`, `item.zone()` —
  `serialization/msgpack_serializer.cpp:42-58`); 3.x+ replaced it with
  `msgpack::object_handle`/`zone`. Touch points: `msgpack_serializer.cpp`,
  the message classes' `msgpack::zone` handling, `reference_func` signature.
- **rapidjson f54b0e47 (1.1.0) → master (5.x)**: JSON bridge in
  `serialization/json_serializer.cpp` + `third-party/json-msgpack` (the latter
  is unmaintained; plan to drop it or vendor a maintained msgpack↔rapidjson
  bridge).
- **cppcodec 61d9b044**: no-op bump.

### Step 2 — Boost 1.65.1 → modern Boost (or standalone Asio)

With websocketpp 0.8.x, move to a current Boost (1.8x) or, better long-term,
standalone Asio / C++20 coroutine Asio (`co_spawn`) to shed the rest:

- `io_service` → `io_context`; `io_service::work` →
  `boost::asio::executor_work_guard` (~30 call sites — see BOOST_AUDIT.md).
- `deadline_timer` + `boost::posix_time` → `steady_timer` +
  `std::chrono::milliseconds` (`dealer/wamp_dealer_invocation.hpp:54,86`,
  `dealer/wamp_dealer.cpp:501-515`).
- `boost::bind` → lambdas (`daemon/daemon.cpp:108,124,177`).
- `boost::system::error_code` → `std::error_code` (free once asio is modern).
- Delete the GCC<4.9 `USE_BOOST_REGEX` branch (`CMakeLists.txt:27-30`,
  `utility/wamp_uri.cpp:19-30`) — the `std::regex` path is already the
  default; this removes the `regex` component entirely.
- `boost::program_options` → keep (still maintained) or swap for CLI11 in the
  single file `daemon/bonefish.cpp`.
- `boost::future/promise` (native API): the `.then()` continuations have no
  std::future equivalent — either keep Boost.Thread here, or redesign the
  native connect API around callbacks before switching to `std::future`.
- `trace.hpp`: `boost::format` + `BOOST_PP_VARIADIC_SIZE` → `std::format`
  (C++20) or {fmt}, with `__VA_OPT__`/fold-based arg conversion. ~200+ trace
  call sites use `%1%` style; the conversion is mechanical but repo-wide.

### Step 3 — language/build

- `-std=c++11` → C++17 (or 20 if fmt→std::format). Replace the remaining
  `new`s with `make_unique` (`daemon.cpp:106`, session/message factories).
- `cmake_minimum_required(VERSION 2.8)` → `3.16+`; rewrite the compiler-flag
  soup in `CMakeLists.txt:11-47` as per-target `target_compile_features`/
  `target_compile_options`; fix the unindented/brittle GCC-branch at
  `CMakeLists.txt:27-31`; drop `FindBoost`-era `include_directories` for
  imported targets (`Boost::boost`, `Boost::program_options`).
  (CMake 3.30+ deprecates `FindBoost` via CMP0167.)
- CI: `.travis.yml` is dead (travis-ci.org, g++-4.8, Boost 1.58). Replace with
  GitHub Actions: matrix over GCC/Clang × Boost 1.8x × CMake 3.2x, plus the
  python msgpack E2E from BUILD.md as an actual regression test.
- Testing: no unit tests exist (ROADMAP.md flagged this; Aeryn was a
  candidate). With C++17, Catch2 or GoogleTest + CTest; message
  marshal/unmarshal round-trips and the rawsocket framing bug above are the
  obvious first subjects.

## Suggested sequence (lowest risk first)

1. Bug fixes 1–3 + regression tests (no dependency churn).
2. Modern CMake, C++17, delete regex branch, CI replacement.
3. websocketpp bump (single biggest unlock) + verify WS/rawsocket E2E.
4. msgpack-c bump (API churn in serialization layer).
5. io_context/chrono/lambdas/std::error_code sweep.
6. trace formatting; program_options/native-futures decisions.
