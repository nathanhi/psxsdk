cmake_minimum_required(VERSION 3.12.0)
project("PSX SDK")
#/tmp/inst/toolchain/bin/mipsel-unknown-elf-gcc -fno-builtin -mno-gpopt -nostdlib -msoft-float -march=mips1 -I /opt/psx/libpsx/include/ -L /opt/psx/bld/ -lpsx -static /opt/psx/examples/psxsnake/psxsnake.c -o /tmp/psxsnake -DEXAMPLES_VMODE=VMODE_PAL
include(ExternalProject)
include(ProcessorCount)
ProcessorCount(N)

set(TOOLCHAIN_DIR "${CMAKE_CURRENT_BINARY_DIR}/toolchain")
message(STATUS "TOOLCHAIN_DIR: ${TOOLCHAIN_DIR}")
ExternalProject_Add(binutils
                    URL                 http://ftp.gnu.org/gnu/binutils/binutils-2.38.tar.xz
                    URL_HASH            SHA256=e316477a914f567eccc34d5d29785b8b0f5a10208d36bbacedcc39048ecfe024
                    INSTALL_DIR         ${TOOLCHAIN_DIR}
                    CONFIGURE_COMMAND   <SOURCE_DIR>/configure --prefix=<INSTALL_DIR> --target=mipsel-unknown-elf --with-float=soft --enable-lto --enable-static --disable-shared
                    BUILD_COMMAND       make -j${N}
                    INSTALL_COMMAND     make install
)

ExternalProject_Add(m4
                    URL                 http://ftp.gnu.org/gnu/m4/m4-1.4.19.tar.xz
                    URL_HASH            SHA256=63aede5c6d33b6d9b13511cd0be2cac046f2e70fd0a07aa9573a04a82783af96
                    INSTALL_DIR         ${TOOLCHAIN_DIR}
                    CONFIGURE_COMMAND   <SOURCE_DIR>/configure --prefix=<INSTALL_DIR> --enable-static --disable-shared
                    BUILD_COMMAND       make -j${N}
                    INSTALL_COMMAND     make install
)

ExternalProject_Add(gmp
                    DEPENDS             m4
                    URL                 https://gmplib.org/download/gmp/gmp-6.2.1.tar.xz
                    URL_HASH            SHA256=fd4829912cddd12f84181c3451cc752be224643e87fac497b69edddadc49b4f2
                    INSTALL_DIR         ${TOOLCHAIN_DIR}
                    CONFIGURE_COMMAND   <SOURCE_DIR>/configure --prefix=<INSTALL_DIR> M4=<INSTALL_DIR>/bin/m4 --enable-static --disable-shared
                    BUILD_COMMAND       make -j${N}
                    INSTALL_COMMAND     make install
)

ExternalProject_Add(mpfr
                    DEPENDS             gmp
                    URL                 https://www.mpfr.org/mpfr-4.1.0/mpfr-4.1.0.tar.xz
                    URL_HASH            SHA256=0c98a3f1732ff6ca4ea690552079da9c597872d30e96ec28414ee23c95558a7f
                    INSTALL_DIR         ${TOOLCHAIN_DIR}
                    CONFIGURE_COMMAND   <SOURCE_DIR>/configure --prefix=<INSTALL_DIR> --with-gmp=<INSTALL_DIR> --enable-lto --enable-static --disable-shared
                    BUILD_COMMAND       make -j${N}
                    INSTALL_COMMAND     make install
)

ExternalProject_Add(mpc
                    DEPENDS             gmp mpfr
                    URL                 https://ftp.gnu.org/gnu/mpc/mpc-1.2.1.tar.gz
                    URL_HASH            SHA256=17503d2c395dfcf106b622dc142683c1199431d095367c6aacba6eec30340459
                    INSTALL_DIR         ${TOOLCHAIN_DIR}
                    CONFIGURE_COMMAND   <SOURCE_DIR>/configure --prefix=<INSTALL_DIR> --with-gmp=<INSTALL_DIR> --with-mpfr=<INSTALL_DIR> --enable-static --disable-shared
                    BUILD_COMMAND       make -j${N}
                    INSTALL_COMMAND     make install
)

ExternalProject_Add(gcc
                    DEPENDS             binutils gmp mpfr mpc
                    URL                 http://ftp.gnu.org/gnu/gcc/gcc-12.1.0/gcc-12.1.0.tar.xz
                    URL_HASH            SHA256=62fd634889f31c02b64af2c468f064b47ad1ca78411c45abe6ac4b5f8dd19c7b
                    INSTALL_DIR         ${TOOLCHAIN_DIR}
                    CONFIGURE_COMMAND   PATH=<INSTALL_DIR>/bin:$ENV{PATH} LD_LIBRARY_PATH=<INSTALL_DIR>/lib <SOURCE_DIR>/configure --prefix=<INSTALL_DIR> --disable-libada --disable-libssp --target=mipsel-unknown-elf --with-float=soft --with-gmp=<INSTALL_DIR> --with-gmp-lib=<INSTALL_DIR>/lib/libgmp.a --with-mpfr=<INSTALL_DIR> --with-mpfr-lib=<INSTALL_DIR>/lib/libmpfr.a --with-mpc=<INSTALL_DIR> --with-mpc-lib=<INSTALL_DIR>/lib/libmpc.a --disable-libquadmath --enable-stage1-languages=c --enable-languages=c --enable-lto --enable-static --disable-shared
                    BUILD_COMMAND       PATH=<INSTALL_DIR>/bin:$ENV{PATH} LD_LIBRARY_PATH=<INSTALL_DIR>/lib make -j${N}
                    INSTALL_COMMAND     PATH=<INSTALL_DIR>/bin:$ENV{PATH} LD_LIBRARY_PATH=<INSTALL_DIR>/lib make install
)

install(
    DIRECTORY ${TOOLCHAIN_DIR}/
    DESTINATION .
    USE_SOURCE_PERMISSIONS
)

install(
    DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/licenses
    DESTINATION share
)
set(CPACK_PACKAGE_NAME "psxsdk")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_LIST_DIR}/../license.txt")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_LIST_DIR}/../README.md")
set(CPACK_PACKAGE_CONTACT "Nathan-J. Hirschauer <nathanhi@deepserve.info>")
set(CPACK_BINARY_DEB "ON")
set(CPACK_BINARY_FREEBSD "OFF")
set(CPACK_BINARY_IFW "OFF")
set(CPACK_BINARY_NSIS "OFF")
set(CPACK_BINARY_RPM "ON")
set(CPACK_BINARY_STGZ "OFF")
set(CPACK_BINARY_TBZ2 "OFF")
set(CPACK_BINARY_TGZ "ON")
set(CPACK_BINARY_TXZ "OFF")
set(CPACK_BINARY_TZ "OFF")
set(CPACK_IGNORE_FILES "/CVS/;/\\.svn/;/\\.bzr/;/\\.hg/;/\\.git/;\\.swp\$;\\.#;/#")
set(CPACK_PACKAGE_DEFAULT_LOCATION "/")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "PSX SDK built using CMake")
set(CPACK_PACKAGE_NAME "psxsdk")
set(CPACK_PACKAGE_RELOCATABLE "true")
set(CPACK_PACKAGE_VENDOR "psxsdk project")
set(CPACK_RPM_PACKAGE_SOURCES "ON")
set(CPACK_SET_DESTDIR "OFF")
set(CPACK_SOURCE_GENERATOR "RPM;TGZ")
set(CPACK_SOURCE_IGNORE_FILES "/CVS/;/\\.svn/;/\\.bzr/;/\\.hg/;/\\.git/;\\.swp\$;\\.#;/#")
set(CPACK_SOURCE_RPM "ON")
set(CPACK_SOURCE_TBZ2 "OFF")
set(CPACK_SOURCE_TGZ "OFF")
set(CPACK_SOURCE_TXZ "OFF")
set(CPACK_SOURCE_TZ "OFF")
set(CPACK_SOURCE_ZIP "OFF")
include(CPack)
