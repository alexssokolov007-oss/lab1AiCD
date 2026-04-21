#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int key;
    char value[128];
} Record;

typedef struct {
    Record *items;
    int size;
} Table;

Table create_table(int size);
void free_table(Table *table);
Table read_table_from_file(const char *filename);
void bubble_sort(Table *table);
int binary_search(const Table *table, int target_key);
Table make_sorted_copy(const Table *source);
void reverse_table(Table *table);

static void write_test_input_file(const char *filename) {
    FILE *file = fopen(filename, "w");
    assert(file != NULL);

    fprintf(file, "11\n");
    fprintf(file, "42 alpha\n");
    fprintf(file, "7 beta\n");
    fprintf(file, "19 gamma\n");
    fprintf(file, "3 delta\n");
    fprintf(file, "88 epsilon\n");
    fprintf(file, "55 zeta\n");
    fprintf(file, "12 eta\n");
    fprintf(file, "31 theta\n");
    fprintf(file, "64 iota\n");
    fprintf(file, "1 kappa\n");
    fprintf(file, "27 lambda\n");

    fclose(file);
}

static void fill_table(Table *table, const int *keys, int size) {
    for (int i = 0; i < size; i++) {
        table->items[i].key = keys[i];
        snprintf(table->items[i].value, sizeof(table->items[i].value), "value%d", keys[i]);
    }
}

static void assert_sorted(const Table *table) {
    for (int i = 1; i < table->size; i++) {
        assert(table->items[i - 1].key <= table->items[i].key);
    }
}

static void test_read_table_from_file(void) {
    const char *filename = "test_input.txt";
    write_test_input_file(filename);

    Table table = read_table_from_file(filename);

    assert(table.size == 11);
    assert(table.items[0].key == 42);
    assert(strcmp(table.items[0].value, "alpha") == 0);

    free_table(&table);
    remove(filename);
    printf("test_read_table_from_file passed\n");
}

static void test_bubble_sort_unsorted(void) {
    int keys[] = {5, -2, 9, 0, 4, 1};
    Table table = create_table(6);
    fill_table(&table, keys, 6);

    bubble_sort(&table);

    assert_sorted(&table);
    assert(table.items[0].key == -2);
    assert(table.items[5].key == 9);

    free_table(&table);
    printf("test_bubble_sort_unsorted passed\n");
}

static void test_bubble_sort_sorted(void) {
    int keys[] = {1, 2, 3, 4, 5};
    Table table = create_table(5);
    fill_table(&table, keys, 5);

    bubble_sort(&table);

    assert_sorted(&table);
    assert(table.items[0].key == 1);
    assert(table.items[4].key == 5);

    free_table(&table);
    printf("test_bubble_sort_sorted passed\n");
}

static void test_binary_search_found(void) {
    int keys[] = {1, 4, 9, 16, 25, 36};
    Table table = create_table(6);
    fill_table(&table, keys, 6);

    int index = binary_search(&table, 16);

    assert(index == 3);
    assert(strcmp(table.items[index].value, "value16") == 0);

    free_table(&table);
    printf("test_binary_search_found passed\n");
}

static void test_binary_search_not_found(void) {
    int keys[] = {1, 4, 9, 16, 25, 36};
    Table table = create_table(6);
    fill_table(&table, keys, 6);

    int index = binary_search(&table, 10);

    assert(index == -1);

    free_table(&table);
    printf("test_binary_search_not_found passed\n");
}

int main(void) {
    test_read_table_from_file();
    test_bubble_sort_unsorted();
    test_bubble_sort_sorted();
    test_binary_search_found();
    test_binary_search_not_found();

    printf("All tests passed successfully!\n");
    return 0;
}
