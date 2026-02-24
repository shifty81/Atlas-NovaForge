#!/bin/bash
# Build script for test_enhanced_ui (Phase 4.5)

# --- Logging Setup ---
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
LOG_DIR="$SCRIPT_DIR/logs"
mkdir -p "$LOG_DIR"
SCRIPT_NAME="$(basename "$0" .sh)"
LOG_FILE="$LOG_DIR/${SCRIPT_NAME}_$(date '+%Y%m%d_%H%M%S').log"
exec > >(tee -a "$LOG_FILE") 2>&1
echo "Log file: $LOG_FILE"
echo ""

echo "Building Phase 4.5 Enhanced UI test..."

# Create build directory
mkdir -p build
cd build

# Configure with CMake
cmake .. -DBUILD_TESTS=ON

# Build test_enhanced_ui
make test_enhanced_ui

if [ $? -eq 0 ]; then
    echo ""
    echo "Build successful!"
    echo "Run with: ./bin/test_enhanced_ui"
    echo ""
    echo "Controls:"
    echo "  F1 - Toggle Inventory Panel"
    echo "  F2 - Toggle Fitting Panel"
    echo "  F3 - Toggle Mission Panel"
    echo "  ESC - Exit"
else
    echo ""
    echo "Build failed!"
    exit 1
fi
