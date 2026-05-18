CC       = gcc
CXX      = g++
CFLAGS   = -std=c11 -O2 -Wall -Wextra -pedantic
CXXFLAGS = -std=c++17 -O2 -Wall
GTEST    = $(shell pkg-config --cflags --libs gtest)

.PHONY: all tests run clean

all: lab06

lab06: main.c
	$(CC) $(CFLAGS) main.c -o lab06

tests: tests.cpp main.c
	$(CXX) $(CXXFLAGS) tests.cpp -o tests $(GTEST)

run: lab06
	./lab06 .

clean:
	-@del /Q lab06.exe tests.exe 2>NUL
