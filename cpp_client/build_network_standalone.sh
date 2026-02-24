#!/bin/bash
# Build network test (standalone, no OpenGL required)

# --- Logging Setup ---
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
LOG_DIR="$SCRIPT_DIR/logs"
mkdir -p "$LOG_DIR"
SCRIPT_NAME="$(basename "$0" .sh)"
LOG_FILE="$LOG_DIR/${SCRIPT_NAME}_$(date '+%Y%m%d_%H%M%S').log"
exec > >(tee -a "$LOG_FILE") 2>&1
echo "Log file: $LOG_FILE"
echo ""

echo "Building network test (standalone)..."

# Create build directory
mkdir -p build_test_network
cd build_test_network

# Compile the network components
echo "Compiling tcp_client.cpp..."
g++ -std=c++17 -c ../src/network/tcp_client.cpp -I../include -I../external/json/include -o tcp_client.o

echo "Compiling protocol_handler.cpp..."
g++ -std=c++17 -c ../src/network/protocol_handler.cpp -I../include -I../external/json/include -o protocol_handler.o

echo "Compiling network_manager.cpp..."
g++ -std=c++17 -c ../src/network/network_manager.cpp -I../include -I../external/json/include -o network_manager.o

echo "Compiling test_network.cpp..."
g++ -std=c++17 -c ../test_network.cpp -I../include -I../external/json/include -o test_network.o

echo "Linking..."
g++ -std=c++17 tcp_client.o protocol_handler.o network_manager.o test_network.o -lpthread -o test_network

if [ -f test_network ]; then
    echo "Build complete! Binary: build_test_network/test_network"
    echo ""
    echo "To run against Python server:"
    echo "  1. Start Python server: python server/server.py"
    echo "  2. Run test: ./build_test_network/test_network"
else
    echo "Build failed!"
    exit 1
fi
