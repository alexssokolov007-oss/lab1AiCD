#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/*
 * Лабораторная работа 3, Вариант 19
 * Вынести из частных (делений) унарные минусы:
 *   (-a) / b    ->  -(a / b)
 *   a / (-b)    ->  -(a / b)
 *   (-a) / (-b) ->  a / b
 */

/* ---- Дерево выражения ---- */

typedef enum { NUM, VAR, NEG, ADD, SUB, MUL, DIV } NodeType;

typedef struct Node {
    NodeType type;
    int      num;       /* для NUM */
    char     var[32];   /* для VAR */
    struct Node *left;
    struct Node *right;
} Node;

static Node *alloc_node(NodeType type) {
    Node *n = calloc(1, sizeof(Node));
    n->type = type;
    return n;
}

static Node *num_node(int v) {
    Node *n = alloc_node(NUM);
    n->num = v;
    return n;
}

static Node *var_node(const char *name) {
    Node *n = alloc_node(VAR);
    strncpy(n->var, name, 31);
    return n;
}

static Node *neg_node(Node *child) {
    Node *n = alloc_node(NEG);
    n->left = child;
    return n;
}

static Node *binop_node(NodeType t, Node *l, Node *r) {
    Node *n = alloc_node(t);
    n->left  = l;
    n->right = r;
    return n;
}

void free_tree(Node *n) {
    if (!n) return;
    free_tree(n->left);
    free_tree(n->right);
    free(n);
}

/* ---- Парсер (рекурсивный спуск) ----
 *
 * Грамматика:
 *   expr   = term  { ('+' | '-') term }
 *   term   = unary { ('*' | '/') unary }
 *   unary  = '-' unary | primary
 *   primary = '(' expr ')' | number | variable
 */

static const char *g_src;
static int         g_pos;

static void skip_ws(void) {
    while (g_src[g_pos] == ' ' || g_src[g_pos] == '\t') g_pos++;
}

static Node *parse_expr(void);
static Node *parse_term(void);
static Node *parse_unary(void);
static Node *parse_primary(void);

static Node *parse_expr(void) {
    Node *left = parse_term();
    skip_ws();
    while (g_src[g_pos] == '+' || g_src[g_pos] == '-') {
        char op = g_src[g_pos++];
        Node *right = parse_term();
        left = binop_node(op == '+' ? ADD : SUB, left, right);
        skip_ws();
    }
    return left;
}

static Node *parse_term(void) {
    Node *left = parse_unary();
    skip_ws();
    while (g_src[g_pos] == '*' || g_src[g_pos] == '/') {
        char op = g_src[g_pos++];
        Node *right = parse_unary();
        left = binop_node(op == '*' ? MUL : DIV, left, right);
        skip_ws();
    }
    return left;
}

static Node *parse_unary(void) {
    skip_ws();
    if (g_src[g_pos] == '-') {
        g_pos++;
        return neg_node(parse_unary());
    }
    return parse_primary();
}

static Node *parse_primary(void) {
    skip_ws();
    if (g_src[g_pos] == '(') {
        g_pos++;
        Node *inner = parse_expr();
        skip_ws();
        if (g_src[g_pos] == ')') g_pos++;
        return inner;
    }
    if (isdigit((unsigned char)g_src[g_pos])) {
        int v = 0;
        while (isdigit((unsigned char)g_src[g_pos]))
            v = v * 10 + (g_src[g_pos++] - '0');
        return num_node(v);
    }
    if (isalpha((unsigned char)g_src[g_pos])) {
        char name[32] = {0};
        int i = 0;
        while (isalnum((unsigned char)g_src[g_pos]) && i < 31)
            name[i++] = g_src[g_pos++];
        return var_node(name);
    }
    g_pos++;
    return num_node(0);
}

Node *parse(const char *expr) {
    g_src = expr;
    g_pos = 0;
    return parse_expr();
}

/* ---- Преобразование: вынести унарные минусы из делений ----
 *
 * Для каждого узла DEL:
 *   - если числитель или знаменатель — унарный минус, вытащить наружу
 *   - два минуса взаимно уничтожаются
 */
Node *transform(Node *n) {
    if (!n) return NULL;

    /* Сначала рекурсивно обрабатываем поддеревья */
    n->left  = transform(n->left);
    n->right = transform(n->right);

    if (n->type != DIV) return n;

    int negs = 0;

    /* Убираем минус из числителя */
    if (n->left && n->left->type == NEG) {
        Node *child = n->left->left;
        n->left->left = NULL;
        free(n->left);
        n->left = child;
        negs++;
    }

    /* Убираем минус из знаменателя */
    if (n->right && n->right->type == NEG) {
        Node *child = n->right->left;
        n->right->left = NULL;
        free(n->right);
        n->right = child;
        negs++;
    }

    /* Нечётное количество минусов — выносим один наружу */
    if (negs % 2 == 1)
        return neg_node(n);

    return n;
}

/* ---- Вывод выражения (инфиксная нотация) ---- */

static int is_binop(NodeType t) {
    return t == ADD || t == SUB || t == MUL || t == DIV;
}

static int op_prec(NodeType t) {
    return (t == ADD || t == SUB) ? 1 : 2;
}

static void print_node(Node *n, int outer_prec) {
    if (!n) return;
    switch (n->type) {
        case NUM:
            printf("%d", n->num);
            break;
        case VAR:
            printf("%s", n->var);
            break;
        case NEG:
            printf("-");
            /* бинарный оператор под минусом — нужны скобки */
            if (is_binop(n->left->type)) {
                printf("(");
                print_node(n->left, 0);
                printf(")");
            } else {
                print_node(n->left, 3);
            }
            break;
        default: {
            int  p   = op_prec(n->type);
            char op  = n->type == ADD ? '+' : n->type == SUB ? '-' :
                       n->type == MUL ? '*' : '/';
            int  par = p < outer_prec;
            if (par) printf("(");
            print_node(n->left, p);
            printf(" %c ", op);
            /* правый операнд у - и / нужно брать в скобки при том же приоритете */
            print_node(n->right, p + (n->type == SUB || n->type == DIV ? 1 : 0));
            if (par) printf(")");
            break;
        }
    }
}

void print_infix(Node *n) {
    print_node(n, 0);
    printf("\n");
}

/* ---- Вывод дерева (визуальное представление) ---- */

static void node_label(Node *n, char *buf, int size) {
    switch (n->type) {
        case NUM: snprintf(buf, size, "%d", n->num);  break;
        case VAR: snprintf(buf, size, "%s", n->var);  break;
        case NEG: snprintf(buf, size, "NEG(-)");      break;
        case ADD: snprintf(buf, size, "+");            break;
        case SUB: snprintf(buf, size, "-");            break;
        case MUL: snprintf(buf, size, "*");            break;
        case DIV: snprintf(buf, size, "/");            break;
    }
}

/* last = 1 означает "последний дочерний узел" (рисуем \\-- вместо +--) */
static void print_tree_impl(Node *n, const char *prefix, int last) {
    if (!n) return;
    char label[64];
    node_label(n, label, sizeof(label));
    printf("%s%s%s\n", prefix, last ? "\\-- " : "+-- ", label);

    char new_prefix[256];
    snprintf(new_prefix, sizeof(new_prefix), "%s%s", prefix, last ? "    " : "|   ");

    if (n->left || n->right) {
        print_tree_impl(n->left,  new_prefix, n->right == NULL);
        if (n->right)
            print_tree_impl(n->right, new_prefix, 1);
    }
}

void print_tree(Node *n) {
    if (!n) return;
    char label[64];
    node_label(n, label, sizeof(label));
    printf("%s\n", label);
    if (n->left)  print_tree_impl(n->left,  "", n->right == NULL);
    if (n->right) print_tree_impl(n->right, "", 1);
}

/* ---- Запись выражения в строку (для тестирования) ---- */

static int do_sprint(Node *n, int outer_prec, char *buf, int pos, int cap) {
    if (!n || pos >= cap) return pos;

#define PUT(c) do { if (pos < cap - 1) buf[pos++] = (c); } while (0)

    switch (n->type) {
        case NUM:
            if (pos < cap)
                pos += snprintf(buf + pos, (size_t)(cap - pos), "%d", n->num);
            break;
        case VAR:
            if (pos < cap)
                pos += snprintf(buf + pos, (size_t)(cap - pos), "%s", n->var);
            break;
        case NEG:
            PUT('-');
            if (is_binop(n->left->type)) {
                PUT('(');
                pos = do_sprint(n->left, 0, buf, pos, cap);
                PUT(')');
            } else {
                pos = do_sprint(n->left, 3, buf, pos, cap);
            }
            break;
        default: {
            int  p   = op_prec(n->type);
            char op  = n->type == ADD ? '+' : n->type == SUB ? '-' :
                       n->type == MUL ? '*' : '/';
            int  par = p < outer_prec;
            if (par) PUT('(');
            pos = do_sprint(n->left, p, buf, pos, cap);
            PUT(' '); PUT(op); PUT(' ');
            pos = do_sprint(n->right, p + (n->type == SUB || n->type == DIV ? 1 : 0),
                            buf, pos, cap);
            if (par) PUT(')');
            break;
        }
    }

#undef PUT
    return pos;
}

void snprint_infix(Node *n, char *buf, int cap) {
    int pos = do_sprint(n, 0, buf, 0, cap);
    if (pos < cap) buf[pos] = '\0';
}

/* ---- Главная программа ---- */

#ifndef UNIT_TEST
int main(void) {
    printf("=================================================\n");
    printf("  Лаб. работа 3, Вариант 19\n");
    printf("  Вынести из частных унарные минусы\n");
    printf("  (-a) / b    ->  -(a / b)\n");
    printf("  a / (-b)    ->  -(a / b)\n");
    printf("  (-a) / (-b) ->  a / b\n");
    printf("=================================================\n");
    printf("Введите выражение (или 'exit' для выхода):\n\n");

    char line[512];
    while (1) {
        printf("> ");
        fflush(stdout);
        if (!fgets(line, sizeof(line), stdin)) break;
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "exit") == 0 || line[0] == '\0') break;
        if (line[0] == '#') continue;   /* комментарий */

        Node *original = parse(line);

        printf("\n--- Исходное выражение ---\n");
        printf("Дерево:\n");
        print_tree(original);
        printf("Инфикс: ");
        print_infix(original);

        Node *result = transform(original);

        printf("\n--- После преобразования ---\n");
        printf("Дерево:\n");
        print_tree(result);
        printf("Инфикс: ");
        print_infix(result);
        printf("\n");

        free_tree(result);
    }

    return 0;
}
#endif
