cmake_minimum_required(VERSION 3.20)
project(kimi_truth CXX)

# kimi_truth — seal verification that cannot be fooled by line endings.
# Consumes LFSSL BLAKE3 (central repo, never mirrored): point LFSSL_DIR at a
# built LFSSL and LFSSL_LIB at the shared library.

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT DEFINED LFSSL_DIR)
    if(WIN32)
        set(LFSSL_DIR "C:/McMaker Projects/Projects/LFSSL - Lamia Fabrica SSL")
    else()
        set(LFSSL_DIR "/mnt/c/McMaker Projects/Projects/LFSSL - Lamia Fabrica SSL")
    endif()
endif()

find_library(LFSSL_LIB
    NAMES lfssl liblfssl
    PATHS
        "${LFSSL_DIR}/build_shared/lib"
        "${LFSSL_DIR}/build_fortress_win/lib"
        "C:/McMaker Projects/Projects/Forum 2026/build_linux"
        "/mnt/c/McMaker Projects/Projects/Forum 2026/build_linux"
    REQUIRED)

add_executable(kimi_truth kimi_truth.cpp)
target_include_directories(kimi_truth PRIVATE "${LFSSL_DIR}/include")
target_link_libraries(kimi_truth PRIVATE "${LFSSL_LIB}")

if(MSVC)
    target_compile_options(kimi_truth PRIVATE /W4 /WX)
else()
    target_compile_options(kimi_truth PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()
