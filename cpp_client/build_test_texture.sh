#!/bin/bash

# Build script for texture loading test

# --- Logging Setup ---
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
LOG_DIR="$SCRIPT_DIR/logs"
mkdir -p "$LOG_DIR"
SCRIPT_NAME="$(basename "$0" .sh)"
LOG_FILE="$LOG_DIR/${SCRIPT_NAME}_$(date '+%Y%m%d_%H%M%S').log"
exec > >(tee -a "$LOG_FILE") 2>&1
echo "Log file: $LOG_FILE"
echo ""

echo "Building Texture Loading Test..."

# Create build directory
mkdir -p build_test_texture
cd build_test_texture

# Compile and link test
g++ -std=c++17 -I../include -I../external -I../external/stb \
    ../test_texture_loading.cpp \
    -o test_texture_loading

if [ $? -eq 0 ]; then
    echo "Build successful!"
    echo "Running tests..."
    ./test_texture_loading
else
    echo "Build failed!"
    exit 1
fi
