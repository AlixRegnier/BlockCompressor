cmake_minimum_required(VERSION 3.14)
project(BlockCompressor)

# Set C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(ExternalProject)
include(FetchContent)

# Add the include directories
include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/include/zstd
    ${PROJECT_BINARY_DIR}/sdsl/include
)

set(SRC_DIR ${PROJECT_SOURCE_DIR}/src)
set(INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)

file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/zstd/include)

# Download and build Zstd as an External Project
ExternalProject_Add(zstd_external
    GIT_REPOSITORY https://github.com/facebook/zstd.git
    GIT_TAG v1.5.7
    SOURCE_SUBDIR build/cmake
    CMAKE_ARGS 
        -DCMAKE_INSTALL_PREFIX=${PROJECT_BINARY_DIR}/zstd
        -DZSTD_BUILD_STATIC=OFF
        -DZSTD_BUILD_SHARED=ON
        -DBUILD_SHARED_LIBS=ON
        -DZSTD_BUILD_PROGRAMS=OFF
        -DZSTD_BUILD_TESTS=OFF
)


FetchContent_Declare(sdsl_external
  GIT_REPOSITORY "https://github.com/xxsds/sdsl-lite.git"
  GIT_TAG "v3.0.3"
  SOURCE_DIR "${PROJECT_BINARY_DIR}/sdsl"
)
FetchContent_MakeAvailable(sdsl_external)



add_library(zstd_lib STATIC IMPORTED)
set_target_properties(zstd_lib PROPERTIES
    IMPORTED_LOCATION "${PROJECT_BINARY_DIR}/zstd/lib/libzstd.so"
    INTERFACE_INCLUDE_DIRECTORIES ${PROJECT_BINARY_DIR}/zstd/include
)
add_dependencies(zstd_lib zstd_external)


# Build the ConfigurationLiterate library
add_library(ConfigurationLiterate SHARED
    ${SRC_DIR}/ConfigurationLiterate.cpp
)

# Build the BlockCompressor library
add_library(BlockCompressor SHARED
    ${SRC_DIR}/BlockCompressor.cpp
)
target_link_libraries(BlockCompressor PRIVATE ConfigurationLiterate)
add_dependencies(BlockCompressor ConfigurationLiterate)


# Build the BlockDecompressor library 
add_library(BlockDecompressor STATIC
    ${SRC_DIR}/BlockDecompressor.cpp
)
target_link_libraries(BlockDecompressor PRIVATE ConfigurationLiterate)
add_dependencies(BlockDecompressor ConfigurationLiterate)


####### ZSTD #######

# Build the BlockCompressorZSTD library
add_library(BlockCompressorZSTD SHARED
    ${SRC_DIR}/zstd/BlockCompressorZSTD.cpp
)
target_link_libraries(BlockCompressorZSTD PUBLIC BlockCompressor zstd_lib)
add_dependencies(BlockCompressorZSTD zstd_lib)

# Build the BlockDecompressorZSTD library
add_library(BlockDecompressorZSTD STATIC
    ${SRC_DIR}/zstd/BlockDecompressorZSTD.cpp
)
target_link_libraries(BlockDecompressorZSTD PUBLIC BlockDecompressor zstd_lib)
add_dependencies(BlockDecompressorZSTD BlockDecompressor zstd_lib)

# Build the mainBlockCompressorZSTD executable
add_executable(mainBlockCompressorZSTD
    ${SRC_DIR}/zstd/mainBlockCompressorZSTD.cpp
)
target_link_libraries(mainBlockCompressorZSTD PUBLIC BlockCompressorZSTD)

# Build the mainBlockDecompressorZSTD executable
add_executable(mainBlockDecompressorZSTD
    ${SRC_DIR}/zstd/mainBlockDecompressorZSTD.cpp
)
target_link_libraries(mainBlockDecompressorZSTD PUBLIC BlockDecompressorZSTD)