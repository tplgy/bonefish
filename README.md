Bonefish
========

Bonefish is an open-source C++ WAMP application router that enables building
applications from loosely-coupled components. It currently supports the WAMP
Basic Profile and RawSocket Transport as part of the Advanced Profile.

For more information on WAMP, see http://wamp.ws/

## Dependencies

You need the following installed:

- A recent C++11-compatible compiler toolchain (clang-3.5 or gcc-4.9 for example)
- CMake
- Boost. It uses the ASIO, system and thread components.

## Getting the Code

```
$ git clone --recursive git@github.com:tplgy/bonefish.git
```

## Build

Bonefish uses CMake as it's build system.

To build out of the source tree (preferred):

```
$ mkdir bonefish-build && cd bonefish-build
$ cmake ../bonefish
$ make
$ make install
```

### Options

- **shared** - if ON, bonefish will be built as a shared library. OFF implies it will be built as a static library.
- **asio_source** - if ON, the Boost.Asio implementation (src.hpp) will be included in the library. Useful for programs which are using Boost.Asio elsewhere.

## Third Party Libraries

Bonefish relies on the following header-only library projects:
- WebSocket++ - https://github.com/zaphoyd/websocketpp
- msgpack for C/C++ - https://github.com/msgpack/msgpack-c
- RapidJSON - https://github.com/miloyip/rapidjson

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
