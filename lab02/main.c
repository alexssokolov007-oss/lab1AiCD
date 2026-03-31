#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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

static void print_array(const char *label, const int *a, int n) {
    printf("%s:", label);
    for (int i = 0; i < n; ++i) {
        printf(" %d", a[i]);
    }
    printf("\n");
}

static void bubble_sort(int *a, int n) {
    for (int i = 0; i < n - 1; ++i) {
        int swapped = 0;
        for (int j = 0; j < n - 1 - i; ++j) {
            if (a[j] > a[j + 1]) {
                int tmp = a[j];
                a[j] = a[j + 1];
                a[j + 1] = tmp;
                swapped = 1;
            }
        }
        if (!swapped) {
            break;
        }
    }
}

static int binary_search(const int *a, int n, int key) {
    int l = 0;
    int r = n - 1;
    while (l <= r) {
        int m = l + (r - l) / 2;
        if (a[m] == key) {
            return m;
        }
        if (a[m] < key) {
            l = m + 1;
        } else {
            r = m - 1;
        }
    }
    return -1;
}

static int cmp_ints(const void *a, const void *b) {
    int x = *(const int *)a;
    int y = *(const int *)b;
    return (x > y) - (x < y);
}

static void shuffle(int *a, int n) {
    for (int i = n - 1; i > 0; --i) {
        int j = rand() % (i + 1);
        int tmp = a[i];
        a[i] = a[j];
        a[j] = tmp;
    }
}

static void run_search(const int *a, int n) {
    int q = 0;
    if (!read_int(g_quiet ? NULL : "Number of search keys (0 to skip): ", &q)) {
        return;
    }
    for (int i = 0; i < q; ++i) {
        int key = 0;
        if (!read_int(g_quiet ? NULL : "Key: ", &key)) {
            return;
        }
        int idx = binary_search(a, n, key);
        if (idx >= 0) {
            printf("Found at index %d (value %d).\n", idx, a[idx]);
        } else {
            printf("Not found.\n");
        }
    }
}

static void run_case(const char *title, int *a, int n) {
    printf("\nCase: %s\n", title);
    print_array("Original", a, n);
    bubble_sort(a, n);
    print_array("Sorted", a, n);
    run_search(a, n);
}

int main(int argc, char **argv) {
    unsigned int seed = 0;
    int seed_set = 0;

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--quiet") == 0) {
            g_quiet = 1;
        } else if (strcmp(argv[i], "--seed") == 0 && i + 1 < argc) {
            seed = (unsigned int)strtoul(argv[i + 1], NULL, 10);
            seed_set = 1;
            i++;
        }
    }

    const int min_n = 11;
    if (!g_quiet) {
        printf("Lab 02 (variant 19).\n");
        printf("Sort method: bubble sort.\n");
        printf("Table structure: 1 (integer key, keys+data together).\n");
        printf("Minimum number of elements: %d.\n\n", min_n);
    }

    int n = 0;
    if (!read_int(g_quiet ? NULL : "Enter number of elements: ", &n)) {
        return 0;
    }
    if (n < min_n) {
        printf("n must be >= %d.\n", min_n);
        return 0;
    }

    int *base = (int *)malloc((size_t)n * sizeof(int));
    if (!base) {
        fprintf(stderr, "Out of memory.\n");
        return 1;
    }

    if (!g_quiet) {
        printf("Enter %d integer keys:\n", n);
    }
    for (int i = 0; i < n; ++i) {
        if (!read_int(g_quiet ? NULL : "", &base[i])) {
            free(base);
            return 0;
        }
    }

    int *case1 = (int *)malloc((size_t)n * sizeof(int));
    int *case2 = (int *)malloc((size_t)n * sizeof(int));
    int *case3 = (int *)malloc((size_t)n * sizeof(int));
    if (!case1 || !case2 || !case3) {
        fprintf(stderr, "Out of memory.\n");
        free(base);
        free(case1);
        free(case2);
        free(case3);
        return 1;
    }

    for (int i = 0; i < n; ++i) {
        case1[i] = base[i];
        case2[i] = base[i];
        case3[i] = base[i];
    }

    qsort(case1, n, sizeof(int), cmp_ints);
    for (int i = 0; i < n; ++i) {
        case2[i] = case1[n - 1 - i];
    }

    if (seed_set) {
        srand(seed);
    } else {
        srand((unsigned int)time(NULL));
    }
    shuffle(case3, n);

    run_case("Already sorted", case1, n);
    run_case("Reverse order", case2, n);
    run_case("Random order", case3, n);

    free(base);
    free(case1);
    free(case2);
    free(case3);
    return 0;
}
