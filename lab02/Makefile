main_prog: main.c
	gcc -std=c11 -Wall -Wextra -pedantic -o main main.c

tests: tests.c main.c
	gcc -std=c11 -Wall -Wextra -pedantic -DUNIT_TEST -o tests tests.c main.c

run: main_prog
	./main

run_tests: tests
	./tests

clean:
	rm -f main tests main.exe tests.exe
