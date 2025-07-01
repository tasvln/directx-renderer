#!/bin/bash

# Create build directory if it doesn't exist
mkdir -p build
cd build || exit

# Run cmake configuration (if needed)
cmake -DCMAKE_TOOLCHAIN_FILE=C:/Users/tasvl/vcpkg/scripts/buildsystems/vcpkg.cmake ..

# Build in Debug mode (change to Release if needed)
cmake --build . --config Debug

# Optionally run the executable after build
# ./RAY
