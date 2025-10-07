include(ExternalProject)
include(GNUInstallDirs)

add_library(zstd_ext INTERFACE)
option(FORCE_BUILD_ZSTD "Always build, instead of using the system version." OFF)
option(FORCE_STATIC_ZSTD "Force building static ZSTD library." OFF)

set(ZSTD_MIN_REQUIRED_VERSION "1.5.0")

if (NOT FORCE_BUILD_ZSTD AND NOT FORCE_BUILD_ALL)

  find_package(zstd ${ZSTD_MIN_REQUIRED_VERSION} CONFIG)

  if(TARGET zstd::libzstd_static)
    target_link_libraries(zstd_ext INTERFACE zstd::libzstd_static)
    message(STATUS "Found ZSTD (found suitable version \"${zstd_VERSION}\", minimum required is \"${ZSTD_MIN_REQUIRED_VERSION}\")")
    return()
  elseif (TARGET zstd::libzstd_shared AND NOT FORCE_STATIC_ZSTD)
    target_link_libraries(zstd_ext INTERFACE zstd::libzstd_shared)
    message(STATUS "Found ZSTD (found suitable version \"${zstd_VERSION}\", minimum required is \"${ZSTD_MIN_REQUIRED_VERSION}\")")
    return()
  endif()


  message(STATUS "ZSTD not found: use bundled version.")

endif()

set(ZSTD_DIR ${PROJECT_BINARY_DIR}/bc_ext_install/zstd)

include(ExternalProject)

if (FORCE_STATIC_ZSTD)
  set(BC_ZSTD_BUILD_SHARED_LIB OFF CACHE BOOL "Build shared ZSTD library" FORCE)
else()
  set(BC_ZSTD_BUILD_SHARED_LIB ON CACHE BOOL "Build shared ZSTD library" FORCE)
endif()

# Download and build Zstd as an External Project
ExternalProject_Add(ZSTD-external
    GIT_REPOSITORY https://github.com/facebook/zstd.git
    GIT_TAG v1.5.7
    SOURCE_SUBDIR build/cmake
    CMAKE_ARGS 
        -DCMAKE_INSTALL_PREFIX=${ZSTD_DIR}
        -DZSTD_BUILD_STATIC=${FORCE_STATIC_ZSTD}
        -DZSTD_BUILD_SHARED=${BC_ZSTD_BUILD_SHARED_LIB}
        -DBUILD_SHARED_LIBS=${BC_ZSTD_BUILD_SHARED_LIB}
        -DZSTD_BUILD_PROGRAMS=OFF
        -DZSTD_BUILD_TESTS=OFF
)

set(ZSTD_LIB "${ZSTD_DIR}/${CMAKE_INSTALL_LIBDIR}/${CMAKE_SHARED_LIBRARY_PREFIX}zstd${CMAKE_SHARED_LIBRARY_SUFFIX}")

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
