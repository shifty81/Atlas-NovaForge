#!/bin/bash

# Simple build script for frustum culling test
# This builds just the test without requiring full external dependencies

# --- Logging Setup ---
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
LOG_DIR="$SCRIPT_DIR/logs"
mkdir -p "$LOG_DIR"
SCRIPT_NAME="$(basename "$0" .sh)"
LOG_FILE="$LOG_DIR/${SCRIPT_NAME}_$(date '+%Y%m%d_%H%M%S').log"
exec > >(tee -a "$LOG_FILE") 2>&1
echo "Log file: $LOG_FILE"
echo ""

echo "Building Frustum Culling Test..."

# Create build directory
mkdir -p build_test
cd build_test

# Compile frustum culler implementation
g++ -c -std=c++17 -I../include -I../external/glm \
    ../src/rendering/frustum_culler.cpp \
    -o frustum_culler.o

# Compile LOD manager implementation
g++ -c -std=c++17 -I../include -I../external/glm \
    ../src/rendering/lod_manager.cpp \
    -o lod_manager.o

# Compile and link test
g++ -std=c++17 -I../include -I../external/glm \
    ../test_frustum_culling.cpp \
    frustum_culler.o \
    lod_manager.o \
    -o test_frustum_culling

if [ $? -eq 0 ]; then
    echo "Build successful!"
    echo "Running tests..."
    ./test_frustum_culling
else
    echo "Build failed!"
    exit 1
fi
