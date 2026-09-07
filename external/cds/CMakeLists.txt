include(ExternalProject)

set(
    BLOCKCOMPRESSOR_CDS_PREFIX
    "${CMAKE_CURRENT_BINARY_DIR}/install"
)

set(
    BLOCKCOMPRESSOR_CDS_INCLUDE_DIR
    "${BLOCKCOMPRESSOR_CDS_PREFIX}/include"
)

file(MAKE_DIRECTORY "${BLOCKCOMPRESSOR_CDS_INCLUDE_DIR}")

ExternalProject_Add(
    cds_external

    GIT_REPOSITORY
        https://github.com/tlemane/cds.git

    # IMPORTANT:
    # Pin this to a release/tag/commit you have tested.
    #
    # Example:
    # GIT_TAG <commit-or-release>
    #
    GIT_SHALLOW
        TRUE

    # Header-only: no configure or compile phase needed.
    CONFIGURE_COMMAND
        ""

    BUILD_COMMAND
        ""

    INSTALL_COMMAND
        ${CMAKE_COMMAND} -E copy_directory
        <SOURCE_DIR>/include
        ${BLOCKCOMPRESSOR_CDS_INCLUDE_DIR}
)

add_library(external_cds INTERFACE)

target_include_directories(
    external_cds
    INTERFACE
        "${BLOCKCOMPRESSOR_CDS_INCLUDE_DIR}"
)

target_compile_features(
    external_cds
    INTERFACE
        cxx_std_23
)

add_dependencies(
    external_cds
    cds_external
)

add_library(external::cds ALIAS external_cds)