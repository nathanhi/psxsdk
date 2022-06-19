function(_add_lib target)
    cmake_parse_arguments(TGT "" "DIRECTORY" "" ${ARGN})

    add_library(${target})
    add_library(psx::${target} ALIAS ${target})
    install(
        TARGETS ${target}
        ARCHIVE DESTINATION mipsel-unknown-elf/lib
        LIBRARY DESTINATION mipsel-unknown-elf/lib
    )
    install(
        DIRECTORY ${TGT_DIRECTORY}/include/
        DESTINATION mipsel-unknown-elf/include
    )

    file(GLOB_RECURSE sources CONFIGURE_DEPENDS ${TGT_DIRECTORY}/src/*)
    target_sources(${target}
        PRIVATE
            ${sources}
    )
    target_include_directories(${target} PUBLIC ${TGT_DIRECTORY}/include)
endfunction()

# PSX core "libc"/stdlib
_add_lib(psx
    DIRECTORY libpsx
)

_add_lib(m
    DIRECTORY libm
)
target_link_libraries(m
    PRIVATE psx::psx
)

_add_lib(adpcm
    DIRECTORY libadpcm
)
target_link_libraries(adpcm
    PRIVATE
        psx::m
        psx::psx
)

# .mod playback library
_add_lib(modplay
    DIRECTORY libmodplay
)
target_compile_definitions(modplay
    PRIVATE
        $<$<NOT:$<PLATFORM_ID:psx>>:NO_PSX_LIB>
)
target_link_libraries(modplay
    PRIVATE
        $<$<PLATFORM_ID:psx>:psx::psx>
        # TODO: Unify tools/adpcm and libs/libadpcm
        $<$<PLATFORM_ID:psx>:psx::adpcm>
)

# Huffmann decompression algorithm
_add_lib(huff
    DIRECTORY libhuff
)
target_link_libraries(huff
    PRIVATE
        psx::psx
)