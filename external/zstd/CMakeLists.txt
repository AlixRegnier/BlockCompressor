include(ExternalProject)
include(GNUInstallDirs)

add_library(zstd_ext INTERFACE)

set(ZSTD_DIR ${PROJECT_BINARY_DIR}/bc_ext_install/zstd)

include(ExternalProject)

set(BC_ZSTD_BUILD_SHARED_LIB OFF CACHE BOOL "Build shared ZSTD library" FORCE)
set(BC_ZSTD_BUILD_STATIC_LIB ON CACHE BOOL "Build static ZSTD library" FORCE)

# Download and build Zstd as an External Project
ExternalProject_Add(ZSTD-external
    GIT_REPOSITORY https://github.com/facebook/zstd.git
    GIT_TAG v1.5.7
    SOURCE_SUBDIR build/cmake
    CMAKE_ARGS 
        -DCMAKE_INSTALL_PREFIX=${ZSTD_DIR}
        -DZSTD_BUILD_STATIC=${BC_ZSTD_BUILD_STATIC_LIB}
        -DZSTD_BUILD_SHARED=${BC_ZSTD_BUILD_SHARED_LIB}
        -DBUILD_SHARED_LIBS=${BC_ZSTD_BUILD_SHARED_LIB}
        -DZSTD_BUILD_PROGRAMS=OFF
        -DZSTD_BUILD_TESTS=OFF
)

set(ZSTD_LIB "${ZSTD_DIR}/${CMAKE_INSTALL_LIBDIR}/${CMAKE_SHARED_LIBRARY_PREFIX}zstd${CMAKE_STATIC_LIBRARY_SUFFIX}")

if (NOT ZSTD_HEADER_ONLY)
  set_target_properties(
    zstd_ext PROPERTIES
    IMPORTED_LOCATION ${ZSTD_LIB}
    INTERFACE_INCLUDE_DIRECTORIES "${ZSTD_DIR}/include"
  )
  target_link_libraries(zstd_ext INTERFACE "${ZSTD_LIB}")
else()
  target_compile_definitions(zstd_ext INTERFACE ZSTD_HEADER_ONLY)
endif()

add_dependencies(zstd_ext ZSTD-external)
