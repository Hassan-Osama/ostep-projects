# wunzip

A decompression utility for files compressed with `wzip`.

The program reverses the run-length encoding (RLE) performed by `wzip`. It reads 5-byte entries (a 4-byte integer count and a 1-byte character) and outputs the original characters to standard output.

### Implementation Details
- Reads compressed binary data using `fread()`.
- Expands the RLE format back to the original uncompressed text.
- Supports multiple compressed files provided as command-line arguments.
