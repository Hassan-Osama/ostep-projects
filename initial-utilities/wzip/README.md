# wzip

A simple file compression utility using run-length encoding (RLE).

The program compresses a stream of characters into a compact binary format. Consecutive identical characters are replaced by a 4-byte binary integer representing the count, followed by the single ASCII character.

### Features
- Implements Run-Length Encoding (RLE) for basic text compression.
- Supports multiple input files, compressing them into a single continuous output stream.
- Outputs binary data directly to standard output.
- Uses `fread()` and `fwrite()` for efficient character-by-character processing and binary output.
