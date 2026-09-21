cmake_minimum_required(VERSION 3.20)
project(kimi_truth CXX)

# kimi_truth — seal verification that cannot be fooled by line endings.
# Consumes LFSSL BLAKE3 (central repo, never mirrored).
#
# Pathing fix 21/09/2026: this repo lives at AvalonOS/inc/Repos/Kimi-Truth;
# LFSSL is its sibling AvalonOS/inc/Repos/LFSSL, already built for both lanes.
# The previous defaults (Tools/*/Kimi_truth home; build_shared/lib,
# build_fortress_win/lib, Forum 2026/build_linux) no longer exist -> fail closed.

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT DEFINED LFSSL_DIR)
    set(LFSSL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../LFSSL")
endif()

if(WIN32)
    set(_lfssl_build "${LFSSL_DIR}/build_win_gcc162")
    set(LFSSL_LIB "${_lfssl_build}/lib/liblfssl.dll.a")
else()
    set(_lfssl_build "${LFSSL_DIR}/build_wsl_gcc162")
    set(LFSSL_LIB "${_lfssl_build}/lib/liblfssl.so.1.0.0")
endif()
if(NOT EXISTS "${LFSSL_LIB}")
    message(FATAL_ERROR "LFSSL shared library not found at ${LFSSL_LIB}; build LFSSL first (fail closed).")
endif()

add_executable(kimi_truth kimi_truth.cpp)
target_include_directories(kimi_truth PRIVATE "${LFSSL_DIR}/include")
target_link_libraries(kimi_truth PRIVATE "${LFSSL_LIB}")
if(NOT WIN32)
    target_link_options(kimi_truth PRIVATE "-Wl,-rpath,${_lfssl_build}/lib")
endif()

if(MSVC)
    target_compile_options(kimi_truth PRIVATE /W4 /WX)
else()
    target_compile_options(kimi_truth PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()
