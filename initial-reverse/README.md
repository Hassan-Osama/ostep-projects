# Reverse Utility

A simple command-line utility implemented in C that reverses the order of lines in a file or from standard input.

## Implementation Details

The program is designed to be efficient and robust, using the following key techniques:

- **Linked List Data Structure**: Uses a singly-linked list to store each line as it is read from the input source. By prepending each new node to the head of the list, the order of the lines is naturally reversed during traversal and printing.
- **Dynamic Line Handling**: Employs `getline()` to read lines of arbitrary length, ensuring the program does not fail on exceptionally long lines.
- **Robust Error Handling**:
    - Validates command-line arguments and provides clear usage instructions.
    - Gracefully handles file access errors with descriptive messages.
    - Implements proper memory management by handling `malloc` failures and freeing all allocated memory before exit.
- **File Safety**: Uses the `stat()` system call to check the inode and device of input and output files. This prevents the program from accidentally overwriting the input file if both paths refer to the same physical file.

## Usage

The program can be used in three ways:

1.  **Standard Input/Output**: Reads from `stdin` and writes to `stdout`.
    ```sh
    ./reverse
    ```
2.  **Single File**: Reads from the specified input file and writes to `stdout`.
    ```sh
    ./reverse input.txt
    ```
3.  **Two Files**: Reads from the specified input file and writes to the specified output file.
    ```sh
    ./reverse input.txt output.txt
    ```
