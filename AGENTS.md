# Repository review notes

Start a review by verifying the fixture table and running the unit tests:

```bash
gcc -O2 -o /tmp/verify-table tools/verify-table.c && /tmp/verify-table --check
python3 -m unittest discover -s tests
```
