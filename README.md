# MoCo SugoMachine Project

This package contains the SugoMachine project of MoCo.

## Installation

Make sure you have installed all required packages on your system.

For Ubuntu:

```bash
sudo apt install googletest libgtest-dev libgmock-dev \
  protobuf-compiler libprotobuf-dev cmake libzmq5-dev \
  clang-format libboost-log-dev libboost-program-options-dev \
  libboost-thread-dev libboost-system-dev libboost-random-dev
```

Additional libraries which are not available as debian package:
* azmq: https://github.com/zeromq/azmq
* jsonrpcpp: https://github.com/badaix/jsonrpcpp.git

### X86 Installation

The additional dependent libraries like azmq and librpcpp will be fetched from Git
repository and build automatically within the project build folder.

The SugoMachine application can be build on host systems as well, to run the
unit tests for example:

```bash
cmake . -Bbuild.x86 -DCMAKE_BUILD_TYPE=Debug
cmake --build build --target all
```

### Cross Target Installation (i.e. on Raspberry PI)

First you need an installed Yocto SDK. If 'LD_LIBRARY_PATH'
has been set in your environment unset it first. Source the
contained environment configuration script and call
cmake as follows:

```bash
source $SDK_RPI_PATH/environment-setup-...
cmake . -Bbuild.rpi -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_TOOLCHAIN_FILE=$OECORE_NATIVE_SYSROOT/usr/share/cmake/OEToolchainConfig.cmake
cmake --build build.rpi -- all
```

#### Debugging

For remote debugging you can use the GDB from SDK director:

```bash
$SDK_RPI_PATH/sysroots/x86_64-pokysdk-linux/usr/bin/arm-poky-linux/arm-poky-linux-gdb
```

## Using

The CoffeAutomat main application should be started as a daemon job when
the system gets started. It is listening to remote commands comming from a
user interface for example.

The application has to be configured appropriate to the usage and hardware
it runs on!

Example:

```bash
SugoMachineApp [-d] -c /etc/SugoMachine.conf
```

---
**Important**: Before the I2C interface can be used, the two kernel modules
'i2c-dev' and 'i2c-bcm2708' have to be loaded first.

---

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
