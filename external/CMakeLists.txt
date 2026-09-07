if(BLOCKCOMPRESSOR_ENABLE_ZSTD)
    add_subdirectory(zstd)
endif()

if(BLOCKCOMPRESSOR_ENABLE_CDS)
    add_subdirectory(cds)
endif()