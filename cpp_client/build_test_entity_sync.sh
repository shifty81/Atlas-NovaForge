#!/bin/bash

# Build script for entity synchronization test
# Simple direct compilation without full CMake setup

# --- Logging Setup ---
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
LOG_DIR="$SCRIPT_DIR/logs"
mkdir -p "$LOG_DIR"
SCRIPT_NAME="$(basename "$0" .sh)"
LOG_FILE="$LOG_DIR/${SCRIPT_NAME}_$(date '+%Y%m%d_%H%M%S').log"
exec > >(tee -a "$LOG_FILE") 2>&1
echo "Log file: $LOG_FILE"
echo ""

echo "Building entity synchronization test..."

cd "$(dirname "$0")"

# Compile
g++ -std=c++17 \
    -I./include \
    test_entity_sync.cpp \
    src/core/entity.cpp \
    src/core/entity_manager.cpp \
    src/core/entity_message_parser.cpp \
    -o test_entity_sync \
    -lpthread

if [ $? -eq 0 ]; then
    echo "✓ Build successful!"
    echo ""
    echo "Run with: ./test_entity_sync"
else
    echo "✗ Build failed!"
    exit 1
fi
