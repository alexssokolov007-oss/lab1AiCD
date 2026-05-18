CC     = gcc
CXX    = g++
CFLAGS = -std=c11  -O2 -Wall -Wextra -pedantic
CXXFLAGS = -std=c++17 -O2 -Wall
GTEST  = $(shell pkg-config --cflags --libs gtest)

.PHONY: all tests run run_small clean

all: lab05

lab05: main.c
	$(CC) $(CFLAGS) main.c -o lab05

tests: tests.cpp main.c
	$(CXX) $(CXXFLAGS) tests.cpp -o tests $(GTEST)

run: lab05
	./lab05 input.txt

run_small: lab05
	./lab05 input_small.txt

clean:
	-@del /Q lab05.exe tests.exe 2>NUL
