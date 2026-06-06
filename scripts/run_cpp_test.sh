#!/bin/bash
# Usage: ./scripts/run_cpp_test.sh monteCarloEngine

if [ -z "$1" ]; then
    echo "Usage: ./scripts/run_cpp_test.sh <testFileName>"
    exit 1
fi

NAME=$1
REPO_DIR="$(cd "$(dirname "$0")/.." && pwd)"
SRC_DIR="$REPO_DIR/code/cpp/src"
TESTS_DIR="$REPO_DIR/code/cpp/tests"

g++ -std=c++20 \
    -I/opt/homebrew/include \
    -I"$SRC_DIR" \
    "$TESTS_DIR/$NAME.cpp" \
    "$SRC_DIR/$NAME.cpp" \
    -L/opt/homebrew/lib \
    -lCatch2Main -lCatch2 \
    -o "$TESTS_DIR/$NAME.test" && "$TESTS_DIR/$NAME.test"
