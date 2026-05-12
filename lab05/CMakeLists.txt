cmake_minimum_required(VERSION 3.14)
project(lab05 C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---------- Shared library (C) ----------
add_library(graph_lib
    src/graph.c
    src/floyd_warshall.c
)
target_include_directories(graph_lib PUBLIC include)
if(UNIX)
    target_link_libraries(graph_lib PUBLIC m)
endif()

# ---------- Main executable (C) ----------
add_executable(lab05_main src/main.c)
target_link_libraries(lab05_main PRIVATE graph_lib)

# ---------- Google Test (C++) ----------
find_package(GTest QUIET)

if(NOT GTest_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
endif()

enable_testing()

add_executable(lab05_tests tests/test_graph.cpp)
target_link_libraries(lab05_tests PRIVATE graph_lib GTest::gtest_main)

add_test(NAME lab05_tests COMMAND lab05_tests)

# ---------- Benchmark (C) ----------
add_executable(lab05_bench bench/bench.c)
target_link_libraries(lab05_bench PRIVATE graph_lib)
