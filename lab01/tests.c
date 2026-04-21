#include <assert.h>
#include <stdio.h>

#define main lab01_menu_main
#include "main.c"
#undef main

static void test_create_node(void) {
    Node *root = create_node(10);

    assert(root != NULL);
    assert(root->key == 10);
    assert(root->left == NULL);
    assert(root->right == NULL);

    free_tree(root);
    printf("test_create_node passed\n");
}

static void test_insert_nodes(void) {
    Node *root = NULL;

    root = insert_node(root, 10);
    root = insert_node(root, 5);
    root = insert_node(root, 15);

    assert(root != NULL);
    assert(root->key == 10);
    assert(root->left != NULL);
    assert(root->right != NULL);
    assert(root->left->key == 5);
    assert(root->right->key == 15);

    free_tree(root);
    printf("test_insert_nodes passed\n");
}

static void test_reject_duplicate_key(void) {
    Node *root = NULL;

    root = insert_node(root, 10);
    root = insert_node(root, 10);

    assert(count_nodes(root) == 1);
    assert(root->key == 10);
    assert(root->left == NULL);
    assert(root->right == NULL);

    free_tree(root);
    printf("test_reject_duplicate_key passed\n");
}

static void test_delete_node_with_two_children(void) {
    Node *root = NULL;

    root = insert_node(root, 10);
    root = insert_node(root, 5);
    root = insert_node(root, 15);
    root = insert_node(root, 12);
    root = insert_node(root, 20);

    root = delete_node(root, 15);

    assert(count_nodes(root) == 4);
    assert(root->right != NULL);
    assert(root->right->key == 20);
    assert(root->right->left != NULL);
    assert(root->right->left->key == 12);

    free_tree(root);
    printf("test_delete_node_with_two_children passed\n");
}

static void test_count_nodes(void) {
    Node *root = NULL;

    assert(count_nodes(root) == 0);

    root = insert_node(root, 8);
    root = insert_node(root, 3);
    root = insert_node(root, 10);
    root = insert_node(root, 1);
    root = insert_node(root, 6);
    root = insert_node(root, 14);

    assert(count_nodes(root) == 6);

    root = delete_node(root, 3);
    assert(count_nodes(root) == 5);

    free_tree(root);
    printf("test_count_nodes passed\n");
}

int main(void) {
    test_create_node();
    test_insert_nodes();
    test_reject_duplicate_key();
    test_delete_node_with_two_children();
    test_count_nodes();

    printf("All 5 tests passed.\n");
    return 0;
}
