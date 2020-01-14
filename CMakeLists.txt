cmake_minimum_required(VERSION 3.4.3)
set(CMAKE_SYSTEM_NAME psx)
project(PSXSDK C)

add_library(psx)
target_sources(psx PRIVATE libpsx/src/syscalls.s libpsx/src/psxsdk.c libpsx/src/libc.c libpsx/src/setup.c libpsx/src/memory.c libpsx/src/gpu.c libpsx/src/memcard.c libpsx/src/pad.c libpsx/src/spu.c libpsx/src/util.c libpsx/src/libc/printf.c libpsx/src/libc/string.c libpsx/src/libc/scanf.c libpsx/src/start.s)
include_directories(psx PUBLIC libpsx/include)
