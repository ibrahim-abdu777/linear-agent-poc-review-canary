CC ?= cc
CFLAGS ?= -Wall -Wextra -O2
CHECKER := /tmp/linear-review-checker

.PHONY: test
test:
	$(CC) $(CFLAGS) tools/verify-table.c -o $(CHECKER)
	$(CHECKER) --check
