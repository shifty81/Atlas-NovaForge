#!/bin/bash
# Build test_lighting

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

echo "Building test_lighting..."

# Create build directory
mkdir -p cpp_client/build_test_lighting
cd cpp_client/build_test_lighting

# Run CMake
cmake .. \
    -DCMAKE_BUILD_TYPE=Debug \
    -DBUILD_TESTS=ON

# Build the test
cmake --build . --target test_lighting -j$(nproc)

echo "Build complete!"
echo "Run with: ./cpp_client/build_test_lighting/bin/test_lighting"
