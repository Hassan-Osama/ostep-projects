# Parallel Zip (pzip)

This directory contains a high-performance parallel implementation of a Run-Length Encoding (RLE) compression tool, `pzip`. It leverages multi-core processors to significantly speed up the compression of large files.

## Features

- **Parallel Compression**: Divides the input file into multiple chunks and processes them in parallel using a pool of worker threads.
- **Dynamic Thread Scaling**: Automatically detects the number of available CPU cores using `sysconf(_SC_NPROCESSORS_ONLN)` to optimize thread count.
- **Memory-Mapped I/O**: Uses `mmap()` to map input files into the process's address space, enabling fast and efficient access to data.
- **Producer-Consumer Model**: Employs a synchronized `TaskQueue` to manage work distribution between the master thread and worker threads.
- **Ordered Output & Merging**: Carefully merges RLE runs that span across chunk boundaries and ensures that the compressed output is written to `stdout` in the correct sequence.
- **Synchronization**: Uses POSIX mutexes and condition variables to manage access to shared data structures and signal task completion.
- **Built-in Benchmarking**: Includes a nanosecond-resolution timer to report execution time and throughput (MB/s) to `stderr`.

## Implementation Details

The implementation in `pzip.c` follows these steps:
1. **Mapping**: The input file is opened and memory-mapped.
2. **Task Distribution**: The file is partitioned into small, fixed-size chunks (1MB). This allows for dynamic load balancing, as faster threads can pick up more tasks while slower threads finish their work.
3. **Worker Processing**: Worker threads pop tasks from the queue, perform RLE compression on their assigned chunk, and store the resulting `RLERun` arrays in a shared `results_array`.
4. **Result Collection**: The main thread waits for each chunk to be completed in order. It iterates through the compressed runs, merging the last run of one chunk with the first run of the next if the characters match.
5. **Output**: The merged RLE runs (4-byte count followed by 1-byte character) are written to `stdout`.

## Building and Running

### Compilation
Compile the project using `gcc` with the optimizer enabled for maximum performance:
```bash
gcc pzip.c -o pzip -Wall -Werror -pthread -O3
```

### Running pzip
Run `pzip` by passing the input file path. Redirect the output to a file if you wish to save the compressed data:
```bash
./pzip input_file > output_file.z
```

## Benchmarking

The tool automatically outputs performance metrics to `stderr` upon completion. This includes the number of cores detected, the total time taken for compression, and the processing throughput:

```text
[system] Detected 8 CPU cores
[benchmark] Time: 0.1852 s
[benchmark] Throughput: 539.85 MB/s
```

## Performance Optimization: Dynamic Load Balancing

Initially, the tool divided the input into exactly `N` chunks (where `N` is the number of CPU cores). This approach was susceptible to "stragglers"—slow threads that delay the entire process.

By refactoring the code to use **1MB fixed-size chunks**, we achieved significant performance gains. This allows for a more granular distribution of work, ensuring all cores stay busy until the entire file is processed.

### Before vs. After (100MB Zero-filled file)

| Optimization Step | Throughput (MB/s) | Note |
| :--- | :--- | :--- |
| **Initial (One chunk per core)** | ~540 MB/s | Bottlenecked by slowest thread/chunk. |
| **Optimized (1MB Fixed Chunks)** | **~4433 MB/s** | Dynamic balancing keeps all cores saturated. |

## Performance Note
Using the `-O3` flag during compilation is highly recommended as it allows the compiler to optimize the core compression loop, significantly improving throughput on large datasets.
