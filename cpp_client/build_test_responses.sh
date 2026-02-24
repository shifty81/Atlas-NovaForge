#!/bin/bash
# Build script for server response handling test

# --- Logging Setup ---
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
LOG_DIR="$SCRIPT_DIR/logs"
mkdir -p "$LOG_DIR"
SCRIPT_NAME="$(basename "$0" .sh)"
LOG_FILE="$LOG_DIR/${SCRIPT_NAME}_$(date '+%Y%m%d_%H%M%S').log"
exec > >(tee -a "$LOG_FILE") 2>&1
echo "Log file: $LOG_FILE"
echo ""

set -e  # Exit on error

echo "Building Server Response Handling Test..."

# Create build directory
mkdir -p build_test_responses
cd build_test_responses

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Build the test
cmake --build . --target test_server_responses

echo ""
echo "Build complete! Running test..."
echo ""

# Run the test
./test_server_responses

echo ""
echo "Test complete!"
