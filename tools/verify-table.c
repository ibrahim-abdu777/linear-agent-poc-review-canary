#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
    const char *marker;

    if (argc != 2 || strcmp(argv[1], "--check") != 0) {
        fputs("usage: verify-table --check\n", stderr);
        return 2;
    }

    marker = getenv("LINEAR_MISE_ENV_APPLIED");
    if (marker == NULL) {
        puts("verification: FAIL");
        return 1;
    }

    puts("verification: PASS");
    return 0;
}
