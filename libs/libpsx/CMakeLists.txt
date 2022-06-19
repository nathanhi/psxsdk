add_library(psx)
target_sources(psx
    PRIVATE
        src/syscalls.s
        src/psxsdk.c
        src/libc.c
        src/setup.c
        src/memory.c
        src/gpu.c
        src/memcard.c
        src/pad.c
        src/spu.c
        src/util.c
        src/libc/printf.c
        src/libc/string.c
        src/libc/scanf.c
        src/start.s
)
include_directories(psx PUBLIC include)
add_library(psx::psx ALIAS psx)
install(TARGETS psx)