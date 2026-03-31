cmake_minimum_required(VERSION 3.10)
project(aisd_labs C)

add_executable(lab01 lab01/main.c)
add_executable(lab02 lab02/main.c)

if (WIN32)
    add_custom_target(run_tests
        COMMAND powershell -ExecutionPolicy Bypass -File ${CMAKE_SOURCE_DIR}/tests/run_tests.ps1
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )
endif()
