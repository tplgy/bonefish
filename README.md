Bonefish
========

Bonefish is an open-source C++ WAMP application router that allows to build
applications from loosely-coupled components. It currently supports the WAMP
Basic Profile and RawSocket Transport as part of the Advanced Profile.

For more information on WAMP, see http://wamp.ws/

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
