#!/bin/bash

# --- Logging Setup ---
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
LOG_DIR="$SCRIPT_DIR/logs"
mkdir -p "$LOG_DIR"
SCRIPT_NAME="$(basename "$0" .sh)"
LOG_FILE="$LOG_DIR/${SCRIPT_NAME}_$(date '+%Y%m%d_%H%M%S').log"
exec > >(tee -a "$LOG_FILE") 2>&1
echo "Log file: $LOG_FILE"
echo ""

echo "╔════════════════════════════════════════════════════════════════╗"
echo "║          C++ Client Dependencies Verification                  ║"
echo "╚════════════════════════════════════════════════════════════════╝"
echo ""

PASS=0
FAIL=0

check_file() {
    local file=$1
    local desc=$2
    if [ -f "$file" ]; then
        local size=$(ls -lh "$file" | awk '{print $5}')
        echo "✅ $desc"
        echo "   └─ $file ($size)"
        ((PASS++))
    else
        echo "❌ $desc"
        echo "   └─ $file (NOT FOUND)"
        ((FAIL++))
    fi
}

check_package() {
    local package=$1
    local desc=$2
    if pkg-config --exists "$package" 2>/dev/null; then
        local version=$(pkg-config --modversion "$package")
        echo "✅ $desc (v$version)"
        ((PASS++))
    else
        echo "⚠️  $package not found in pkg-config"
        ((FAIL++))
    fi
}

echo "📦 Checking Bundled Dependencies..."
echo ""

check_file "external/glad/src/glad.c" "GLAD Source"
check_file "external/glad/include/glad/glad.h" "GLAD Header"
check_file "external/glad/include/glad/khrplatform.h" "KHR Platform Header"
check_file "external/json/include/json.hpp" "nlohmann/json"

echo ""
echo "🔧 Checking System Libraries..."
echo ""

check_package "glfw3" "GLFW"
check_package "glm" "GLM"

echo ""
echo "🔨 Checking Build Tools..."
echo ""

if command -v cmake &> /dev/null; then
    cmake_version=$(cmake --version | head -n1 | awk '{print $3}')
    echo "✅ CMake (v$cmake_version)"
    ((PASS++))
else
    echo "❌ CMake not found"
    ((FAIL++))
fi

if command -v make &> /dev/null; then
    make_version=$(make --version | head -n1 | awk '{print $3}')
    echo "✅ Make (v$make_version)"
    ((PASS++))
else
    echo "❌ Make not found"
    ((FAIL++))
fi

if command -v g++ &> /dev/null; then
    gcc_version=$(g++ --version | head -n1)
    echo "✅ G++ ($gcc_version)"
    ((PASS++))
else
    echo "❌ G++ not found"
    ((FAIL++))
fi

echo ""
echo "📊 Compilation Test..."
echo ""

# Create test file
cat > /tmp/test_deps.cpp << 'TESTCODE'
#include <glad/glad.h>
#include <json.hpp>
using json = nlohmann::json;
int main() { 
    json j; 
    return GL_VERSION_1_0; 
}
TESTCODE

if g++ -I./external/glad/include -I./external/json/include -std=c++17 \
   -o /tmp/test_compile /tmp/test_deps.cpp 2>/dev/null; then
    echo "✅ Dependencies compile successfully"
    rm -f /tmp/test_compile /tmp/test_deps.cpp
    ((PASS++))
else
    echo "❌ Compilation test failed"
    ((FAIL++))
fi

echo ""
echo "╔════════════════════════════════════════════════════════════════╗"
echo "║                      Verification Results                      ║"
echo "╠════════════════════════════════════════════════════════════════╣"
printf "║ ✅ Passed: %-45s ║\n" "$PASS"
printf "║ ❌ Failed: %-45s ║\n" "$FAIL"
echo "╚════════════════════════════════════════════════════════════════╝"
echo ""

if [ $FAIL -eq 0 ]; then
    echo "🎉 All dependencies verified successfully!"
    echo ""
    echo "Next steps:"
    echo "  cd build"
    echo "  cmake -DCMAKE_BUILD_TYPE=Release -DUSE_SYSTEM_LIBS=ON .."
    echo "  make -j\$(nproc)"
    echo ""
    exit 0
else
    echo "⚠️  Some dependencies are missing or misconfigured"
    echo "Please review the errors above"
    echo ""
    exit 1
fi
