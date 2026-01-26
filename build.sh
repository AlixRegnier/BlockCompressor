#!/bin/bash

mkdir -p ./build
cd build

cmake .. -DCMAKE_BUILD_TYPE=Release -DBC_BUILD_MAIN=1
make -j



cd -