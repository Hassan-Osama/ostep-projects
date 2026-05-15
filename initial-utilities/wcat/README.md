# wcat

A simple implementation of the UNIX `cat` utility.

The program reads files specified as command-line arguments and prints their contents to standard output sequentially. It uses standard C library functions including `fopen()`, `fgets()`, and `fclose()`.

### Features
- Supports multiple input files.
- Gracefully handles missing or unreadable files by printing an error message (`wcat: cannot open file`).
- Exits with status `0` on success and `1` on error.
- If no files are provided, it exits successfully without output.
