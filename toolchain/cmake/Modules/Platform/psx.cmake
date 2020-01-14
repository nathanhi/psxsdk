include_guard(GLOBAL)

set(PSX 1)

# Custom system `psx` for CMake
set(CMAKE_SYSTEM_PROCESSOR "mips")
set(CMAKE_CROSS_COMPILING TRUE)

# Toolchain prefix
set(CMAKE_SYSROOT "/opt/psx/toolchain")

# Tool names
set(CMAKE_C_COMPILER_INIT "${CMAKE_SYSROOT}/bin/mipsel-unknown-elf-gcc")
set(CMAKE_CXX_COMPILER_INIT "${CMAKE_SYSROOT}/bin/mipsel-unknown-elf-cpp")
set(CMAKE_ADDR2LINE "mipsel-unknown-elf-addr2line")
set(CMAKE_AR "mipsel-unknown-elf-ar")
set(CMAKE_LINKER "mipsel-unknown-elf-ld")
set(CMAKE_NM "mipsel-unknown-elf-nm")
set(CMAKE_OBJCOPY "mipsel-unknown-elf-objcopy")
set(CMAKE_OBJDUMP "mipsel-unknown-elf-objdump")
set(CMAKE_READELF "mipsel-unknown-elf-readelf")
set(CMAKE_STRIP "mipsel-unknown-elf-strip")
set(CMAKE_SIZE_UTIL "mipsel-unknown-elf-size")

# Compiler Flags
SET(PSX_COMMON_C_FLAGS "-fno-builtin -mno-gpopt -nostdlib -msoft-float -march=mips1")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${PSX_COMMON_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${PSX_COMMON_C_FLAGS}")

# Specify search path for compiler
set(CMAKE_C_COMPILER_TARGET "mipsel-unknown-elf")
set(CMAKE_CXX_COMPILER_TARGET "mipsel-unknown-elf")
set(CMAKE_SYSTEM_PROGRAM_PATH "${CMAKE_SYSROOT}/bin")

# Currently no shared library support
set_property(GLOBAL PROPERTY TARGET_SUPPORTS_SHARED_LIBS FALSE)
set(BUILD_SHARED_LIBS FALSE)

# Common include path for psx projects
set(CMAKE_SYSTEM_INCLUDE_PATH "${CMAKE_CURRENT_LIST_DIR}/libpsx/include")

# Common library path
foreach(e CMAKE_SYSTEM_LIBRARY_PATH CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES)
    set(e "${CMAKE_CURRENT_LIST_DIR}/libpsx/lib")
endforeach()

# Predefine region
if(NOT PSX_REGION)
    set(PSX_REGION "EUR")
endif()