#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define N        2
#define MAX_KEYS (2 * N)
#define OVF_KEYS (2 * N + 1)
#define MAX_CH   (OVF_KEYS + 1)
#define KEY_LEN  7

typedef struct BNode {
    int    n_keys;
    char   keys[OVF_KEYS][KEY_LEN];
    double vals[OVF_KEYS];
    struct BNode *ch[MAX_CH];
} BNode;

typedef struct { BNode *root; } BTree;

static BNode *new_node(void) {
    return calloc(1, sizeof(BNode));
}

static int is_leaf(BNode *nd) {
    return nd->ch[0] == NULL;
}

static int lower_bound(BNode *nd, const char *key) {
    int i = 0;
    while (i < nd->n_keys && strcmp(nd->keys[i], key) < 0)
        i++;
    return i;
}

static void node_insert_at(BNode *nd, int pos,
                           const char *key, double val, BNode *right)
{
    for (int i = nd->n_keys; i > pos; i--) {
        strcpy(nd->keys[i], nd->keys[i - 1]);
        nd->vals[i]   = nd->vals[i - 1];
        nd->ch[i + 1] = nd->ch[i];
    }
    strcpy(nd->keys[pos], key);
    nd->vals[pos]   = val;
    nd->ch[pos + 1] = right;
    nd->n_keys++;
}

static void node_remove_at(BNode *nd, int pos) {
    for (int i = pos; i < nd->n_keys - 1; i++) {
        strcpy(nd->keys[i], nd->keys[i + 1]);
        nd->vals[i]   = nd->vals[i + 1];
        nd->ch[i + 1] = nd->ch[i + 2];
    }
    nd->n_keys--;
}

static BNode *search(BNode *nd, const char *key, int *idx) {
    if (!nd) return NULL;
    int i = lower_bound(nd, key);
    if (i < nd->n_keys && strcmp(nd->keys[i], key) == 0) {
        *idx = i;
        return nd;
    }
    if (is_leaf(nd)) return NULL;
    return search(nd->ch[i], key, idx);
}

int btree_search(BNode *root, const char *key, double *out) {
    int i;
    BNode *nd = search(root, key, &i);
    if (!nd) return 0;
    *out = nd->vals[i];
    return 1;
}

static int insert_rec(BNode *nd, const char *key, double val,
                      char *up_key, double *up_val, BNode **up_right)
{
    int pos = lower_bound(nd, key);

    if (pos < nd->n_keys && strcmp(nd->keys[pos], key) == 0) {
        nd->vals[pos] = val;
        return 0;
    }

    if (is_leaf(nd)) {
        node_insert_at(nd, pos, key, val, NULL);
    } else {
        char   ck[KEY_LEN];
        double cv;
        BNode *cr = NULL;
        if (insert_rec(nd->ch[pos], key, val, ck, &cv, &cr))
            node_insert_at(nd, pos, ck, cv, cr);
    }

    if (nd->n_keys == OVF_KEYS) {
        int    med   = N;
        BNode *right = new_node();

        right->n_keys = OVF_KEYS - med - 1;
        for (int i = 0; i < right->n_keys; i++) {
            strcpy(right->keys[i], nd->keys[med + 1 + i]);
            right->vals[i] = nd->vals[med + 1 + i];
            right->ch[i]   = nd->ch[med + 1 + i];
        }
        right->ch[right->n_keys] = nd->ch[OVF_KEYS];

        strcpy(up_key, nd->keys[med]);
        *up_val   = nd->vals[med];
        *up_right = right;

        nd->n_keys = med;
        return 1;
    }
    return 0;
}

void btree_insert(BTree *t, const char *key, double val) {
    if (!t->root) {
        t->root = new_node();
        strcpy(t->root->keys[0], key);
        t->root->vals[0] = val;
        t->root->n_keys  = 1;
        return;
    }

    char   uk[KEY_LEN];
    double uv;
    BNode *ur = NULL;

    if (insert_rec(t->root, key, val, uk, &uv, &ur)) {
        BNode *new_root = new_node();
        strcpy(new_root->keys[0], uk);
        new_root->vals[0] = uv;
        new_root->ch[0]   = t->root;
        new_root->ch[1]   = ur;
        new_root->n_keys  = 1;
        t->root = new_root;
    }
}

static void tree_min(BNode *nd, char *key, double *val) {
    while (!is_leaf(nd)) nd = nd->ch[0];
    strcpy(key, nd->keys[0]);
    *val = nd->vals[0];
}

static int fix_child(BNode *nd, int idx) {
    BNode *child = nd->ch[idx];
    BNode *left  = (idx > 0)          ? nd->ch[idx - 1] : NULL;
    BNode *right = (idx < nd->n_keys) ? nd->ch[idx + 1] : NULL;

    if (left && left->n_keys > N - 1) {
        for (int i = child->n_keys; i > 0; i--) {
            strcpy(child->keys[i], child->keys[i - 1]);
            child->vals[i]   = child->vals[i - 1];
            child->ch[i + 1] = child->ch[i];
        }
        child->ch[1] = child->ch[0];

        strcpy(child->keys[0], nd->keys[idx - 1]);
        child->vals[0] = nd->vals[idx - 1];
        child->ch[0]   = left->ch[left->n_keys];

        strcpy(nd->keys[idx - 1], left->keys[left->n_keys - 1]);
        nd->vals[idx - 1] = left->vals[left->n_keys - 1];

        left->n_keys--;
        child->n_keys++;
        return idx;
    }

    if (right && right->n_keys > N - 1) {
        strcpy(child->keys[child->n_keys], nd->keys[idx]);
        child->vals[child->n_keys]   = nd->vals[idx];
        child->ch[child->n_keys + 1] = right->ch[0];
        child->n_keys++;

        strcpy(nd->keys[idx], right->keys[0]);
        nd->vals[idx] = right->vals[0];

        for (int i = 0; i < right->n_keys - 1; i++) {
            strcpy(right->keys[i], right->keys[i + 1]);
            right->vals[i]  = right->vals[i + 1];
            right->ch[i]    = right->ch[i + 1];
        }
        right->ch[right->n_keys - 1] = right->ch[right->n_keys];
        right->n_keys--;
        return idx;
    }

    if (left) {
        strcpy(left->keys[left->n_keys], nd->keys[idx - 1]);
        left->vals[left->n_keys]   = nd->vals[idx - 1];
        left->ch[left->n_keys + 1] = child->ch[0];
        left->n_keys++;

        for (int i = 0; i < child->n_keys; i++) {
            strcpy(left->keys[left->n_keys], child->keys[i]);
            left->vals[left->n_keys]   = child->vals[i];
            left->ch[left->n_keys + 1] = child->ch[i + 1];
            left->n_keys++;
        }

        for (int i = idx - 1; i < nd->n_keys - 1; i++) {
            strcpy(nd->keys[i], nd->keys[i + 1]);
            nd->vals[i]   = nd->vals[i + 1];
            nd->ch[i + 1] = nd->ch[i + 2];
        }
        nd->n_keys--;
        free(child);
        return idx - 1;
    } else {
        strcpy(child->keys[child->n_keys], nd->keys[idx]);
        child->vals[child->n_keys]   = nd->vals[idx];
        child->ch[child->n_keys + 1] = right->ch[0];
        child->n_keys++;

        for (int i = 0; i < right->n_keys; i++) {
            strcpy(child->keys[child->n_keys], right->keys[i]);
            child->vals[child->n_keys]   = right->vals[i];
            child->ch[child->n_keys + 1] = right->ch[i + 1];
            child->n_keys++;
        }

        for (int i = idx; i < nd->n_keys - 1; i++) {
            strcpy(nd->keys[i], nd->keys[i + 1]);
            nd->vals[i]   = nd->vals[i + 1];
            nd->ch[i + 1] = nd->ch[i + 2];
        }
        nd->n_keys--;
        free(right);
        return idx;
    }
}

static void delete_rec(BNode *nd, const char *key) {
    int pos   = lower_bound(nd, key);
    int found = (pos < nd->n_keys && strcmp(nd->keys[pos], key) == 0);

    if (found) {
        if (is_leaf(nd)) {
            node_remove_at(nd, pos);
        } else {
            char   sk[KEY_LEN];
            double sv;
            tree_min(nd->ch[pos + 1], sk, &sv);
            strcpy(nd->keys[pos], sk);
            nd->vals[pos] = sv;

            if (nd->ch[pos + 1]->n_keys < N)
                pos = fix_child(nd, pos + 1);
            else
                pos = pos + 1;
            delete_rec(nd->ch[pos], sk);
        }
        return;
    }

    if (is_leaf(nd)) return;

    if (nd->ch[pos]->n_keys < N)
        pos = fix_child(nd, pos);

    delete_rec(nd->ch[pos], key);
}

void btree_delete(BTree *t, const char *key) {
    if (!t->root) { printf("Дерево пусто.\n"); return; }

    int idx;
    if (!search(t->root, key, &idx)) {
        printf("Ключ \"%s\" не найден.\n", key);
        return;
    }

    delete_rec(t->root, key);

    if (t->root->n_keys == 0) {
        BNode *old = t->root;
        t->root = old->ch[0];
        free(old);
    }
}

static void fprint_keys(BNode *nd, FILE *f) {
    fprintf(f, "[");
    for (int i = 0; i < nd->n_keys; i++) {
        if (i) fprintf(f, "|");
        fprintf(f, "%s:%.4g", nd->keys[i], nd->vals[i]);
    }
    fprintf(f, "]");
}

static void fprint_rec(BNode *nd, const char *prefix, int last, FILE *f) {
    if (!nd) return;
    fprintf(f, "%s%s", prefix, last ? "\\-- " : "+-- ");
    fprint_keys(nd, f);
    fprintf(f, "\n");

    if (!is_leaf(nd)) {
        char np[512];
        snprintf(np, sizeof(np), "%s%s", prefix, last ? "    " : "|   ");
        for (int i = 0; i <= nd->n_keys; i++)
            fprint_rec(nd->ch[i], np, i == nd->n_keys, f);
    }
}

void btree_fprint(BTree *t, FILE *f) {
    if (!t->root) { fprintf(f, "(дерево пусто)\n"); return; }
    fprint_keys(t->root, f);
    fprintf(f, "\n");
    if (!is_leaf(t->root)) {
        for (int i = 0; i <= t->root->n_keys; i++)
            fprint_rec(t->root->ch[i], "", i == t->root->n_keys, f);
    }
}

void btree_print(BTree *t) { btree_fprint(t, stdout); }

static void free_rec(BNode *nd) {
    if (!nd) return;
    if (!is_leaf(nd))
        for (int i = 0; i <= nd->n_keys; i++)
            free_rec(nd->ch[i]);
    free(nd);
}

void btree_free(BTree *t) { free_rec(t->root); t->root = NULL; }

static void run_cmd(BTree *t, int op, const char *key, double val, FILE *fout) {
    if (fout) {
        if      (op == 1) fprintf(fout, "1 %s %.4g\n", key, val);
        else if (op == 2) fprintf(fout, "2 %s\n", key);
        else if (op == 3) fprintf(fout, "3\n");
        else if (op == 4) fprintf(fout, "4 %s\n", key);
    }

    switch (op) {
        case 1:
            btree_insert(t, key, val);
            printf("Добавлен: %s = %.4g\n", key, val);
            if (fout) fprintf(fout, "Добавлен: %s = %.4g\n", key, val);
            break;
        case 2: {
            int dummy;
            if (!search(t->root, key, &dummy)) {
                printf("Ключ \"%s\" не найден.\n", key);
                if (fout) fprintf(fout, "Ключ \"%s\" не найден.\n", key);
            } else {
                btree_delete(t, key);
                printf("Удалён: %s\n", key);
                if (fout) fprintf(fout, "Удалён: %s\n", key);
            }
            break;
        }
        case 3:
            printf("Дерево:\n");
            btree_print(t);
            if (fout) { fprintf(fout, "Дерево:\n"); btree_fprint(t, fout); }
            break;
        case 4: {
            double v;
            if (btree_search(t->root, key, &v)) {
                printf("Найдено: %s = %.4g\n", key, v);
                if (fout) fprintf(fout, "Найдено: %s = %.4g\n", key, v);
            } else {
                printf("Ключ \"%s\" не найден.\n", key);
                if (fout) fprintf(fout, "Ключ \"%s\" не найден.\n", key);
            }
            break;
        }
        default:
            printf("Неизвестная операция: %d\n", op);
    }
}

void process_file(BTree *t, const char *in_path, const char *out_path) {
    FILE *fin = fopen(in_path, "r");
    if (!fin) { perror(in_path); return; }

    FILE *fout = fopen(out_path, "w");
    if (!fout) { perror(out_path); fclose(fin); return; }

    char line[256];
    while (fgets(line, sizeof(line), fin)) {
        line[strcspn(line, "\n\r")] = '\0';
        if (line[0] == '\0' || line[0] == '#') continue;

        int    op  = 0;
        char   key[KEY_LEN] = {0};
        double val = 0.0;
        sscanf(line, "%d %6s %lf", &op, key, &val);

        printf("[%s]\n", line);
        run_cmd(t, op, key, val, fout);
        printf("\n");
        fprintf(fout, "\n");
    }

    fclose(fin);
    fclose(fout);
    printf("Результаты записаны в: %s\n", out_path);
}

#ifndef UNIT_TEST
int main(void) {
    printf("=================================================\n");
    printf("  Лаб. работа 4, Вариант 19 — B-дерево (n=2)\n");
    printf("=================================================\n");
    printf("  1 <ключ> <знач>  — добавить\n");
    printf("  2 <ключ>         — удалить\n");
    printf("  3                — напечатать дерево\n");
    printf("  4 <ключ>         — найти\n");
    printf("  0                — выход\n");
    printf("  file <вх> <вых>  — выполнить файл команд\n");
    printf("-------------------------------------------------\n\n");

    BTree t = {NULL};
    char  line[256];

    while (1) {
        printf("> ");
        fflush(stdout);
        if (!fgets(line, sizeof(line), stdin)) break;
        line[strcspn(line, "\n\r")] = '\0';
        if (line[0] == '\0') continue;
        if (line[0] == '0')  break;

        if (strncmp(line, "file", 4) == 0) {
            char in_path[128], out_path[128];
            if (sscanf(line + 4, "%127s %127s", in_path, out_path) == 2)
                process_file(&t, in_path, out_path);
            else
                printf("Использование: file <вход> <выход>\n");
            continue;
        }

        int    op  = 0;
        char   key[KEY_LEN] = {0};
        double val = 0.0;
        sscanf(line, "%d %6s %lf", &op, key, &val);
        run_cmd(&t, op, key, val, NULL);
    }

    btree_free(&t);
    return 0;
}
#endif
