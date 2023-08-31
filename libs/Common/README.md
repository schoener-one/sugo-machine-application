# Sugo Common Library

This library is part of the **Sugo Machine Project**.

## Description

The Sugo project provides a bunch of modules (as libraries) for driving multi-component
machines in an easy way. It follows the overall principles **KISS (Keep It Simple, Stupid)** and
**DRY (Don't Repeat Yourself)** in all modules to keep them easy understandable, maintainable
und usable. For that, the modules avoid implementing functionality themselves if solutions 
already exist in open-source libraries and they provide a simple and useful interface for
complex functions.

This library contains commonly used types and functionality of the Sugo project.

## Getting started

The library is developed in a way to support multiple architectures, like X86 and ARM etc..

### Prerequisites

The following external development libraries are required:

* Boost library (>=1.61)
* [JSON-RPC 2.0 library (v1.3.2)](https://github.com/badaix/jsonrpcpp/releases/tag/v1.3.2)
* Cmake

For testing the following libraries are required:

* GoogleTest

If you are using **Ubuntu** as a development host system, you could install required files with
this command:

```bash
sudo apt install googletest libgtest-dev libgmock-dev \
  clang-format libboost-log-dev libboost-program-options-dev \
  libboost-thread-dev libboost-system-dev libboost-random-dev
```

### Installation

The library can be build simply with cmake commands:

```bash
cmake -B ./build -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build ./build --target all
```

### Cross target installation with Yocto SDK

First, you need an installed Yocto SDK to a dedicated path. Important, if **LD_LIBRARY_PATH**
has been set in your environment, unset it first. Source the contained environment
configuration script and call cmake as follows:

```bash
source $SDK_RPI_PATH/environment-setup-...
cmake . -B ./build -G Ninja -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_TOOLCHAIN_FILE=$OECORE_NATIVE_SYSROOT/usr/share/cmake/OEToolchainConfig.cmake
cmake --build ./build --target all
```

### Testing

All unit tests can be found in the _test_ folder. The project comes with different unit-tests.
In order to run all tests, you just have to build the project as previously described. After the
tests have been build successfully execute the following commands to run all tests:

```bash
cd ./build/test
ctest
```

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
