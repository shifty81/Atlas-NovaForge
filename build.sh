#!/usr/bin/env bash
#
# Atlas Engine Build Script
#
# Builds the Atlas engine executables:
#   AtlasServer  - Headless authoritative server
#   AtlasClient  - Player runtime client
#   AtlasEditor  - Developer client (editor + runtime superset)
#
# Usage:
#   ./build.sh [options] [targets...]
#
# Targets:
#   server          Build AtlasServer
#   client          Build AtlasClient
#   release-client  Build AtlasClient without dev tools (diagnostics, rewriter)
#   editor          Build AtlasEditor (developer client)
#   runtime         Build AtlasRuntime
#   engine          Build AtlasEngine and AtlasGameplay libraries
#   tests           Build and run tests
#   all             Build all targets (default)
#
# Options:
#   -b, --build-type TYPE   Build type: Debug, Release, Development (default: Release)
#   -j, --jobs N            Parallel build jobs (default: auto-detected)
#   -o, --output DIR        Output directory for executables (default: dist/)
#   -c, --clean             Clean build directory before building
#   -t, --test              Run tests after building
#   -i, --install           Install SDK (headers + libraries) to OUTPUT_DIR/sdk
#   -r, --run TARGET        Build and run TARGET (editor, server, client, runtime)
#   -h, --help              Show this help message
#
# Examples:
#   ./build.sh                           # Build all targets in Release mode
#   ./build.sh server client             # Build server and client only
#   ./build.sh -b Debug editor --test    # Build editor in Debug mode and run tests
#   ./build.sh --clean all               # Clean rebuild of everything
#   ./build.sh --run editor              # Build and run AtlasEditor
#   ./build.sh --run runtime -- --project projects/atlas-sample/sample.atlas
#

set -euo pipefail

# --- Defaults ---
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SOURCE_DIR="$SCRIPT_DIR"
BUILD_DIR="$SOURCE_DIR/build"
BUILD_TYPE="Release"
OUTPUT_DIR="$SOURCE_DIR/dist"
CLEAN=false
RUN_TESTS=false
INSTALL_SDK=false
STRIP_TOOLS=false
RUN_TARGET=""
RUN_ARGS=()
JOBS=""
TARGETS=()

# --- Colors (disabled if not a terminal) ---
if [ -t 1 ]; then
    RED='\033[0;31m'
    GREEN='\033[0;32m'
    YELLOW='\033[1;33m'
    CYAN='\033[0;36m'
    BOLD='\033[1m'
    RESET='\033[0m'
else
    RED='' GREEN='' YELLOW='' CYAN='' BOLD='' RESET=''
fi

info()  { echo -e "${CYAN}[INFO]${RESET}  $*"; }
ok()    { echo -e "${GREEN}[OK]${RESET}    $*"; }
warn()  { echo -e "${YELLOW}[WARN]${RESET}  $*"; }
error() { echo -e "${RED}[ERROR]${RESET} $*" >&2; }

usage() {
    sed -n '/^# Usage:/,/^[^#]/{ /^#/s/^# \?//p }' "$0"
    exit 0
}

# --- Parse arguments ---
while [[ $# -gt 0 ]]; do
    case "$1" in
        -b|--build-type)
            BUILD_TYPE="$2"; shift 2 ;;
        -j|--jobs)
            JOBS="$2"; shift 2 ;;
        -o|--output)
            OUTPUT_DIR="$2"; shift 2 ;;
        -c|--clean)
            CLEAN=true; shift ;;
        -t|--test)
            RUN_TESTS=true; shift ;;
        -r|--run)
            RUN_TARGET="$2"; shift 2 ;;
        -i|--install)
            INSTALL_SDK=true; shift ;;
        --) shift; RUN_ARGS=("$@"); set -- ;;
        -h|--help)
            usage ;;
        -*)
            error "Unknown option: $1"; usage ;;
        *)
            TARGETS+=("$1"); shift ;;
    esac
done

# If --run is specified, set that as the sole target
if [ -n "$RUN_TARGET" ]; then
    TARGETS=("$RUN_TARGET")
fi

# Default to "all" if no targets specified
if [ ${#TARGETS[@]} -eq 0 ]; then
    TARGETS=("all")
fi

# --- Map build type ---
CMAKE_BUILD_TYPE="$BUILD_TYPE"
case "$BUILD_TYPE" in
    Debug|debug)
        CMAKE_BUILD_TYPE="Debug" ;;
    Release|release)
        CMAKE_BUILD_TYPE="Release" ;;
    Development|development|Dev|dev)
        CMAKE_BUILD_TYPE="RelWithDebInfo" ;;
    *)
        error "Unknown build type: $BUILD_TYPE (expected Debug, Release, or Development)"
        exit 1 ;;
esac

# --- Detect parallel jobs ---
if [ -z "$JOBS" ]; then
    if command -v nproc &>/dev/null; then
        JOBS=$(nproc)
    elif command -v sysctl &>/dev/null; then
        JOBS=$(sysctl -n hw.ncpu 2>/dev/null || echo 4)
    else
        JOBS=4
    fi
fi

# --- Resolve CMake targets ---
CMAKE_TARGETS=()
BUILD_TESTS=false

for target in "${TARGETS[@]}"; do
    case "$target" in
        server)          CMAKE_TARGETS+=("AtlasServer") ;;
        client)          CMAKE_TARGETS+=("AtlasClient") ;;
        release-client)  CMAKE_TARGETS+=("AtlasClient"); STRIP_TOOLS=true ;;
        editor)          CMAKE_TARGETS+=("AtlasEditor") ;;
        runtime)         CMAKE_TARGETS+=("AtlasRuntime") ;;
        engine)          CMAKE_TARGETS+=("AtlasEngine" "AtlasGameplay") ;;
        tests)           CMAKE_TARGETS+=("AtlasTests"); BUILD_TESTS=true; RUN_TESTS=true ;;
        all)             CMAKE_TARGETS=("AtlasServer" "AtlasClient" "AtlasEditor" "AtlasRuntime"); BUILD_TESTS=false ;;
        *)               error "Unknown target: $target"; exit 1 ;;
    esac
done

# If --test was passed, ensure AtlasTests is built
if [ "$RUN_TESTS" = true ] && [[ ! " ${CMAKE_TARGETS[*]} " =~ " AtlasTests " ]]; then
    CMAKE_TARGETS+=("AtlasTests")
fi

# --- Setup build log ---
LOG_DIR="$SOURCE_DIR/logs"
mkdir -p "$LOG_DIR"
BUILD_LOG="$LOG_DIR/build.log"

# Redirect all output to both console and build log
exec > >(tee -a "$BUILD_LOG") 2>&1

# Write a header to the build log
echo ""
echo "=== Atlas Build Log ==="
echo "Started: $(date '+%Y-%m-%d %H:%M:%S')"
echo ""

# --- Print build configuration ---
echo ""
echo -e "${BOLD}╔══════════════════════════════════════╗${RESET}"
echo -e "${BOLD}║        Atlas Engine Build Script      ║${RESET}"
echo -e "${BOLD}╚══════════════════════════════════════╝${RESET}"
echo ""
info "Build type:    $BUILD_TYPE ($CMAKE_BUILD_TYPE)"
info "Parallel jobs: $JOBS"
info "Output dir:    $OUTPUT_DIR"
info "Targets:       ${CMAKE_TARGETS[*]}"
info "Run tests:     $RUN_TESTS"
info "Install SDK:   $INSTALL_SDK"
if [ -n "$RUN_TARGET" ]; then
    info "Run after build: $RUN_TARGET"
fi
info "Build log:     $BUILD_LOG"
echo ""

# --- Check prerequisites ---
info "Checking prerequisites..."

MISSING_PKGS=()

if command -v cmake &>/dev/null; then
    ok "cmake found: $(cmake --version | head -n1)"
else
    warn "cmake not found"
    MISSING_PKGS+=("cmake")
fi

if command -v g++ &>/dev/null; then
    ok "C++ compiler found: $(g++ --version | head -n1)"
elif command -v clang++ &>/dev/null; then
    ok "C++ compiler found: $(clang++ --version | head -n1)"
else
    warn "No C++ compiler found"
    MISSING_PKGS+=("compiler")
fi

if command -v ninja &>/dev/null; then
    ok "Build tool: ninja"
elif command -v make &>/dev/null; then
    ok "Build tool: make"
else
    warn "No build tool found"
    MISSING_PKGS+=("build-tool")
fi

if [ ${#MISSING_PKGS[@]} -gt 0 ]; then
    echo ""
    info "Missing prerequisites: ${MISSING_PKGS[*]}"
    info "Attempting to install automatically..."

    if command -v apt-get &>/dev/null; then
        APT_PKGS=()
        for pkg in "${MISSING_PKGS[@]}"; do
            case "$pkg" in
                cmake)      APT_PKGS+=("cmake") ;;
                compiler)   APT_PKGS+=("g++") ;;
                build-tool) APT_PKGS+=("make") ;;
            esac
        done
        info "  Installing via apt: ${APT_PKGS[*]}"
        sudo apt-get update -qq 2>&1 | while IFS= read -r line; do echo "    $line"; done
        sudo apt-get install -y -qq "${APT_PKGS[@]}" 2>&1 | while IFS= read -r line; do echo "    $line"; done
        ok "  Packages installed via apt"

    elif command -v dnf &>/dev/null; then
        DNF_PKGS=()
        for pkg in "${MISSING_PKGS[@]}"; do
            case "$pkg" in
                cmake)      DNF_PKGS+=("cmake") ;;
                compiler)   DNF_PKGS+=("gcc-c++") ;;
                build-tool) DNF_PKGS+=("make") ;;
            esac
        done
        info "  Installing via dnf: ${DNF_PKGS[*]}"
        sudo dnf install -y "${DNF_PKGS[@]}" 2>&1 | while IFS= read -r line; do echo "    $line"; done
        ok "  Packages installed via dnf"

    elif command -v brew &>/dev/null; then
        BREW_PKGS=()
        for pkg in "${MISSING_PKGS[@]}"; do
            case "$pkg" in
                cmake)      BREW_PKGS+=("cmake") ;;
                compiler)
                    warn "  On macOS, install Xcode Command Line Tools: xcode-select --install"
                    ;;
                build-tool) BREW_PKGS+=("make") ;;
            esac
        done
        if [ ${#BREW_PKGS[@]} -gt 0 ]; then
            info "  Installing via brew: ${BREW_PKGS[*]}"
            brew install "${BREW_PKGS[@]}" 2>&1 | while IFS= read -r line; do echo "    $line"; done
            ok "  Packages installed via brew"
        fi

    elif command -v pacman &>/dev/null; then
        PAC_PKGS=()
        for pkg in "${MISSING_PKGS[@]}"; do
            case "$pkg" in
                cmake)      PAC_PKGS+=("cmake") ;;
                compiler)   PAC_PKGS+=("gcc") ;;
                build-tool) PAC_PKGS+=("make") ;;
            esac
        done
        info "  Installing via pacman: ${PAC_PKGS[*]}"
        sudo pacman -S --noconfirm "${PAC_PKGS[@]}" 2>&1 | while IFS= read -r line; do echo "    $line"; done
        ok "  Packages installed via pacman"

    else
        echo ""
        error "Could not detect a supported package manager (apt, dnf, brew, pacman)."
        error "Please install the following manually:"
        for pkg in "${MISSING_PKGS[@]}"; do
            case "$pkg" in
                cmake)      error "  - CMake 3.22+  (https://cmake.org/download/)" ;;
                compiler)   error "  - C++ compiler: g++ or clang++" ;;
                build-tool) error "  - Build tool: make or ninja" ;;
            esac
        done
        exit 1
    fi

    # Re-verify after install
    echo ""
    info "Verifying installed tools..."
    HAS_ERRORS=false

    if ! command -v cmake &>/dev/null; then
        error "  cmake still not found after install"
        HAS_ERRORS=true
    else
        ok "  cmake: $(cmake --version | head -n1)"
    fi

    if ! command -v g++ &>/dev/null && ! command -v clang++ &>/dev/null; then
        error "  C++ compiler still not found after install"
        HAS_ERRORS=true
    else
        ok "  C++ compiler: $(g++ --version 2>/dev/null | head -n1 || clang++ --version 2>/dev/null | head -n1)"
    fi

    if ! command -v make &>/dev/null && ! command -v ninja &>/dev/null; then
        error "  Build tool still not found after install"
        HAS_ERRORS=true
    else
        ok "  Build tool: $(make --version 2>/dev/null | head -n1 || ninja --version 2>/dev/null)"
    fi

    if [ "$HAS_ERRORS" = true ]; then
        echo ""
        error "Some prerequisites could not be installed. Please install them manually and re-run."
        exit 1
    fi
fi

echo ""
ok "All prerequisites satisfied."
echo ""

# --- Clean ---
if [ "$CLEAN" = true ]; then
    info "Cleaning build directory..."
    rm -rf "$BUILD_DIR"
    ok "Build directory cleaned"
fi

# --- Configure ---
info "Configuring CMake..."
mkdir -p "$BUILD_DIR"

CMAKE_EXTRA_ARGS=()
if [ "$STRIP_TOOLS" = true ]; then
    CMAKE_EXTRA_ARGS+=("-DATLAS_INCLUDE_TOOLS=OFF")
    info "Tools stripped:  yes (release-client mode)"
fi

cmake -S "$SOURCE_DIR" -B "$BUILD_DIR" \
    -DCMAKE_BUILD_TYPE="$CMAKE_BUILD_TYPE" \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
    -DCMAKE_INSTALL_PREFIX="$OUTPUT_DIR/sdk" \
    "${CMAKE_EXTRA_ARGS[@]+"${CMAKE_EXTRA_ARGS[@]}"}" \
    2>&1 | while IFS= read -r line; do echo "  $line"; done

ok "CMake configured"

# --- Build ---
info "Building targets..."

for target in "${CMAKE_TARGETS[@]}"; do
    info "  Building $target..."
    cmake --build "$BUILD_DIR" --target "$target" -j "$JOBS" \
        2>&1 | while IFS= read -r line; do echo "    $line"; done
    ok "  $target built"
done

ok "All targets built successfully"

# --- Test ---
if [ "$RUN_TESTS" = true ]; then
    echo ""
    info "Running tests..."
    set +e
    test_output=$("$BUILD_DIR/tests/AtlasTests" 2>&1)
    test_exit=$?
    set -e
    echo "$test_output" | while IFS= read -r line; do echo "  $line"; done
    if [ "$test_exit" -ne 0 ]; then
        error "Tests failed with exit code $test_exit"
        exit "$test_exit"
    fi
    ok "All tests passed"
fi

# --- Install SDK ---
if [ "$INSTALL_SDK" = true ]; then
    echo ""
    info "Installing Atlas SDK to $OUTPUT_DIR/sdk..."
    cmake --install "$BUILD_DIR" \
        2>&1 | while IFS= read -r line; do echo "  $line"; done
    ok "Atlas SDK installed to $OUTPUT_DIR/sdk"
    info "External game modules can now use:"
    info "  cmake -DAtlasEngine_DIR=$OUTPUT_DIR/sdk/lib/cmake/AtlasEngine"
    info "  cmake -DAtlasGameplay_DIR=$OUTPUT_DIR/sdk/lib/cmake/AtlasGameplay"
fi

# --- Stage output ---
echo ""
info "Staging executables to $OUTPUT_DIR..."
mkdir -p "$OUTPUT_DIR"

# Map of CMake target name -> subdirectory and binary name
declare -A TARGET_PATHS=(
    ["AtlasServer"]="server/AtlasServer"
    ["AtlasClient"]="client/AtlasClient"
    ["AtlasEditor"]="editor/AtlasEditor"
    ["AtlasRuntime"]="runtime/AtlasRuntime"
)

copied=0
for target in "${CMAKE_TARGETS[@]}"; do
    rel="${TARGET_PATHS[$target]:-}"
    if [ -z "$rel" ]; then
        continue
    fi
    src="$BUILD_DIR/$rel"
    if [ -f "$src" ]; then
        cp "$src" "$OUTPUT_DIR/"
        binary_name=$(basename "$src")
        ok "  $binary_name → $OUTPUT_DIR/$binary_name"
        copied=$((copied + 1))
    fi
done

# --- Summary ---
echo ""
echo -e "${BOLD}═══════════════════════════════════════${RESET}"
echo -e "${GREEN}${BOLD} Build complete!${RESET}"
echo ""
if [ "$copied" -gt 0 ]; then
    info "Executables in $OUTPUT_DIR/:"
    ls -lh "$OUTPUT_DIR"/ | grep -v "^total"
fi
echo -e "${BOLD}═══════════════════════════════════════${RESET}"
echo ""
echo "=== Build Finished: $(date '+%Y-%m-%d %H:%M:%S') ==="
info "Build log saved to: $BUILD_LOG"
echo ""

# --- Run target ---
if [ -n "$RUN_TARGET" ]; then
    # Map short name to binary name
    declare -A RUN_BINARY_MAP=(
        ["server"]="AtlasServer"
        ["client"]="AtlasClient"
        ["editor"]="AtlasEditor"
        ["runtime"]="AtlasRuntime"
    )
    binary_name="${RUN_BINARY_MAP[$RUN_TARGET]:-}"
    if [ -z "$binary_name" ]; then
        error "Cannot run target '$RUN_TARGET'. Valid targets: server, client, editor, runtime"
        exit 1
    fi

    # Try dist/ first, then build subdirectory
    run_path=""
    if [ -f "$OUTPUT_DIR/$binary_name" ]; then
        run_path="$OUTPUT_DIR/$binary_name"
    elif [ -f "$BUILD_DIR/$RUN_TARGET/$binary_name" ]; then
        run_path="$BUILD_DIR/$RUN_TARGET/$binary_name"
    fi

    if [ -z "$run_path" ]; then
        error "Executable '$binary_name' not found in $OUTPUT_DIR/ or $BUILD_DIR/$RUN_TARGET/"
        exit 1
    fi

    echo ""
    info "Launching $binary_name..."
    info "  $run_path ${RUN_ARGS[*]:-}"
    echo ""
    exec "$run_path" "${RUN_ARGS[@]}"
fi
