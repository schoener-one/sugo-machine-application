# Sugo Machine Application Project

This package contains the Sugo machine application project.

## Prerequisite

Make sure you have installed all required packages on your system.

For Ubuntu:

```bash
sudo apt install googletest libgtest-dev libgmock-dev \
  protobuf-compiler libprotobuf-dev cmake libzmq5-dev \
  clang-format libboost-log-dev libboost-program-options-dev \
  libboost-thread-dev libboost-system-dev libboost-random-dev
```

Additional libraries which are not available as debian package yet:
* azmq: https://github.com/zeromq/azmq
* jsonrpcpp: https://github.com/badaix/jsonrpcpp.git


### Installation on x86 host

If you don't have the required installed libraries like azmq and librpcpp locally installed, you could fetch and build them also by cmake target:

```bash
cmake --build build --target ExternalLibraries
```

The sugo machine application can be build on host systems as well, to run the unit tests for example:

```bash
cmake . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build --target all
```

### Cross Target Installation (i.e. on Raspberry PI)

First, you need an installed Yocto SDK. If 'LD_LIBRARY_PATH' has been set in your environment, unset it first. Source the contained environment configuration script and call cmake as follows:

```bash
source $SDK_RPI_PATH/environment-setup-...
cmake . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_TOOLCHAIN_FILE=$OECORE_NATIVE_SYSROOT/usr/share/cmake/OEToolchainConfig.cmake
cmake --build build --target all
```

#### Debugging

For remote debugging you can use the GDB from SDK director:

```bash
$SDK_RPI_PATH/sysroots/x86_64-pokysdk-linux/usr/bin/arm-poky-linux/arm-poky-linux-gdb
```

## Using

The machine application could be started easily from commandline by passing the appropriate configuration file. If the application should be started automatically in system, it should be started as a daemon job.

The application is listening to remote commands comming from a user web interface.

Before the application is started, make sure the application configuration is adapted appropriate to the hardware setup of the system it runs on!

Example:

```bash
sugo machineApp [-d] -c /etc/sugo machine.conf
```

> **_NOTE:_** On Raspberry Pi systems make sure the two kernel modules
'i2c-dev' and 'i2c-bcm2708' have to be loaded first before the I2C interface is used!

---

### Testing

All unit tests can be found in the _test_ folder. The project comes with different unit-tests. In order to run all tests, you just have to build the project as previously described. After the tests have been build successfully, execute the following commands to run all tests:

```bash
cd ./build/test
ctest
```

## System Documentation

* [Architecture](doc/Architecture.md)
* [HardwareAbstractionLayer](HardwareAbstractionLayer/doc/README.md)

## Usage

For more information please refer to the [Design Specification](doc/Design.md).

## Contributing

The software is not allowed to be contributed for commercial intentions.

## License

Copyright 2023 by Denis Schoener

Distributed under the GPLv3 License. See [LICENSE](LICENSE) for more information.

> **_NOTE:_**  The library is also distributed under a dual commercial license!

## Contact

Maintainer: Denis Schoener (denis@schoener-one.de)

## TODOs

* Split project libs to submodules (i.e. Common, HardwareAbstractionLayer etc.) 
* Replace address and topic by class ServiceId
* Introduce clang-tidy checks by default and remove warnings
* Create ServiceId to network address translation. Kind of factory which could translate
service identifiers to a full qualified network address.
* Remove cppcheck suppressed parts (e.g. '// cppcheck-suppress')
* Introduce component properties
* Support TCP messaging with mDNS name resolution
* Support CAN messaging
* Add a log format checker for integration tests (see boost::log::core) to check if an error happened
* FreeRTOS support