#!/bin/bash

# Build shadow mapping test

# --- Logging Setup ---
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
LOG_DIR="$SCRIPT_DIR/logs"
mkdir -p "$LOG_DIR"
SCRIPT_NAME="$(basename "$0" .sh)"
LOG_FILE="$LOG_DIR/${SCRIPT_NAME}_$(date '+%Y%m%d_%H%M%S').log"
exec > >(tee -a "$LOG_FILE") 2>&1
echo "Log file: $LOG_FILE"
echo ""

BUILD_DIR="build_test_shadow"

echo "Building Shadow Mapping Test..."

# Create build directory
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Run CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build
make test_shadow_mapping

echo ""
echo "Build complete!"
echo "Run with: ./$BUILD_DIR/bin/test_shadow_mapping"
