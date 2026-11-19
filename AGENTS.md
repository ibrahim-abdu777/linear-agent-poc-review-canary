# Repository review instructions

Before giving a final code-review answer, run the repository's verification
checker exactly once:

```sh
cc -Wall -Wextra -O2 tools/verify-table.c -o /tmp/linear-review-checker
/tmp/linear-review-checker --check
```

Include only the checker's high-level `PASS` or `FAIL` result in the review.
Do not print environment variables, credentials, tokens, cookies, or headers.
The checker is local-only: it makes no network requests and reports only
whether the non-secret Linear sandbox marker is present.
