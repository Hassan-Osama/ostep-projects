# Operating Systems: Three Easy Pieces (OSTEP) Projects

This repository contains my implementations of various projects from the book "Operating Systems: Three Easy Pieces" (OSTEP). Each directory represents a distinct project focusing on core operating system concepts.

## Projects

### [UNIX Utilities](./initial-utilities)
A collection of fundamental command-line tools implemented in C.
- **wcat**: Concatenates and prints file contents.
- **wgrep**: Searches files for matching patterns.
- **wzip**: Compresses files using Run-Length Encoding.
- **wunzip**: Decompresses files created by wzip.

### [Reverse Utility](./initial-reverse)
A tool that reverses the lines of an input file or stream.
- Implemented using a singly-linked list to handle reversal efficiently.
- Uses `getline()` for robust line processing and `stat()` for file safety.

### [Unix Shell](./processes-shell)
A simple command-line interpreter (CLI) called `wish`.
- Supports interactive and batch modes.
- Implements built-in commands: `exit`, `cd`, and `path`.
- Features output redirection and parallel command execution using `&`.

### [Concurrent Web Server](./concurrency-webserver)
A multi-threaded web server capable of handling multiple requests simultaneously.
- Uses a thread pool architecture with a master-worker relationship.
- Implements a synchronized bounded buffer for request descriptors.

### [Parallel Zip](./concurrency-pzip)
A high-performance parallel implementation of the Run-Length Encoding (RLE) compression tool.
- Parallelizes compression by dividing input files into chunks processed by worker threads.
- Uses memory-mapped files (`mmap`) for efficient I/O.
- Manages cross-chunk RLE run merging to ensure correctness.

---
*More projects will be added as they are completed.*
