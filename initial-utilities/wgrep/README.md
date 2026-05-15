# wgrep

A simple implementation of the UNIX `grep` utility.

The program searches for a specified search term in one or more files and prints any lines that contain a match. If no files are specified, it searches standard input (`stdin`).

### Implementation Details
- Uses `getline()` to handle arbitrarily long lines of text efficiently.
- Uses `strstr()` for case-sensitive string matching.
- Handles file access errors by printing `wgrep: cannot open file` and exiting with status `1`.
- If no command-line arguments are provided, it prints usage instructions and exits with status `1`.
