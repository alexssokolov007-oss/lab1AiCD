# Лабораторная работа 3

**ФИО:** Соколов Александр Сергеевич

**Вариант:** 19

## Задание

Вынести из частных (операций деления) унарные минусы:

```
(-a) / b    →  -(a / b)
a / (-b)    →  -(a / b)
(-a) / (-b) →  a / b
```

## Сборка и запуск

```bash
gcc -std=c11 -Wall -Wextra -pedantic -o main main.c
main.exe
```

## Тесты

```bash
gcc -std=c11 -Wall -Wextra -pedantic -DUNIT_TEST -o tests tests.c main.c
tests.exe
```
