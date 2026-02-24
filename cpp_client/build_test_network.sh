#!/bin/bash
# Build network test

# --- Logging Setup ---
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
LOG_DIR="$SCRIPT_DIR/logs"
mkdir -p "$LOG_DIR"
SCRIPT_NAME="$(basename "$0" .sh)"
LOG_FILE="$LOG_DIR/${SCRIPT_NAME}_$(date '+%Y%m%d_%H%M%S').log"
exec > >(tee -a "$LOG_FILE") 2>&1
echo "Log file: $LOG_FILE"
echo ""

echo "Building network test..."

# Create build directory
mkdir -p build_test_network
cd build_test_network

# Run CMake
cmake .. -DBUILD_TESTS=ON -DUSE_SYSTEM_LIBS=ON

# Build only the test
cmake --build . --target test_network

echo "Build complete. Binary: build_test_network/test_network"
