# OVERVIEW.md

Architecture and feature map of Bonefish, a C++11 WAMP v2 router (broker +
dealer) by Topology LP. All structural claims below were verified by reading
the source; line numbers refer to the current tree.

## What it is

A WAMP router that hosts one or more **realms**, each backed by a
`wamp_router` that attaches every session to both a **broker** (pub/sub) and a
**dealer** (RPC). It can run as a standalone daemon (`daemon/`) or be embedded
in-process (`examples/integration.cpp`).

Supported today (README + verified at runtime):

- Transports: RawSocket over TCP and Unix-domain sockets, WebSocket
  (`wamp.2.json` and `wamp.2.msgpack` subprotocols), and a process-local
  `native` socketpair API.
- Serializers: MessagePack and JSON (WebSocket only by default; rawsocket
  hard-requires msgpack — `rawsocket_server_impl.cpp:201-208`).
- RPC: REGISTER/CALL/INVOCATION/YIELD with call timeouts and progressive
  call results (ROADMAP.md lists RawSocket, call timeouts, progressive call
  results as the completed Advanced-Profile features).
- Pub/Sub: SUBSCRIBE/PUBLISH/EVENT. `PUBLISH.Options.acknowledge`
  (PUBLISHED) is **not** implemented (first item in ROADMAP.md).
- Not implemented: authentication (`AUTHENTICATE` is an empty `case` in
  `common/wamp_message_processor.cpp:49`), `CANCEL`, `INTERRUPT` (enum exists
  in `messages/wamp_message_type.hpp:56`, never sent), caller/exclusion roles,
  pattern-based matching, shared/clustered registrations (one callee per
  procedure — the dealer keeps a single session per procedure URI,
  `dealer/wamp_dealer.cpp:190-196`).

## Wire format (verified against the IETF draft, wamp-proto.org)

Message codes (enum `messages/wamp_message_type.hpp:33-57`) match the current
spec: HELLO=1, WELCOME=2, ABORT=3, CHALLENGE=4, AUTHENTICATE=5, GOODBYE=6,
HEARTBEAT=7, ERROR=8, PUBLISH=16, PUBLISHED=17, SUBSCRIBE=32, SUBSCRIBED=33,
UNSUBSCRIBE=34, UNSUBSCRIBED=35, EVENT=36, CALL=48, CANCEL=49, RESULT=50,
REGISTER=64, REGISTERED=65, UNREGISTER=66, UNREGISTERED=67, INVOCATION=68,
INTERRUPT=69, YIELD=70.

Notable field orders (each message class's `unmarshal` is authoritative):

- `[HELLO, Realm, Details]` — `messages/wamp_hello_message.hpp:107-108`
- `[WELCOME, Session, Details]` — `messages/wamp_welcome_message.hpp:107-108`
- `[CALL, Request, Options, Procedure, Args?]` — `wamp_call_message.hpp:136-138`
- `[INVOCATION, Request, Registration, Details, Args?]` — `wamp_invocation_message.hpp:136-140`
- `[YIELD, Request, Options?, Args?]` — `wamp_yield_message.hpp:130-136`
- `[RESULT, Request, Details, Args?]` — `wamp_result_message.hpp:129-132`
- `[SUBSCRIBE, Request, Options, Topic]` — `wamp_subscribe_message.hpp:109-111`
- `[EVENT, Subscription, Publication, Details, Args?]` — `wamp_event_message.hpp:136-140`

RawSocket handshake: 4-byte magic `0x7F` high byte + length-exponent + 4-bit
serializer code (`0x2`=msgpack); server echoes capabilities then switches to
4-byte big-endian length-prefixed messages (`rawsocket_connection.hpp:111-158`,
`rawsocket_server_impl.cpp:179-234`).

## Process model

`daemon/bonefish.cpp` parses options with boost::program_options
(`-r realm`, `-w port`, `-t port`, `-u path`, `--no-json`, `--no-msgpack`,
`-d`) and hands off to `daemon::run()` (`daemon/daemon.cpp`). The daemon owns
a single `boost::asio::io_service` (`daemon.hpp:53`), keeps it alive with an
`io_service::work` (`daemon.cpp:106`), and traps SIGINT/SIGTERM via
`boost::asio::signal_set` (`daemon.cpp:108,177`) dispatching
`shutdown_handler` on the service (`daemon.cpp:124`). Everything runs on one
thread; ROADMAP.md notes multi-core scalability as future work.

## Library layout (`src/bonefish/`)

| Dir | Role |
|---|---|
| `router/` | `wamp_router` (public handle), `wamp_router_impl` (session map, attach/detach, HELLO/WELCOME/GOODBYE handling, welcome details with dealer features `call_timeout`/`progressive_call_results`, `wamp_router_impl.cpp:155-193`), `wamp_routers` registry |
| `broker/` | SUBSCRIBE/PUBLISH/UNSUBSCRIBE state, EVENT fan-out; advertises role `broker` |
| `dealer/` | REGISTER/CALL state, INVOCATION dispatch, YIELD/RESULT, ERROR mapping, call timeout via `boost::asio::deadline_timer` (`wamp_dealer_invocation.hpp:54,86`, `wamp_dealer.cpp:501-515`) |
| `rawsocket/` | server + tcp/uds listeners; `rawsocket_connection.hpp` (hpp-only) does handshake + length-prefixed framing |
| `websocket/` | websocketpp server config/protocol (`websocketpp` under the hood; `wamp_session.hpp:30-31` leaks websocketpp types into nearly every TU) |
| `native/` | in-process endpoints over socketpair, `boost::future/promise` API (`native_endpoint.hpp:21-44`) |
| `session/` | `wamp_session` (state machine NONE/OPEN/CLOSED/FAILED, roles set) |
| `messages/` | one hand-rolled class per WAMP message, msgpack-object backed `marshal`/`unmarshal` |
| `serialization/` | `msgpack_serializer` (msgpack-c 1.x API), `json_serializer` (rapidjson + json-msgpack bridge) |
| `common/` | `wamp_message_processor` — the message switch |
| `identifiers/` | sequential + random (mt19937_64, ≤2^53 per spec) id generators |
| `roles/`, `session/` | role/type parsing (`wamp_hello_details.cpp`) |
| `trace/` | `BONEFISH_TRACE` macro (boost::format + boost::preprocessor variadics, `trace.hpp:20-23`) |
| `utility/` | `wamp_uri` validation (std::regex default; boost::regex only on GCC<4.9, `CMakeLists.txt:27-30`) |

## Build targets

`src/CMakeLists.txt:134` builds static lib `bonefish` (shared via `-Dshared=ON`);
`daemon/CMakeLists.txt` links the `bonefish_bin` target (output name
`bonefish`); `examples/` and `test/functional/websocket/` link against it.
RT library detection comes from `cmake/Modules/FindRT.cmake`;
`cmake/Modules/FindBonefish.cmake` supports downstream consumption
(`install` rules: `src/CMakeLists.txt:141-144`).
