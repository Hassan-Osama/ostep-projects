# Unix Shell Implementation (wish)

This directory contains my implementation of `wish` (Wisconsin Shell), a simple Unix shell developed as part of the OSTEP projects.

## Features

- **Interactive and Batch Modes**: Supports both manual command entry with a `wish> ` prompt and automated execution from script files.
- **Built-in Commands**:
  - `exit`: Gracefully terminates the shell.
  - `cd`: Changes the current working directory using `chdir()`.
  - `path`: Configures the search path for executables (overwrites existing path).
- **Path Search**: Automatically searches for executables across user-defined paths (defaults to `/bin`).
- **Redirection**: Redirects both standard output and standard error to a file using the `>` operator (e.g., `ls > output.txt`).
- **Parallel Commands**: Executes multiple commands simultaneously using the `&` operator, waiting for all to complete before returning control to the user.
- **Robust Error Handling**: Prints a single consistent error message `An error has occurred` to stderr for all syntax and runtime errors.

## Implementation Details

The shell is implemented in `wish.c` and uses standard Unix system calls:
- `fork()`, `execv()`, and `waitpid()` for process management.
- `getline()` for robust input reading.
- `strsep()` for efficient string parsing.
- `open()`, `dup2()`, and `close()` for output redirection.

## Building and Running

To compile the shell:
```bash
gcc wish.c -o wish -Wall -Werror
```

To run interactively:
```bash
./wish
```

To run a batch file:
```bash
./wish tests/1.in
```

## Testing

The implementation can be verified using the provided test suite:
```bash
./test-wish.sh
```
