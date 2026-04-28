#include <assert.h>
#include <stdio.h>
#include <string.h>

/* Вперёд-объявления из main.c */
typedef struct Node Node;

Node *parse(const char *expr);
Node *transform(Node *n);
void  snprint_infix(Node *n, char *buf, int cap);
void  free_tree(Node *n);

/* Вспомогательная функция: парсит → преобразует → сравнивает строку */
static void check(const char *input, const char *expected) {
    char buf[512];
    Node *tree   = parse(input);
    Node *result = transform(tree);
    snprint_infix(result, buf, sizeof(buf));
    free_tree(result);

    if (strcmp(buf, expected) != 0) {
        printf("FAIL: \"%s\"\n", input);
        printf("  ожидалось: \"%s\"\n", expected);
        printf("  получено:  \"%s\"\n", buf);
        assert(0);
    }
    printf("OK: \"%s\"  ->  \"%s\"\n", input, buf);
}

/* ---- Тесты ---- */

static void test_neg_in_numerator(void) {
    check("(-a) / b", "-(a / b)");
}

static void test_neg_in_denominator(void) {
    check("a / (-b)", "-(a / b)");
}

static void test_neg_both_cancel(void) {
    check("(-a) / (-b)", "a / b");
}

static void test_neg_num_constants(void) {
    check("(-5) / 3",   "-(5 / 3)");
    check("4 / (-2)",   "-(4 / 2)");
    check("(-3) / (-7)", "3 / 7");
}

static void test_no_change_without_neg(void) {
    check("a / b",   "a / b");
    check("a + b",   "a + b");
    check("a * b",   "a * b");
}

static void test_neg_in_sum_context(void) {
    /* Деление с унарным минусом стоит внутри суммы */
    check("(-a) / b + c", "-(a / b) + c");
    check("(-a) / (-b) + d", "a / b + d");
}

static void test_neg_with_multiplication(void) {
    /* Деление стоит внутри произведения */
    check("a / (-b) * c", "-(a / b) * c");
}

static void test_compound_numerator(void) {
    /* Числитель — выражение под унарным минусом */
    check("(-(a + b)) / c", "-((a + b) / c)");
}

static void test_neg_not_in_division_operand(void) {
    /* Унарный минус внутри произведения — не вытаскивается,
       так как непосредственным операндом деления он не является */
    check("(-a) * b / ((-c) * d)", "-a * b / (-c * d)");
    check("a / ((-b) * c)",        "a / (-b * c)");
}

static void test_chained_divisions(void) {
    /* Цепочка делений: ((-a) / b) / c */
    check("(-a) / b / c", "-(a / b / c)");
    /* Два минуса в цепочке: ((-a) / (-b)) / c */
    check("(-a) / (-b) / c", "a / b / c");
}

int main(void) {
    test_neg_in_numerator();
    test_neg_in_denominator();
    test_neg_both_cancel();
    test_neg_num_constants();
    test_no_change_without_neg();
    test_neg_in_sum_context();
    test_neg_with_multiplication();
    test_compound_numerator();
    test_neg_not_in_division_operand();
    test_chained_divisions();

    printf("\nВсе тесты прошли успешно!\n");
    return 0;
}
