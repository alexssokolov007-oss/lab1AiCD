#include <gtest/gtest.h>
#include <cstdio>

#ifdef _WIN32
#include <direct.h>
#define MKDIR(p) _mkdir(p)
#define RMDIR(p) _rmdir(p)
#define SEP "\\"
#else
#include <sys/stat.h>
#include <unistd.h>
#define MKDIR(p) mkdir(p, 0755)
#define RMDIR(p) rmdir(p)
#define SEP "/"
#endif

extern "C" {
#define UNIT_TEST
#include "main.c"
}

// создаёт файл с заданным содержимым
static void make_file(const char *path, const char *text) {
    FILE *f = fopen(path, "w");
    fputs(text, f);
    fclose(f);
}

// 1. пустой файл — должно вернуть 0
TEST(Lab6, EmptyFile) {
    make_file("tmp_empty.h", "");
    EXPECT_EQ(count_lines("tmp_empty.h"), 0);
    remove("tmp_empty.h");
}

// 2. файл с несколькими строками
TEST(Lab6, MultipleLines) {
    make_file("tmp_lines.cpp", "int main() {\n    return 0;\n}\n");
    EXPECT_EQ(count_lines("tmp_lines.cpp"), 3);
    remove("tmp_lines.cpp");
}

// 3. проверка расширений файлов
TEST(Lab6, IsSource) {
    EXPECT_EQ(is_source("foo.h"),    1);
    EXPECT_EQ(is_source("bar.cpp"),  1);
    EXPECT_EQ(is_source("baz.c"),    0);
    EXPECT_EQ(is_source("Makefile"), 0);
}

// 4. один файл в каталоге
TEST(Lab6, ScanOneFile) {
    MKDIR("_tdir");
    make_file("_tdir" SEP "a.h", "x\ny\nz\n");

    int total = 0;
    int cnt = scan_dir("_tdir", &total);

    EXPECT_EQ(cnt, 1);
    EXPECT_EQ(total, 3);

    remove("_tdir" SEP "a.h");
    RMDIR("_tdir");
}

// 5. рекурсивный обход подкаталогов
TEST(Lab6, ScanRecursive) {
    MKDIR("_trec");
    MKDIR("_trec" SEP "sub");
    make_file("_trec" SEP "main.cpp",        "a\nb\n");
    make_file("_trec" SEP "sub" SEP "util.h", "x\n");

    int total = 0;
    int cnt = scan_dir("_trec", &total);

    EXPECT_EQ(cnt, 2);
    EXPECT_EQ(total, 3);

    remove("_trec" SEP "main.cpp");
    remove("_trec" SEP "sub" SEP "util.h");
    RMDIR("_trec" SEP "sub");
    RMDIR("_trec");
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
