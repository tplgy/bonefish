Bonefish
========

[![Build Status](https://travis-ci.org/tplgy/bonefish.png)](https://travis-ci.org/tplgy/bonefish)

Bonefish is an open-source C++ WAMP application router that enables building applications from loosely-coupled components. It currently supports the WAMP Basic Profile and RawSocket Transport as part of the Advanced Profile. Bonefish is capable of hosting multiple routers each with their own realm and associated set of websocket and/or rawsocket endpoints.

For more information on WAMP, see http://wamp.ws/

## Contribute

See [CONTRIBUTING.md](CONTRIBUTING.md)

## Dependencies

You need the following installed:

- A C++11-compatible compiler toolchain (minimum supported versions: clang-3.4, gcc-4.8)
- CMake
- Boost. It uses the ASIO, system and thread components.

## Getting the Code

```
$ git clone --recursive git@github.com:tplgy/bonefish.git
```

## Build

Bonefish uses CMake as its build system.

To build out of the source tree (preferred):

```
$ mkdir bonefish-build && cd bonefish-build
$ cmake ../bonefish
$ make
$ make install
```

If you have the `ninja` build tool available, invoke `cmake` with the `-G Ninja` option and then use `ninja` instead of `make`.

### Deploy

You have the option to use bonefish as both an embedded library or as a standalone WAMP server.

#### Embedded

To understand how to integrate with bonefish as an embedded library an example has been provided:

```
examples/integration.cpp
```

To debug bonefish when it is an embedded library there is a global setting that can be turned on:

```
bonefish::trace::set_enabled(bool enabled)
```

This will enable debug tracing which is currently sent to the console. It is up to the embedder to select an appropriate mechanism for enabling and disabling debugging such as a preprocessor macro or environment variable.

#### Standalone
Apart from using it as an embedded library, you can also run bonefish as a standalone WAMP server:

```
daemon/bonefish --realm "default" --websocket-port 9999 --rawsocket-port 8888
```

To debug bonefish when it is run in standalone mode you can supply the optional command line switch:

```
daemon/bonefish --realm "default" --websocket-port 9999 --rawsocket-port 8888 --debug
```

This will enable debug tracing which is currently sent to the console.

### Options

- **shared** - if ON, bonefish will be built as a shared library. OFF implies it will be built as a static library (default).
- **stdlib** - when building with Clang, you can choose libc++ (default) or libstdc++.

## Third Party Libraries

Bonefish relies on the following header-only library projects:

- [**WebSocket++**](https://github.com/zaphoyd/websocketpp)
- [**msgpack-c**](https://github.com/msgpack/msgpack-c)
- [**RapidJSON**](https://github.com/miloyip/rapidjson)

## License

```
Copyright 2015 Topology LP.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

   http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
```
