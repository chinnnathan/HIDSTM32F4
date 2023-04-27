#!/bin/bash

cd build
cmake -G "Ninja" -DCMAKE_TOOLCHAIN_FILE=../mousewiggle.cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build . -- -j 4