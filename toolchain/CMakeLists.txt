cmake_minimum_required(VERSION 3.12.0)
project("PSX SDK Toolchain")

set(TOOLCHAIN_TYPE "clang" CACHE STRING "Specify compiler type to build, can be either clang or gcc")

include(ExternalProject)
include(ProcessorCount)
ProcessorCount(N)

set(TOOLCHAIN_DIR "${CMAKE_CURRENT_LIST_DIR}")

if(TOOLCHAIN_TYPE STREQUAL "clang")
    ExternalProject_Add(llvm
      URL https://github.com/llvm/llvm-project/releases/download/llvmorg-11.0.0/llvm-project-11.0.0.tar.xz
      URL_HASH SHA256=b7b639fc675fa1c86dd6d0bc32267be9eb34451748d2efd03f674b773000e92b
      PREFIX "${TOOLCHAIN_DIR}"
      SOURCE_DIR "src/llvm"
      SOURCE_SUBDIR "llvm"
      INSTALL_DIR "."
      PATCH_COMMAND patch -p1 < ${TOOLCHAIN_DIR}/patches/llvm_mips1.patch
      BUILD_COMMAND ${CMAKE_COMMAND} --build . -j ${N}
      CMAKE_ARGS
        -DLLVM_TARGETS_TO_BUILD:STRING=Mips
        -DLLVM_DEFAULT_TARGET_TRIPLE:STRING=mipsel-unknown-elf
        -DLLVM_ENABLE_LTO:BOOL=ON
        -DCMAKE_INSTALL_PREFIX:PATH=${TOOLCHAIN_DIR}
        -DCMAKE_BUILD_TYPE:STRING=Release
        -DLLVM_INCLUDE_TESTS:BOOL=OFF
        -DLLVM_INCLUDE_GO_TESTS:BOOL=OFF
        -DLLVM_INCLUDE_EXAMPLES:BOOL=OFF
        -DLLVM_INCLUDE_DOCS:BOOL=OFF
        -DLLVM_ENABLE_DOXYGEN:BOOL=OFF
        -DLLVM_ENABLE_PROJECTS:STRING=clang
        -DLLVM_OPTIMIZED_TABLEGEN:BOOL=ON
    )
elseif(TOOLCHAIN_TYPE STREQUAL "gcc")
    ExternalProject_Add(binutils
                        URL                 http://ftp.gnu.org/gnu/binutils/binutils-2.33.1.tar.xz
                        URL_HASH            SHA256=ab66fc2d1c3ec0359b8e08843c9f33b63e8707efdff5e4cc5c200eae24722cbf
                        SOURCE_DIR          ${TOOLCHAIN_DIR}/src/binutils
                        PREFIX              ${TOOLCHAIN_DIR}
                        INSTALL_DIR         ${TOOLCHAIN_DIR}
                        CONFIGURE_COMMAND   ${TOOLCHAIN_DIR}/src/binutils/configure --prefix=${TOOLCHAIN_DIR} --target=mipsel-unknown-elf --with-float=soft
                        BUILD_COMMAND       make -j${N}
                        INSTALL_COMMAND     make install
    )

    ExternalProject_Add(m4
                        URL                 http://ftp.gnu.org/gnu/m4/m4-1.4.18.tar.xz
                        URL_HASH            SHA256=f2c1e86ca0a404ff281631bdc8377638992744b175afb806e25871a24a934e07
                        SOURCE_DIR          ${TOOLCHAIN_DIR}/src/m4
                        PREFIX              ${TOOLCHAIN_DIR}
                        INSTALL_DIR         ${TOOLCHAIN_DIR}
                        PATCH_COMMAND       patch -p1 < ${TOOLCHAIN_DIR}/patches/m4/libio_removal.patch
                        CONFIGURE_COMMAND   ${TOOLCHAIN_DIR}/src/m4/configure --prefix=${TOOLCHAIN_DIR}
                        BUILD_COMMAND       make -j${N}
                        INSTALL_COMMAND     make install
    )

    ExternalProject_Add(gmp
                        DEPENDS             m4
                        URL                 https://gmplib.org/download/gmp/gmp-6.1.2.tar.xz
                        URL_HASH            SHA256=87b565e89a9a684fe4ebeeddb8399dce2599f9c9049854ca8c0dfbdea0e21912
                        SOURCE_DIR          ${TOOLCHAIN_DIR}/src/gmp
                        PREFIX              ${TOOLCHAIN_DIR}
                        INSTALL_DIR         ${TOOLCHAIN_DIR}
                        CONFIGURE_COMMAND   ${TOOLCHAIN_DIR}/src/gmp/configure --prefix=${TOOLCHAIN_DIR} M4=${TOOLCHAIN_DIR}/bin/m4
                        BUILD_COMMAND       make -j${N}
                        INSTALL_COMMAND     make install
    )

    ExternalProject_Add(mpfr
                        DEPENDS             gmp
                        URL                 https://www.mpfr.org/mpfr-current/mpfr-4.0.2.tar.xz
                        URL_HASH            SHA256=1d3be708604eae0e42d578ba93b390c2a145f17743a744d8f3f8c2ad5855a38a
                        SOURCE_DIR          ${TOOLCHAIN_DIR}/src/mpfr
                        PREFIX              ${TOOLCHAIN_DIR}
                        INSTALL_DIR         ${TOOLCHAIN_DIR}
                        CONFIGURE_COMMAND   ${TOOLCHAIN_DIR}/src/mpfr/configure --prefix=${TOOLCHAIN_DIR} --with-gmp=${TOOLCHAIN_DIR}
                        BUILD_COMMAND       make -j${N}
                        INSTALL_COMMAND     make install
    )

    ExternalProject_Add(mpc
                        DEPENDS             gmp mpfr
                        URL                 https://ftp.gnu.org/gnu/mpc/mpc-1.1.0.tar.gz
                        URL_HASH            SHA256=6985c538143c1208dcb1ac42cedad6ff52e267b47e5f970183a3e75125b43c2e
                        SOURCE_DIR          ${TOOLCHAIN_DIR}/src/mpc
                        PREFIX              ${TOOLCHAIN_DIR}
                        INSTALL_DIR         ${TOOLCHAIN_DIR}
                        CONFIGURE_COMMAND   ${TOOLCHAIN_DIR}/src/mpc/configure --prefix=${TOOLCHAIN_DIR} --with-gmp=${TOOLCHAIN_DIR} --with-mpfr=${TOOLCHAIN_DIR}
                        BUILD_COMMAND       make -j${N}
                        INSTALL_COMMAND     make install
    )

    ExternalProject_Add(gcc
                        DEPENDS             binutils gmp mpfr mpc
                        URL                 http://ftp.gnu.org/gnu/gcc/gcc-9.2.0/gcc-9.2.0.tar.xz
                        URL_HASH            SHA256=ea6ef08f121239da5695f76c9b33637a118dcf63e24164422231917fa61fb206
                        SOURCE_DIR          ${TOOLCHAIN_DIR}/src/gcc
                        PREFIX              ${TOOLCHAIN_DIR}
                        INSTALL_DIR         ${TOOLCHAIN_DIR}
                        CONFIGURE_COMMAND   PATH=${TOOLCHAIN_DIR}/bin:$ENV{PATH} LD_LIBRARY_PATH=${TOOLCHAIN_DIR}/lib ${TOOLCHAIN_DIR}/src/gcc/configure --prefix=${TOOLCHAIN_DIR} --disable-libada --disable-libssp --target=mipsel-unknown-elf --with-float=soft --with-gmp=${TOOLCHAIN_DIR} --with-gmp-lib=${TOOLCHAIN_DIR}/lib/libgmp.so --with-mpfr=${TOOLCHAIN_DIR} --with-mpfr-lib=${TOOLCHAIN_DIR}/lib/libmpfr.so --with-mpc=${TOOLCHAIN_DIR} --with-mpc-lib=${TOOLCHAIN_DIR}/lib/libmpc.so --disable-libquadmath --enable-stage1-languages=c --enable-languages=c --enable-lto
                        BUILD_COMMAND       PATH=${TOOLCHAIN_DIR}/bin:$ENV{PATH} LD_LIBRARY_PATH=${TOOLCHAIN_DIR}/lib make -j${N}
                        INSTALL_COMMAND     PATH=${TOOLCHAIN_DIR}/bin:$ENV{PATH} LD_LIBRARY_PATH=${TOOLCHAIN_DIR}/lib make install
    )
else()
    message(FATAL_ERROR "Unknown toolchain type ${TOOLCHAIN_TYPE}")
endif()