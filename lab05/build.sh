#!/usr/bin/env bash
# Build script for lab05 (MSYS2 / MinGW-w64 environment)
# Run from the lab05 directory: bash build.sh
set -e

LAB="$(cd "$(dirname "$0")" && pwd)"
BUILD="$LAB/build_out"
INC="$LAB/include"
GTEST_INC="/c/msys64/mingw64/include"
GTEST_LIB="/c/msys64/mingw64/lib"
C_FLAGS="-std=c11 -O2 -Wall -I$INC"
CXX_FLAGS="-std=c++17 -O2 -Wall -I$INC"

mkdir -p "$BUILD"

echo "[1/4] Compiling library (C)..."
gcc $C_FLAGS -c "$LAB/src/graph.c"          -o "$BUILD/graph.o"
gcc $C_FLAGS -c "$LAB/src/floyd_warshall.c" -o "$BUILD/floyd_warshall.o"

echo "[2/4] Linking main executable (C)..."
gcc $C_FLAGS "$LAB/src/main.c" "$BUILD/graph.o" "$BUILD/floyd_warshall.o" \
    -lm -o "$BUILD/lab05_main.exe"

echo "[3/4] Linking tests (C++)..."
g++ $CXX_FLAGS -I"$GTEST_INC" \
    "$LAB/tests/test_graph.cpp" "$BUILD/graph.o" "$BUILD/floyd_warshall.o" \
    -L"$GTEST_LIB" -lgtest_main -lgtest -lpthread \
    -o "$BUILD/lab05_tests.exe"

echo "[4/4] Linking benchmark (C)..."
gcc $C_FLAGS "$LAB/bench/bench.c" "$BUILD/graph.o" "$BUILD/floyd_warshall.o" \
    -lm -o "$BUILD/lab05_bench.exe"

echo ""
echo "=== Build successful ==="
echo "Run tests:     $BUILD/lab05_tests.exe"
echo "Run benchmark: $BUILD/lab05_bench.exe"
echo "Run main:      $BUILD/lab05_main.exe"
