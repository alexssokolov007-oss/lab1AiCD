#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <dirent.h>
#include <sys/stat.h>
#endif

// считает количество строк в файле (по кол-ву символов \n)
int count_lines(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return -1;

    int lines = 0;
    int c;
    while ((c = fgetc(f)) != EOF)
        if (c == '\n')
            lines++;

    fclose(f);
    return lines;
}

// возвращает 1, если файл с расширением .h или .cpp
int is_source(const char *name) {
    const char *ext = strrchr(name, '.');
    if (ext == NULL) return 0;
    return strcmp(ext, ".h") == 0 || strcmp(ext, ".cpp") == 0;
}

// рекурсивный обход каталога, суммирует строки в *total
#ifdef _WIN32
int scan_dir(const char *dir, int *total) {
    char pattern[512];
    sprintf(pattern, "%s\\*", dir);

    WIN32_FIND_DATAA fd;
    HANDLE h = FindFirstFileA(pattern, &fd);
    if (h == INVALID_HANDLE_VALUE) return 0;

    int count = 0;
    do {
        if (strcmp(fd.cFileName, ".") == 0 || strcmp(fd.cFileName, "..") == 0)
            continue;

        char full[512];
        sprintf(full, "%s\\%s", dir, fd.cFileName);

        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            count += scan_dir(full, total);
        } else if (is_source(fd.cFileName)) {
            int n = count_lines(full);
            printf("%6d  %s\n", n, full);
            *total += n;
            count++;
        }
    } while (FindNextFileA(h, &fd));

    FindClose(h);
    return count;
}
#else
int scan_dir(const char *dir, int *total) {
    DIR *d = opendir(dir);
    if (!d) return 0;

    struct dirent *entry;
    int count = 0;
    while ((entry = readdir(d)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        char full[512];
        sprintf(full, "%s/%s", dir, entry->d_name);

        struct stat st;
        stat(full, &st);

        if (S_ISDIR(st.st_mode)) {
            count += scan_dir(full, total);
        } else if (is_source(entry->d_name)) {
            int n = count_lines(full);
            printf("%6d  %s\n", n, full);
            *total += n;
            count++;
        }
    }

    closedir(d);
    return count;
}
#endif

#ifndef UNIT_TEST
int main(int argc, char *argv[]) {
#ifdef _WIN32
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
#endif
    const char *dir = ".";
    if (argc > 1)
        dir = argv[1];

    printf("Каталог: %s\n\n", dir);

    int total = 0;
    int count = scan_dir(dir, &total);

    printf("\nФайлов:      %d\n", count);
    printf("Итого строк: %d\n", total);

    return 0;
}
#endif
