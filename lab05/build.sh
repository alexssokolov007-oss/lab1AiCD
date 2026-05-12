#!/usr/bin/env bash
# Build script for lab05 (MSYS2 / MinGW-w64 environment)
# Run from the lab05 directory: bash build.sh
set -e

LAB="$(cd "$(dirname "$0")" && pwd)"
BUILD="$LAB/build_out"
INC="$LAB/include"
GTEST_INC="/c/msys64/mingw64/include"
GTEST_LIB="/c/msys64/mingw64/lib"
CXX_FLAGS="-std=c++17 -O2 -Wall -I$INC"

mkdir -p "$BUILD"

echo "[1/4] Compiling library..."
g++ $CXX_FLAGS -c "$LAB/src/graph.cpp"          -o "$BUILD/graph.o"
g++ $CXX_FLAGS -c "$LAB/src/floyd_warshall.cpp" -o "$BUILD/floyd_warshall.o"

echo "[2/4] Linking main executable..."
g++ $CXX_FLAGS "$LAB/src/main.cpp" "$BUILD/graph.o" "$BUILD/floyd_warshall.o" \
    -o "$BUILD/lab05_main.exe"

echo "[3/4] Linking tests..."
g++ $CXX_FLAGS -I"$GTEST_INC" \
    "$LAB/tests/test_graph.cpp" "$BUILD/graph.o" "$BUILD/floyd_warshall.o" \
    -L"$GTEST_LIB" -lgtest_main -lgtest -lpthread \
    -o "$BUILD/lab05_tests.exe"

echo "[4/4] Linking benchmark..."
g++ $CXX_FLAGS "$LAB/bench/bench.cpp" "$BUILD/graph.o" "$BUILD/floyd_warshall.o" \
    -o "$BUILD/lab05_bench.exe"

echo ""
echo "=== Build successful ==="
echo "Run tests:     $BUILD/lab05_tests.exe"
echo "Run benchmark: $BUILD/lab05_bench.exe"
echo "Run main:      $BUILD/lab05_main.exe"
