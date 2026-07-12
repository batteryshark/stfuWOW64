cmake_minimum_required(VERSION 3.15)

if(POLICY CMP0091)
    cmake_policy(SET CMP0091 NEW)
endif()

project(stfu_wow64
    VERSION 0.1.0
    DESCRIPTION "Research DLL that terminates WOW64 startup through the wow64log hook"
    LANGUAGES C
)

option(STFU_WOW64_MSVC_NO_C_RUNTIME "Bypass the MSVC C runtime and enter at DllMain" ON)
option(STFU_WOW64_STATIC_RUNTIME "Prefer a static C runtime when a runtime is used" ON)

add_library(wow64log SHARED src/stfuwow64.c)

target_compile_definitions(wow64log PRIVATE
    WIN32_LEAN_AND_MEAN
    NOMINMAX
)

if(MSVC)
    target_compile_options(wow64log PRIVATE /W4)
else()
    target_compile_options(wow64log PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -ffunction-sections
        -fdata-sections
    )

    target_link_options(wow64log PRIVATE
        -Wl,--gc-sections
    )
endif()

if(MSVC AND STFU_WOW64_MSVC_NO_C_RUNTIME)
    target_compile_options(wow64log PRIVATE /GS-)
    target_link_options(wow64log PRIVATE /ENTRY:DllMain /NODEFAULTLIB)
    target_link_libraries(wow64log PRIVATE kernel32)
elseif(MSVC AND STFU_WOW64_STATIC_RUNTIME)
    set_property(TARGET wow64log PROPERTY
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
    )
endif()

if(MINGW AND STFU_WOW64_STATIC_RUNTIME)
    target_link_options(wow64log PRIVATE -static)
endif()

set_target_properties(wow64log PROPERTIES
    PREFIX ""
    OUTPUT_NAME "wow64log"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)
