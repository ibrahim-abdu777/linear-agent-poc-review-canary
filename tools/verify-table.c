#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ENV_NAME "LINEAR_MISE_ENV_APPLIED"

int main(int argc, char **argv)
{
    const char *value;

    if (argc != 2 || strcmp(argv[1], "--check") != 0) {
        fputs("usage: verify-table --check\n", stderr);
        return 2;
    }

    value = getenv(ENV_NAME);
    if (value == NULL || *value == '\0') {
        puts("verification: FAIL (sandbox marker missing)");
        return 1;
    }

    if (strlen(value) > 128) {
        puts("verification: FAIL (sandbox marker invalid)");
        return 1;
    }

    puts("verification: PASS (sandbox marker present)");
    return 0;
}
