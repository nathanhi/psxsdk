# PSXSDK

PSXSDK is an unofficial homebrew software development kit for the Sony PlayStation™.

The SDK supports both GCC as well as 
Its programming tools (compiler, assembler, linker, etc.) are based on GNU binutils
and GNU Compiler Collection (GCC) while the PlayStation-specific tools, which you
need to convert data to PSX format, were either written from scratch or adapted from
already existing open source programs.

## Building
### Toolchain
To use the PSX SDK, the toolchain (GCC & binutils) has to be built first. This has to
be done manually:

```
cd toolchain; cmake .; cmake --build .
```

This will build the clang 9 based toolchain for MIPS ELF binaries.

### libpsx
To build libpsx and/or other psxsdk components, you always need to specify the correct
toolchain file when calling CMake:

```
cmake -DCMAKE_TOOLCHAIN_FILE=clang-mips-toolchain.cmake .
```
Some variables can be overwritten, in case different prefixes/etc. are required. The following
variables exist:

- PSXSDK_PREFIX
- GCC_VER
- BINUTILS_VER
