# Отчет по лабораторной работе

**ФИО:** Соколов Александр Сергеевич

**Вариант:** 19

## Формулировка задания

Реализовать двоичное дерево поиска с возможностью:
- добавления узлов;
- удаления узлов;
- вывода дерева на экран;
- вычисления функции по варианту.

Для варианта 19 требуется определить число вершин двоичного дерева.

## Описание выполненной работы

### Структура проекта

Проект состоит из следующих файлов:

#### 1. `main.c` - Основная программа и реализация дерева

Содержит структуру `Node`, функции для работы с двоичным деревом поиска и интерактивное меню.

Структура узла:

```c
typedef struct Node {
    int key;
    struct Node *left;
    struct Node *right;
} Node;
```

**Основные функции:**
- `create_node()` — создание нового узла
- `insert_node()` — вставка ключа по правилу двоичного дерева поиска
- `find_min()` — поиск минимального узла в поддереве
- `delete_node()` — удаление узла из дерева
- `print_tree()` — вывод дерева с отступами по глубине
- `count_nodes()` — подсчет числа вершин дерева
- `free_tree()` — освобождение памяти дерева
- `print_menu()` — вывод интерактивного меню

Реализация функции задачи:

```c
static int count_nodes(const Node *root) {
    if (!root) {
        return 0;
    }
    return 1 + count_nodes(root->left) + count_nodes(root->right);
}
```

Реализация удаления узла:

```c
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
```

#### 2. `tests.c` - Тесты

Тесты подключают `main.c` через переопределение `main` и проверяют функции дерева через `assert`.

Тест-кейсы:
- `test_create_node` — создание узла дерева
- `test_insert_nodes` — вставка левого и правого потомка
- `test_reject_duplicate_key` — отказ от добавления дублирующегося ключа
- `test_delete_node_with_two_children` — удаление узла с двумя потомками
- `test_count_nodes` — подсчет числа вершин до и после удаления

#### 3. `Makefile` - Сборка проекта

Содержит цели для сборки основной программы и запуска тестов.

## Как запустить

**Запуск основной программы:**

```bash
make run
```

**Запуск тестов:**

```bash
make run_tests
```
