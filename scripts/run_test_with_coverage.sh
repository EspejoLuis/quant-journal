#!/bin/bash
# run_test_with_coverage.sh - Build and run C++ coverage for QuantLibCpp
# Usage:
#   ./run_test_with_coverage.sh          # reuse existing build
#   ./run_test_with_coverage.sh --clean  # delete build and start fresh

set -e  # stop if any command fails

REPO_DIR="$(cd "$(dirname "$0")/.." && pwd)"
SRC_DIR="$REPO_DIR/code/cpp"


# 1. Optional clean
if [ "$1" == "--clean" ]; then
    echo "Cleaning build-coverage folder..."
    rm -rf "$SRC_DIR/build-coverage"
fi

# 2. Configure (only if folder doesn't exist or was cleaned)
if [ ! -d "$SRC_DIR/build-coverage" ]; then
    echo "Configuring build with coverage instrumentation..."
    cmake -S "$SRC_DIR" -B "$SRC_DIR/build-coverage" -DCMAKE_BUILD_TYPE=Coverage
fi

# 3. Build
echo "Building project..."
cmake --build "$SRC_DIR/build-coverage" 

# 4. Run coverage target
echo "Running coverage..."
cmake --build "$SRC_DIR/build-coverage"  --target coverage
