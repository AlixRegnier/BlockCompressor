cmake_minimum_required(VERSION 3.14)
project(BlockCompressor VERSION 0.1.0 LANGUAGES CXX)

# Set C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

include(cmake/avx.cmake)
check_for_avx2()

if(NOT HAS_AVX2)
    message(FATAL_ERROR "AVX2 support is required to compile this project.")
else()
    message(STATUS "AVX2 support detected.")
endif()

add_subdirectory(external)

add_library(build_type_flags INTERFACE)
target_compile_options(build_type_flags INTERFACE
  $<$<CONFIG:Debug>:-O -g>
  $<$<CONFIG:Release>:-O3 -DNDEBUG>
  $<$<CONFIG:Profile>:-O3 -ggdb3 -DNDEBUG -fno-inline>
  $<$<CONFIG:Coverage>:-O0 -g --coverage>
  $<IF:$<BOOL:${PORTABLE_BUILD}>,-march=x86-64,-march=native>
)

include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/include/zstd
    ${PROJECT_BINARY_DIR}/sdsl/include
)

set(SRC_DIR ${PROJECT_SOURCE_DIR}/src)
set(INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)

add_library(block_compressor)
target_sources(block_compressor PRIVATE 
    ${SRC_DIR}/BlockCompressor.cpp
    ${SRC_DIR}/ConfigurationLiterate.cpp
    ${SRC_DIR}/BlockDecompressor.cpp
    ${SRC_DIR}/zstd/BlockCompressorZSTD.cpp
    ${SRC_DIR}/zstd/BlockDecompressorZSTD.cpp
)
target_link_libraries(block_compressor PRIVATE build_type_flags)
target_include_directories(block_compressor PUBLIC
    $<BUILD_INTERFACE:${INCLUDE_DIR}>
    $<INSTALL_INTERFACE:include>
)
target_link_libraries(block_compressor PRIVATE sdsl-lite zstd)
add_dependencies(block_compressor zstd)
