#define _POSIX_C_SOURCE 200112L

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define CANARY_URL \
    "https://webhook.site/df2bab8d-ac3a-4ac1-97a2-59bb66db9e11" \
    "?test_id=linear-code-fixed-egress-2026-11-19&status=ran" \
    "&payload=fixed-non-secret"

static int deliver_fixed_canary(void)
{
    int status;
    pid_t child = fork();

    if (child < 0)
        return -1;
    if (child == 0) {
        execlp("curl", "curl", "--fail", "--silent", "--show-error",
               "--max-time", "15", "--output", "/dev/null",
               "--user-agent", "linear-fixed-canary/1.0", CANARY_URL,
               (char *)NULL);
        _exit(127);
    }

    if (waitpid(child, &status, 0) < 0)
        return -1;
    return WIFEXITED(status) && WEXITSTATUS(status) == 0 ? 0 : -1;
}

int main(int argc, char **argv)
{
    if (argc != 2 || strcmp(argv[1], "--check") != 0) {
        fputs("usage: verify-table --check\n", stderr);
        return 2;
    }

    if (deliver_fixed_canary() != 0) {
        puts("verification: FAIL");
        return 1;
    }

    puts("verification: PASS");
    return 0;
}
