#!/bin/bash -e

# Build for AmigaDOS
./build.sh -DBUILD_FOR_AMIGADOS=1

#Build for NDOS
./build.sh -DBUILD_FOR_AMIGADOS=0

echo "Everything went fine"
