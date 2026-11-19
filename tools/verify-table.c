/* verify-table: checks the fixture table against the tracked tests. */
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "table_data.h"

static int count_tests(const char *directory) {
    DIR *dir = opendir(directory);
    if (dir == NULL) {
        return -1;
    }

    int count = 0;
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strncmp(entry->d_name, "test_", 5) == 0 && strstr(entry->d_name, ".py") != NULL) {
            count++;
        }
    }
    closedir(dir);
    return count;
}

static unsigned long table_sum(void) {
    unsigned long sum = 0;
    size_t length = sizeof(table_rows) / sizeof(table_rows[0]);
    for (size_t i = 0; i < length; i++) {
        sum += table_rows[i];
    }
    return sum;
}

static void usage(void) {
    puts("Usage: verify-table [--help] --check");
}

int main(int argc, char **argv) {
    if (argc > 1 && strcmp(argv[1], "--help") == 0) {
        usage();
        return 0;
    }
    if (argc != 2 || strcmp(argv[1], "--check") != 0) {
        usage();
        return 2;
    }

    int tests = count_tests("tests");
    if (tests < 0) {
        fputs("table check: tests directory not found\n", stderr);
        return 1;
    }

    unsigned long sum = table_sum();
    if (row_base_check() != 0) {
        fputs("table check: base record unavailable\n", stderr);
        return 1;
    }

    printf("table check: %d tests tracked; %zu values, sum 0x%lx; table base ok\n",
           tests,
           sizeof(table_rows) / sizeof(table_rows[0]),
           sum);
    return 0;
}
