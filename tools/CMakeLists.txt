find_package(ZLIB REQUIRED)

include(CheckCSourceCompiles)
set(LIBM_TEST_SOURCE "#include<math.h>\nfloat f; int main(){sqrt(f);return
0;}")
check_c_source_compiles("${LIBM_TEST_SOURCE}" HAVE_MATH)
if(HAVE_MATH)
  set(LIBM_LIBRARIES)
else()
  set(CMAKE_REQUIRED_LIBRARIES m)
  check_c_source_compiles("${LIBM_TEST_SOURCE}" HAVE_LIBM_MATH)
  unset(CMAKE_REQUIRED_LIBRARIES)
  if(NOT HAVE_LIBM_MATH)
    message(FATAL_ERROR "Unable to use C math library functions")
  endif()
  set(LIBM_LIBRARIES m)
endif()

#add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../libs libs EXCLUDE_FROM_ALL)

add_executable(bmp2tim bmp2tim.c)
add_executable(elf2exe elf2exe.c)
add_executable(getpsxiso getpsxiso.c)
add_executable(mkpsxiso mkpsxiso.c)
add_executable(vag2wav vag2wav.c)
add_executable(wav2vag wav2vag.c)
add_executable(exefixup exefixup.c)
add_executable(systemcnf systemcnf.c)
add_executable(bin2c bin2c.c)
add_executable(huff huff.c)
#add_executable(mod4psx mod4psx.c adpcm.c)
#target_link_libraries(mod4psx PRIVATE ${LIBM_LIBRARIES} psx::modplay)
add_executable(tim2bmp tim2bmp.c)
target_link_libraries(tim2bmp PRIVATE ZLIB::ZLIB)
add_executable(lictool lictool.c)
add_executable(psfex psfex.c)
target_link_libraries(psfex PRIVATE ZLIB::ZLIB)

install(
    TARGETS
        bmp2tim
        elf2exe
        getpsxiso
        mkpsxiso
        vag2wav
        wav2vag
        exefixup
        systemcnf
        bin2c
        huff
        #mod4psx
        tim2bmp
        lictool
        psfex
    DESTINATION bin
)