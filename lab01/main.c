#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node {
    int key;
    struct Node *left;
    struct Node *right;
} Node;

static int g_quiet = 0;

static void clear_line(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
    }
}

static int read_int(const char *prompt, int *out) {
    for (;;) {
        if (prompt && !g_quiet) {
            printf("%s", prompt);
        }
        if (scanf("%d", out) == 1) {
            return 1;
        }
        if (feof(stdin)) {
            return 0;
        }
        clear_line();
        if (!g_quiet) {
            printf("Invalid input. Try again.\n");
        }
    }
}

static Node *create_node(int key) {
    Node *n = (Node *)malloc(sizeof(Node));
    if (!n) {
        fprintf(stderr, "Out of memory.\n");
        exit(1);
    }
    n->key = key;
    n->left = NULL;
    n->right = NULL;
    return n;
}

static Node *insert_node(Node *root, int key) {
    if (!root) {
        return create_node(key);
    }
    if (key < root->key) {
        root->left = insert_node(root->left, key);
    } else if (key > root->key) {
        root->right = insert_node(root->right, key);
    }
    return root;
}

static Node *find_min(Node *root) {
    while (root && root->left) {
        root = root->left;
    }
    return root;
}

static Node *delete_node(Node *root, int key) {
    if (!root) {
        return NULL;
    }
    if (key < root->key) {
        root->left = delete_node(root->left, key);
    } else if (key > root->key) {
        root->right = delete_node(root->right, key);
    } else {
        if (!root->left) {
            Node *right = root->right;
            free(root);
            return right;
        }
        if (!root->right) {
            Node *left = root->left;
            free(root);
            return left;
        }
        Node *min = find_min(root->right);
        root->key = min->key;
        root->right = delete_node(root->right, min->key);
    }
    return root;
}

static void print_tree(const Node *root, int depth) {
    if (!root) {
        return;
    }
    for (int i = 0; i < depth; ++i) {
        printf("  ");
    }
    printf("%d\n", root->key);
    print_tree(root->left, depth + 1);
    print_tree(root->right, depth + 1);
}

static int count_nodes(const Node *root) {
    if (!root) {
        return 0;
    }
    return 1 + count_nodes(root->left) + count_nodes(root->right);
}

static void free_tree(Node *root) {
    if (!root) {
        return;
    }
    free_tree(root->left);
    free_tree(root->right);
    free(root);
}

static void print_menu(void) {
    printf("\nMenu:\n");
    printf("1. Add node\n");
    printf("2. Print tree\n");
    printf("3. Delete node\n");
    printf("4. Variant 19: count nodes\n");
    printf("5. Exit\n");
}

int main(int argc, char **argv) {
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--quiet") == 0) {
            g_quiet = 1;
        }
    }

    Node *root = NULL;
    if (!g_quiet) {
        printf("Lab 01 (variant 19): count nodes in a binary search tree.\n");
    }

    for (;;) {
        int choice = 0;
        if (!g_quiet) {
            print_menu();
        }
        if (!read_int(g_quiet ? NULL : "> ", &choice)) {
            break;
        }
        if (choice == 1) {
            int key = 0;
            if (read_int(g_quiet ? NULL : "Enter key: ", &key)) {
                root = insert_node(root, key);
            }
        } else if (choice == 2) {
            if (!root) {
                printf("Tree is empty.\n");
            } else {
                print_tree(root, 0);
            }
        } else if (choice == 3) {
            int key = 0;
            if (read_int(g_quiet ? NULL : "Enter key to delete: ", &key)) {
                root = delete_node(root, key);
            }
        } else if (choice == 4) {
            printf("Node count: %d\n", count_nodes(root));
        } else if (choice == 5) {
            break;
        } else {
            if (!g_quiet) {
                printf("Unknown option.\n");
            }
        }
    }

    free_tree(root);
    return 0;
}
