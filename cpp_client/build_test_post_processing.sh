#!/bin/bash
# Build test for post-processing effects

# --- Logging Setup ---
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
LOG_DIR="$SCRIPT_DIR/logs"
mkdir -p "$LOG_DIR"
SCRIPT_NAME="$(basename "$0" .sh)"
LOG_FILE="$LOG_DIR/${SCRIPT_NAME}_$(date '+%Y%m%d_%H%M%S').log"
exec > >(tee -a "$LOG_FILE") 2>&1
echo "Log file: $LOG_FILE"
echo ""

echo "Building post-processing test..."
cd cpp_client
mkdir -p build
cd build
cmake .. -DBUILD_TESTS=ON
make test_post_processing
echo "Done! Run with: cd cpp_client/build/bin && ./test_post_processing"
