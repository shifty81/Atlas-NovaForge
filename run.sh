#!/usr/bin/env bash
#
# Atlas Engine — Quick Launcher
#
# Usage:
#   ./run.sh editor                    # Launch the editor
#   ./run.sh server                    # Launch headless server
#   ./run.sh client                    # Launch the client
#   ./run.sh runtime <project.atlas>   # Launch runtime with a project
#   ./run.sh test                      # Run tests
#

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
DIST_DIR="$SCRIPT_DIR/dist"
BUILD_DIR="$SCRIPT_DIR/build"

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
    echo -e "${BOLD}Atlas Engine — Quick Launcher${RESET}"
    echo ""
    echo "Usage: ./run.sh <target> [args...]"
    echo ""
    echo "Targets:"
    echo "  editor              Launch AtlasEditor"
    echo "  server              Launch AtlasServer"
    echo "  client              Launch AtlasClient"
    echo "  runtime [args...]   Launch AtlasRuntime with optional arguments"
    echo "  test                Build and run tests"
    echo ""
    echo "Examples:"
    echo "  ./run.sh editor"
    echo "  ./run.sh runtime --project projects/atlas-sample/sample.atlas"
    echo "  ./run.sh test"
    exit 0
}

if [ $# -eq 0 ]; then
    usage
fi

TARGET="$1"
shift

# Map target names to binary names and build subdirectories
declare -A BINARY_MAP=(
    ["editor"]="AtlasEditor"
    ["server"]="AtlasServer"
    ["client"]="AtlasClient"
    ["runtime"]="AtlasRuntime"
)

declare -A BUILD_SUBDIR_MAP=(
    ["editor"]="editor"
    ["server"]="server"
    ["client"]="client"
    ["runtime"]="runtime"
)

# Handle test target separately
if [ "$TARGET" = "test" ]; then
    info "Building and running tests..."
    exec "$SCRIPT_DIR/build.sh" --test tests
fi

# Validate target
binary_name="${BINARY_MAP[$TARGET]:-}"
if [ -z "$binary_name" ]; then
    error "Unknown target: $TARGET"
    echo "Valid targets: editor, server, client, runtime, test"
    exit 1
fi

# Find the executable — check dist/ first, then build subdirectory
find_executable() {
    if [ -f "$DIST_DIR/$binary_name" ]; then
        echo "$DIST_DIR/$binary_name"
    elif [ -f "$BUILD_DIR/${BUILD_SUBDIR_MAP[$TARGET]}/$binary_name" ]; then
        echo "$BUILD_DIR/${BUILD_SUBDIR_MAP[$TARGET]}/$binary_name"
    fi
}

executable="$(find_executable)"

# If not found, build first
if [ -z "$executable" ]; then
    warn "$binary_name not found — building first..."
    "$SCRIPT_DIR/build.sh" "$TARGET"
    executable="$(find_executable)"
fi

if [ -z "$executable" ]; then
    error "Failed to locate $binary_name after build."
    exit 1
fi

info "Launching $binary_name..."
info "  $executable $*"
echo ""
exec "$executable" "$@"
