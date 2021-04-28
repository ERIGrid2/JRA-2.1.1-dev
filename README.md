# ERIGrid 2.0: Task JRA 2.1.1 development repository

## About

Task JRA 2.1.1 aims at developing solutions for communication network co-simulation based on the FMI 3.0 specification.

This repository contains:

 * Source code for FMI 3.0-compliant test FMUs (folder `fmi3`)
 * C++ source code for running the test FMUs (folder `test-cpp`)
 
## Installation and Usage (Ubuntu 20.04)

### Prerequisites

 * CMake (version 3.12 or later): `sudo apt install cmake`
 * GCC toolchain: `sudo apt install build-essential`
 * Python

### Building the test FMUs (Linux)

The following lines will create the complete test FMUs in folder `fmi3/build/dist`:
```bash
cd fmi3
mkdir build
cd build
cmake ..
make
```

### Running the tests (C++)

After building the test FMUs, build the tests:
```bash
cd test-cpp
mkdir build
cd build
cmake ..
make
```

**NOTES**: 

 * The build configuration (file `test-cpp/CMakeLists.txt`) assumes that the test FMUs were built in folder `fmi3/build` (see previous step).
 * These tests do not use the test FMUs directly, but rather the compiled shared libraries found in folder `fmi3/build/temp`.

### Running the tests (Python)

TBD
