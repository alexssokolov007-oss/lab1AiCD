#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_VALUE_LEN 128
#define MIN_ELEMENTS 11

typedef struct {
    int key;
    char value[MAX_VALUE_LEN];
} Record;

typedef struct {
    Record *items;
    int size;
} Table;

Table create_table(int size) {
    Table table;
    table.size = size;
    table.items = (Record *)calloc((size_t)size, sizeof(Record));

    if (table.items == NULL) {
        printf("Ошибка: не удалось выделить память под таблицу.\n");
        exit(1);
    }

    return table;
}

void free_table(Table *table) {
    if (table == NULL) {
        return;
    }

    free(table->items);
    table->items = NULL;
    table->size = 0;
}

Table copy_table(const Table *source) {
    Table copy = create_table(source->size);

    for (int i = 0; i < source->size; i++) {
        copy.items[i] = source->items[i];
    }

    return copy;
}

Table read_table_from_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Ошибка: не удалось открыть файл '%s'.\n", filename);
        exit(1);
    }

    int n;
    if (fscanf(file, "%d", &n) != 1) {
        printf("Ошибка: не удалось прочитать количество элементов.\n");
        fclose(file);
        exit(1);
    }

    if (n < MIN_ELEMENTS) {
        printf("Ошибка: для структуры №1 требуется минимум %d элементов.\n", MIN_ELEMENTS);
        fclose(file);
        exit(1);
    }

    Table table = create_table(n);

    for (int i = 0; i < n; i++) {
        if (fscanf(file, "%d %127s", &table.items[i].key, table.items[i].value) != 2) {
            printf("Ошибка: не удалось прочитать запись %d.\n", i + 1);
            free_table(&table);
            fclose(file);
            exit(1);
        }
    }

    fclose(file);
    return table;
}

void print_table(const Table *table, const char *title) {
    printf("\n%s\n", title);
    printf("--------------------------------\n");
    printf("| %-8s | %-15s |\n", "KEY", "VALUE");
    printf("--------------------------------\n");

    for (int i = 0; i < table->size; i++) {
        printf("| %-8d | %-15s |\n", table->items[i].key, table->items[i].value);
    }

    printf("--------------------------------\n");
}

void swap_records(Record *a, Record *b) {
    Record temp = *a;
    *a = *b;
    *b = temp;
}

void bubble_sort(Table *table) {
    for (int i = 0; i < table->size - 1; i++) {
        int swapped = 0;

        for (int j = 0; j < table->size - 1 - i; j++) {
            if (table->items[j].key > table->items[j + 1].key) {
                swap_records(&table->items[j], &table->items[j + 1]);
                swapped = 1;
            }
        }

        if (!swapped) {
            break;
        }
    }
}

int binary_search(const Table *table, int target_key) {
    int left = 0;
    int right = table->size - 1;

    while (left <= right) {
        int mid = left + (right - left) / 2;

        if (table->items[mid].key == target_key) {
            return mid;
        }

        if (table->items[mid].key < target_key) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }

    return -1;
}

int compare_records(const void *left, const void *right) {
    const Record *a = (const Record *)left;
    const Record *b = (const Record *)right;

    return (a->key > b->key) - (a->key < b->key);
}

Table make_sorted_copy(const Table *source) {
    Table result = copy_table(source);
    qsort(result.items, (size_t)result.size, sizeof(Record), compare_records);
    return result;
}

void reverse_table(Table *table) {
    for (int i = 0; i < table->size / 2; i++) {
        swap_records(&table->items[i], &table->items[table->size - 1 - i]);
    }
}

void process_case(const Table *source, const char *case_name) {
    Table table = copy_table(source);

    print_table(&table, case_name);
    bubble_sort(&table);
    print_table(&table, "После пузырьковой сортировки:");

    printf("\nВведите целые ключи для поиска в отсортированной таблице.\n");
    printf("Для завершения введите слово stop.\n");

    while (1) {
        char input[64];
        printf("Ключ: ");

        if (scanf("%63s", input) != 1) {
            break;
        }

        if (strcmp(input, "stop") == 0) {
            break;
        }

        char *endptr;
        long key = strtol(input, &endptr, 10);

        if (*endptr != '\0') {
            printf("Некорректный целый ключ.\n");
            continue;
        }

        int index = binary_search(&table, (int)key);
        if (index >= 0) {
            printf("Найдена запись: key = %d, value = %s\n",
                   table.items[index].key,
                   table.items[index].value);
        } else {
            printf("Запись с ключом %ld не найдена.\n", key);
        }
    }

    free_table(&table);
}

#ifndef UNIT_TEST
int main(void) {
    Table base = read_table_from_file("input.txt");

    Table sorted_case = make_sorted_copy(&base);
    Table reverse_case = copy_table(&sorted_case);
    Table unsorted_case = copy_table(&base);

    reverse_table(&reverse_case);

    process_case(&sorted_case, "Случай 1: таблица уже упорядочена");
    process_case(&reverse_case, "Случай 2: таблица в обратном порядке");
    process_case(&unsorted_case, "Случай 3: таблица неупорядочена");

    free_table(&base);
    free_table(&sorted_case);
    free_table(&reverse_case);
    free_table(&unsorted_case);

    return 0;
}
#endif
