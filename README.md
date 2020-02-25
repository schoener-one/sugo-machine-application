# MoCo CoffeeAutomat Project

This package contains the CoffeeAutomat project of MoCo.

## Installation

Make sure you have installed all required packages on your system.

For Ubuntu:

```bash
sudo apt install gtest-dev gmock-dev libprotobuf-dev cmake libzmq5 \
  clang-format libboost-log-dev libboost-program-options-dev \
  libboost-thread-dev libboost-system-dev 
```

Additional libraries not available as debian package:
* azmq: https://github.com/zeromq/azmq
* jsonrpcpp: https://github.com/badaix/jsonrpcpp.git

### X86 installation

First you have to make sure the additional dependent libraries like azmq and
librpcpp are available. For that create an sdk.x86 directory, step in and
clone both repositories. For every package build them and install them into
the sdk.86 root path:

*azmq:*

> **HINT**: Apply the azmq patch from meta-moco first!

```bash
cd sdk.x86
git clone https://github.com/zeromq/azmq
cd azmq
cmake . -Bbuild.x86 -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_INSTALL_PREFIX:PATH=$PWD/.. -DAZMQ_NO_TESTS=1
cmake --build build.x86 -- install
```

*jsonrpcpp:*

```bash
cd sdk.x86
git clone https://github.com/badaix/jsonrpcpp.git
cd jsonrpcpp
cmake . -Bbuild.x86 -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_INSTALL_PREFIX:PATH=$PWD/..
cmake --build build.x86 -- install
```


The CoffeeAutomat application can be build on host systems as well, to run the
unit tests for example:

```bash
cmake . -Bbuild.x86 -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_CXX_FLAGS="-I$PWD/sdk.x86/include" \
  -DCMAKE_EXE_LINKER_FLAGS="-L$PWD/sdk.x86/lib"
cmake --build build -- all
```

### Cross target installation (i.e. on Raspberry PI)

First you need an installed Yocto SDK. If 'LD_LIBRARY_PATH'
has been set in your environment unset it first. Source the
contained environment configuration script and call
cmake as follows:

```bash
source .../sdk/environment-setup-...
cmake . -Bbuild -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_TOOLCHAIN_FILE=$OECORE_NATIVE_SYSROOT/usr/share/cmake/OEToolchainConfig.cmake
cmake --build build -- all
```

## Using

The CoffeAutomat main application should be started as a daemon job when
the system gets started. It is listening to remote commands comming from a
user interface for example.

The application has to be configured appropriate to the usage and hardware
it runs on!

Example:

```bash
CoffeeAutomatApp [-d] -c /etc/CoffeeAutomat.conf
```

**Important**: Before the I2C interface can be used, the two kernel modules
'i2c-dev' and 'i2c-bcm2708' have to be loaded first.

## Testing

The project comes contains different unit-tests. In order to run them
you just have to build the project as previously described. After that
you have to change to the build directory (e.g. 'build.x86') and just
call 'ctest', which runs all tests.

## Development

**Architecture documentation**: [Architecture.md](doc/Architecture.md)

# Contributing

The software is not allowed to be contributed for commercial intentions.

# License

CLOSED

# TODOs

1. Remove cppcheck suppressed parts (e.g. '// cppcheck-suppress')
