# Concurrent Web Server

This directory contains a multi-threaded web server (`wserver`) implemented as part of the OSTEP concurrency projects. The server is designed to handle multiple simultaneous HTTP requests using a thread pool architecture.

## Features

- **Multi-threaded Architecture**: Utilizes a fixed-size pool of worker threads created at startup to avoid the overhead of thread creation per request.
- **Master-Worker Model**:
    - **Master Thread**: Responsible for accepting new network connections and placing the resulting file descriptors into a shared bounded buffer.
    - **Worker Threads**: Continuously pull connection descriptors from the buffer and handle the HTTP requests (static or dynamic).
- **Synchronized Bounded Buffer**: Implements a thread-safe circular buffer using mutexes and condition variables (`pthread_mutex_t`, `pthread_cond_t`) to manage the producer-consumer relationship between the master and worker threads.
- **Content Support**:
    - **Static Content**: Serves files (HTML, images, text) using memory mapping (`mmap`).
    - **Dynamic Content**: Executes CGI programs (like `spin.cgi`) by forking a new process and redirecting output to the client socket.
- **Configurable Scheduling**: Supports FIFO (First-In-First-Out) scheduling for processing buffered requests.

## Project Structure

- `wserver.c`: Main server logic, thread pool management, and command-line parsing.
- `request.c`: Logic for parsing HTTP requests and serving static/dynamic content.
- `buffer.c` / `buffer.h`: Implementation of the synchronized bounded buffer.
- `io_helper.c` / `io_helper.h`: Robust wrappers for system calls.
- `wclient.c`: A simple HTTP client for testing.
- `spin.c`: A sample CGI program that "spins" to simulate a long-running task.

## Building and Running

### Compilation
Use the provided Makefile to build the server, client, and CGI program:
```bash
cd src
make
```

### Running the Server
The server accepts several command-line arguments:
```bash
./wserver [-d basedir] [-p port] [-t threads] [-b buffers] [-s schedalg]
```
- `-d`: Root directory for file serving (default: `.`).
- `-p`: Port to listen on (default: `10000`).
- `-t`: Number of worker threads (default: `1`).
- `-b`: Bounded buffer size (default: `1`).
- `-s`: Scheduling algorithm, either `FIFO` or `SFF` (Note: current implementation uses FIFO).

Example:
```bash
./wserver -p 8080 -t 4 -b 10
```

### Running the Client
You can test the server using `wclient` or any web browser:
```bash
./wclient localhost 8080 /index.html
```

## Implementation Highlights

- **Synchronization**: The bounded buffer ensures that the master thread blocks when the buffer is full and worker threads block when the buffer is empty, preventing busy-waiting.
