#!/bin/bash -e

PROJECT=coroutine68k

rm -rf ../${PROJECT}_build

mkdir ../${PROJECT}_build
cd ../${PROJECT}_build
cmake -G "Eclipse CDT4 - Unix Makefiles" -DCMAKE_ECLIPSE_GENERATE_LINKED_RESOURCES=OFF $* ../$PROJECT
make -j$(nproc)
