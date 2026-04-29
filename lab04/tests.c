#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct BNode BNode;
typedef struct { BNode *root; } BTree;

void   btree_insert(BTree *t, const char *key, double val);
void   btree_delete(BTree *t, const char *key);
int    btree_search(BNode *root, const char *key, double *out);
void   btree_print(BTree *t);
void   btree_free(BTree *t);


static void check_found(BTree *t, const char *key, double expected) {
    double v = 0.0;
    int ok = btree_search(t->root, key, &v);
    if (!ok || v != expected) {
        printf("FAIL: search(%s): ожидалось %.4g, получено %s=%.4g\n",
               key, expected, ok ? "found" : "NOT FOUND", v);
        assert(0);
    }
    printf("OK: search(%s) = %.4g\n", key, v);
}

static void check_not_found(BTree *t, const char *key) {
    double v;
    if (btree_search(t->root, key, &v)) {
        printf("FAIL: search(%s): должен быть не найден, а нашёл %.4g\n",
               key, v);
        assert(0);
    }
    printf("OK: search(%s) -> не найден\n", key);
}


static void test_basic_insert_search(void) {
    printf("\n--- test_basic_insert_search ---\n");
    BTree t = {NULL};

    btree_insert(&t, "apple", 1.0);
    btree_insert(&t, "fig",   2.0);
    btree_insert(&t, "kiwi",  3.0);

    check_found(&t, "apple", 1.0);
    check_found(&t, "fig",   2.0);
    check_found(&t, "kiwi",  3.0);
    check_not_found(&t, "mango");

    btree_free(&t);
}

static void test_update_existing(void) {
    printf("\n--- test_update_existing ---\n");
    BTree t = {NULL};

    btree_insert(&t, "key", 1.0);
    btree_insert(&t, "key", 9.9);

    check_found(&t, "key", 9.9);
    btree_free(&t);
}

static void test_split_root(void) {
    printf("\n--- test_split_root (вставка 5 ключей вызывает разбиение корня) ---\n");
    BTree t = {NULL};

    const char *keys[] = {"aa", "bb", "cc", "dd", "ee"};
    for (int i = 0; i < 5; i++) btree_insert(&t, keys[i], (double)(i + 1));

    for (int i = 0; i < 5; i++) check_found(&t, keys[i], (double)(i + 1));
    btree_free(&t);
}

static void test_delete_leaf(void) {
    printf("\n--- test_delete_leaf ---\n");
    BTree t = {NULL};

    btree_insert(&t, "aaa", 1.0);
    btree_insert(&t, "bbb", 2.0);
    btree_insert(&t, "ccc", 3.0);

    btree_delete(&t, "bbb");
    check_not_found(&t, "bbb");
    check_found(&t, "aaa", 1.0);
    check_found(&t, "ccc", 3.0);

    btree_free(&t);
}

static void test_delete_nonexistent(void) {
    printf("\n--- test_delete_nonexistent ---\n");
    BTree t = {NULL};
    btree_insert(&t, "aa", 1.0);
    btree_delete(&t, "zz"); 
    check_found(&t, "aa", 1.0);
    btree_free(&t);
}

static void test_large_tree(void) {
    printf("\n--- test_large_tree (25 ключей) ---\n");
    BTree t = {NULL};

    /* добавляем 25 ключей */
    const char *keys[] = {
        "aa","ab","ac","ad","ae","af","ag","ah","ai","aj",
        "ak","al","am","an","ao","ap","aq","ar","as","at",
        "au","av","aw","ax","ay"
    };
    int n = 25;
    for (int i = 0; i < n; i++) btree_insert(&t, keys[i], (double)i);

    /* все ключи найдены */
    for (int i = 0; i < n; i++) check_found(&t, keys[i], (double)i);

    /* удаляем каждый третий */
    for (int i = 0; i < n; i += 3) btree_delete(&t, keys[i]);

    /* проверяем: удалённые не найдены, остальные — найдены */
    for (int i = 0; i < n; i++) {
        if (i % 3 == 0) check_not_found(&t, keys[i]);
        else            check_found(&t, keys[i], (double)i);
    }

    btree_free(&t);
}

static void test_reverse_order(void) {
    printf("\n--- test_reverse_order (вставка в убывающем порядке) ---\n");
    BTree t = {NULL};

    const char *keys[] = {"zz","yy","xx","ww","vv","uu","tt","ss","rr","qq"};
    int n = 10;
    for (int i = 0; i < n; i++) btree_insert(&t, keys[i], (double)i);
    for (int i = 0; i < n; i++) check_found(&t, keys[i], (double)i);

    btree_free(&t);
}

static void test_lexicographic_order(void) {
    printf("\n--- test_lexicographic_order ---\n");
    BTree t = {NULL};

    btree_insert(&t, "ba",  1.0);
    btree_insert(&t, "aab", 2.0);
    btree_insert(&t, "az",  3.0);

    check_found(&t, "ba",  1.0);
    check_found(&t, "aab", 2.0);
    check_found(&t, "az",  3.0);

    btree_free(&t);
}

/* ------------------------------------------------------------------ */

int main(void) {
    test_basic_insert_search();
    test_update_existing();
    test_split_root();
    test_delete_leaf();
    test_delete_nonexistent();
    test_large_tree();
    test_reverse_order();
    test_lexicographic_order();

    printf("\nВсе тесты прошли успешно!\n");
    return 0;
}
