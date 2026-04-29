CC     = gcc
CFLAGS = -std=c11 -Wall -Wextra -pedantic

all: main.exe tests.exe

main.exe: main.c
	$(CC) $(CFLAGS) -o main.exe main.c

tests.exe: tests.c main.c
	$(CC) $(CFLAGS) -DUNIT_TEST -o tests.exe tests.c main.c

clean:
	del /Q main.exe tests.exe output.txt 2>nul || true
