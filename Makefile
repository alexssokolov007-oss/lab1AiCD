CC=gcc
CFLAGS=-std=c11 -O2 -Wall -Wextra -pedantic

LAB01=lab01/lab01
LAB02=lab02/lab02

.PHONY: all lab01 lab02 test clean

all: lab01 lab02

lab01:
	$(CC) $(CFLAGS) lab01/main.c -o $(LAB01)

lab02:
	$(CC) $(CFLAGS) lab02/main.c -o $(LAB02)

test:
	powershell -ExecutionPolicy Bypass -File tests/run_tests.ps1

clean:
	-@del /Q lab01\lab01.exe lab02\lab02.exe 2>NUL
