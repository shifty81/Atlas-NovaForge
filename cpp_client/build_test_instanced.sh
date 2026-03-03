#!/bin/bash

# Build script for instanced rendering test

# --- Logging Setup ---
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
LOG_DIR="$SCRIPT_DIR/logs"
mkdir -p "$LOG_DIR"
SCRIPT_NAME="$(basename "$0" .sh)"
LOG_FILE="$LOG_DIR/${SCRIPT_NAME}_$(date '+%Y%m%d_%H%M%S').log"
exec > >(tee -a "$LOG_FILE") 2>&1
echo "Log file: $LOG_FILE"
echo ""

echo "Building Instanced Rendering Test..."

# Create build directory
mkdir -p build_test_instanced
cd build_test_instanced

# Compile and link test (this test doesn't need OpenGL, just logic testing)
g++ -std=c++17 -I../include -I../external/glm \
    ../test_instanced_rendering.cpp \
    -o test_instanced_rendering

if [ $? -eq 0 ]; then
    echo "Build successful!"
    echo "Running tests..."
    ./test_instanced_rendering
else
    echo "Build failed!"
    exit 1
fi
