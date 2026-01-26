include(ExternalProject)
include(GNUInstallDirs)

add_library(lz4_ext INTERFACE)
option(FORCE_BUILD_LZ4 "Always build, instead of using the system version." OFF)
option(FORCE_STATIC_LZ4 "Force building static LZ4 library." OFF)

set(LZ4_MIN_REQUIRED_VERSION "1.10.0")

if (NOT FORCE_BUILD_LZ4 AND NOT FORCE_BUILD_ALL)

  find_package(lz4 ${LZ4_MIN_REQUIRED_VERSION} CONFIG)

  if(TARGET lz4::liblz4_static)
    target_link_libraries(lz4_ext INTERFACE lz4::liblz4_static)
    message(STATUS "Found LZ4 (found suitable version \"${lz4_VERSION}\", minimum required is \"${LZ4_MIN_REQUIRED_VERSION}\")")
    return()
  elseif (TARGET lz4::liblz4_shared AND NOT FORCE_STATIC_LZ4)
    target_link_libraries(lz4_ext INTERFACE lz4::liblz4_shared)
    message(STATUS "Found LZ4 (found suitable version \"${lz4_VERSION}\", minimum required is \"${LZ4_MIN_REQUIRED_VERSION}\")")
    return()
  endif()


  message(STATUS "LZ4 not found: use bundled version.")

endif()

set(LZ4_DIR ${PROJECT_BINARY_DIR}/bc_ext_install/lz4)

include(ExternalProject)

if (FORCE_STATIC_LZ4)
  set(BC_LZ4_BUILD_SHARED_LIB OFF CACHE BOOL "Build shared LZ4 library" FORCE)
else()
  set(BC_LZ4_BUILD_SHARED_LIB ON CACHE BOOL "Build shared LZ4 library" FORCE)
endif()

# Download and build Lz4 as an External Project
ExternalProject_Add(LZ4-external
    GIT_REPOSITORY https://github.com/lz4/lz4.git
    GIT_TAG v1.10.0
    SOURCE_SUBDIR build/cmake
    CMAKE_ARGS 
        -DCMAKE_INSTALL_PREFIX=${LZ4_DIR}
        -DBUILD_STATIC_LIBS=${FORCE_STATIC_LZ4}
        -DBUILD_SHARED_LIBS=${BC_LZ4_BUILD_SHARED_LIB}
        -DLZ4_BUILD_CLI=OFF
)

set(LZ4_LIB "${LZ4_DIR}/${CMAKE_INSTALL_LIBDIR}/${CMAKE_SHARED_LIBRARY_PREFIX}lz4${CMAKE_SHARED_LIBRARY_SUFFIX}")

if (NOT LZ4_HEADER_ONLY)
  set_target_properties(
    lz4_ext PROPERTIES
    IMPORTED_LOCATION ${LZ4_LIB}
    INTERFACE_INCLUDE_DIRECTORIES "${LZ4_DIR}/include"
  )
  target_link_libraries(lz4_ext INTERFACE "${LZ4_LIB}")
else()
  target_compile_definitions(lz4_ext INTERFACE LZ4_HEADER_ONLY)
endif()

add_dependencies(lz4_ext LZ4-external)
