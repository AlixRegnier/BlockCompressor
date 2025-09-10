#!/bin/bash

mkdir -p ./build
cd build

cmake .. -DCMAKE_BUILD_TYPE=Release -DBC_BUILD_MAIN=1
make



cd -

#g++ ./src/zstd/mainBlockCompressorZSTD.cpp -o sta_compressor -I./include -I./include/zstd -I./build/_deps/sdsl_external-src/include/ -I./build/bc_ext_install/zstd/include/ -L ./build -lblock_compressor -L ./build/bc_ext_install/zstd/lib/ -lzstd
#g++ ./src/zstd/mainBlockDecompressorZSTD.cpp -o sta_decompressor -I./include -I./include/zstd -I./build/_deps/sdsl_external-src/include/ -I./build/bc_ext_install/zstd/include/ -L ./build -lblock_compressor -L ./build/bc_ext_install/zstd/lib/ -lzstd