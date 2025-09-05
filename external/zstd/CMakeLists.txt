include(ExternalProject)

add_library(zstd INTERFACE)

option(FORCE_BUILD_ZSTD "Always build, instead of using the system version." OFF)
option(ZSTD_HEADER_ONLY "Use header only version of zstd." OFF)

#set(ZSTD_MIN_REQUIRED_VERSION "8.1.0...<9.0.0")

if (NOT FORCE_BUILD_ZSTD AND NOT FORCE_BUILD_ALL)

  find_package(zstd ${ZSTD_MIN_REQUIRED_VERSION} CONFIG QUIET)

  if (TARGET zstd::zstd)

    get_target_property(ZSTD_INCLUDE_DIRS zstd::zstd INTERFACE_INCLUDE_DIRECTORIES)

    find_library(ZSTD_LIBRARY zstd)

    file(READ ${ZSTD_INCLUDE_DIRS}/zstd/core.h zstd_core)

    if (NOT zstd_core MATCHES "ZSTD_VERSION ([0-9]+)([0-9][0-9])([0-9][0-9])")
      set(ZSTD_VERSION "Unknown version")
    endif()

    set(ZSTD_VERSION "${CMAKE_MATCH_1}.${CMAKE_MATCH_2}.${CMAKE_MATCH_3}")

    if (ZSTD_HEADER_ONLY)
      target_link_libraries(zstd INTERFACE zstd::zstd-header-only)
      message(STATUS "Found ZSTD: header-only (found suitable version \"${ZSTD_VERSION}\", minimum required is \"${ZSTD_MIN_REQUIRED_VERSION}\")")
    else()
      target_link_libraries(zstd INTERFACE zstd::zstd)
      message(STATUS "Found ZSTD: ${ZSTD_LIBRARY} (found suitable version \"${ZSTD_VERSION}\", minimum required is \"${ZSTD_MIN_REQUIRED_VERSION}\")")
    endif()

    return()

  else()
      message(STATUS "ZSTD not found: use bundled version.")
  endif()

endif()

set(ZSTD_DIR ${EXT_INSTALL_DIR}/zstd)

include(ExternalProject)

# Download and build Zstd as an External Project
ExternalProject_Add(ZSTD-external
    GIT_REPOSITORY https://github.com/facebook/zstd.git
    GIT_TAG v1.5.7
    SOURCE_SUBDIR build/cmake
    CMAKE_ARGS 
        -DCMAKE_INSTALL_PREFIX=${ZSTD_DIR}
        -DZSTD_BUILD_STATIC=OFF
        -DZSTD_BUILD_SHARED=ON
        -DBUILD_SHARED_LIBS=ON
        -DZSTD_BUILD_PROGRAMS=OFF
        -DZSTD_BUILD_TESTS=OFF
)

if (NOT ZSTD_HEADER_ONLY)
  set_target_properties(
    zstd PROPERTIES
    IMPORTED_LOCATION "${ZSTD_DIR}/${CMAKE_INSTALL_LIBDIR}/${CMAKE_SHARED_LIBRARY_PREFIX}zstd${CMAKE_SHARED_LIBRARY_SUFFIX}"
    INTERFACE_INCLUDE_DIRECTORIES "${ZSTD_DIR}/include"
  )
else()
  target_compile_definitions(zstd INTERFACE ZSTD_HEADER_ONLY)
endif()

add_dependencies(zstd ZSTD-external)