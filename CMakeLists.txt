cmake_minimum_required(VERSION 3.24)

project(
    blockcompressor
    VERSION 1.0.0
    LANGUAGES CXX
)

# ---------------------------------------------------------------------------
# Options
# ---------------------------------------------------------------------------

option(
    BLOCKCOMPRESSOR_ENABLE_ZSTD
    "Enable headers that depend on Zstd"
    OFF
)

option(
    BLOCKCOMPRESSOR_ENABLE_CDS
    "Enable headers that depend on cds"
    OFF
)

# ---------------------------------------------------------------------------
# Core header-only library
# ---------------------------------------------------------------------------

add_library(blockcompressor INTERFACE)
add_library(blockcompressor::core ALIAS blockcompressor)

target_include_directories(
    blockcompressor
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# Pick whatever your base library actually requires.
target_compile_features(
    blockcompressor
    INTERFACE
        cxx_std_20
)

target_sources(
    blockcompressor
    INTERFACE
    FILE_SET HEADERS
    BASE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    FILES
        include/block_compressor.hpp
        include/block_decompressor.hpp
        include/compressor.hpp
        include/config.hpp
        include/config_io.hpp
        include/decompressor.hpp
        include/error.hpp
        include/int_container.hpp
        include/stream.hpp
        include/utils.hpp
)

# ---------------------------------------------------------------------------
# External dependencies
# ---------------------------------------------------------------------------

add_subdirectory(external)

# ---------------------------------------------------------------------------
# Optional Zstd-facing interface
# ---------------------------------------------------------------------------

if(BLOCKCOMPRESSOR_ENABLE_ZSTD)
    add_library(blockcompressor_zstd INTERFACE)
    add_library(blockcompressor::zstd ALIAS blockcompressor_zstd)

    target_link_libraries(
        blockcompressor_zstd
        INTERFACE
            blockcompressor
            external::zstd
    )

    target_sources(
        blockcompressor_zstd
        INTERFACE
        FILE_SET HEADERS
        BASE_DIRS
            ${CMAKE_CURRENT_SOURCE_DIR}/include
        FILES
            include/compressor_zstd.hpp
            include/decompressor_zstd.hpp
            include/config_zstd.hpp
    )
endif()

# ---------------------------------------------------------------------------
# Optional cds-facing interface
# ---------------------------------------------------------------------------

if(BLOCKCOMPRESSOR_ENABLE_CDS)
    add_library(blockcompressor_cds INTERFACE)
    add_library(blockcompressor::cds ALIAS blockcompressor_cds)

    target_link_libraries(
        blockcompressor_cds
        INTERFACE
            blockcompressor
            external::cds
    )

    # tlemane/cds currently requires C++23.
    target_compile_features(
        blockcompressor_cds
        INTERFACE
            cxx_std_23
    )

    target_sources(
        myproject_cds
        INTERFACE
        FILE_SET HEADERS
        BASE_DIRS
            ${CMAKE_CURRENT_SOURCE_DIR}/include
        FILES
            include/int_container_ef.hpp
    )
endif()