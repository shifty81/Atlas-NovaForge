#!/bin/bash
# Build script for Phase 4.6 advanced features test

# --- Logging Setup ---
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
LOG_DIR="$SCRIPT_DIR/logs"
mkdir -p "$LOG_DIR"
SCRIPT_NAME="$(basename "$0" .sh)"
LOG_FILE="$LOG_DIR/${SCRIPT_NAME}_$(date '+%Y%m%d_%H%M%S').log"
exec > >(tee -a "$LOG_FILE") 2>&1
echo "Log file: $LOG_FILE"
echo ""

set -e

echo "Building Phase 4.6 Advanced Features Test..."

# Create build directory
BUILD_DIR="build_test_phase46"
mkdir -p $BUILD_DIR
cd $BUILD_DIR

# Configure CMake
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Build the test program
cmake --build . --target test_phase46_advanced -j$(nproc)

echo "Build complete!"
echo "Run with: ./$BUILD_DIR/bin/test_phase46_advanced"
