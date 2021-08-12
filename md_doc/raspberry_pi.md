## Compiling on a Raspberry Pi (ARM)
As Gradle native is not compatible with the ARM architecture of the Raspberry Pi, [CMake](https://cmake.org/)  is used for the compilation process.

### Requirements
- CMake (>= 3.6.2)

Note that CUnit, cppcheck, valgrind and Java are **not** required to compile using CMake.

### How to build
In order to build you need to execute the following commands in the project root directory
```
mkdir -p build
cd build
cmake ..
make
```
The executables are located in `build/bin/exe/examples`, the library is located in `build/bin/lib`

### Notes
- As the default MD4 implementation does not work on ARM correctly, OpenSSL / `libcrypto` is used as a replacement.
This means that this library has to be available in the system 
- The CUnit test are **not** executed in the build process and are not supported otherwise either
- RaSTA, SCI-P and SCI-LS are compiled into a single shared library `librasta`
- The examples are same examples as in the Gradle build script (`scip_example`, `scils_example`, `rasta_example_new`), except that they are configured to run on localhost
- you can use `make install`to install the library files on the system (you might need root privileged)