#ifndef COOKIE_FILES_H
#define COOKIE_FILES_H

#include <stdint.h>
#include <stddef.h>

/*
 * utilities for reading/writing to files
 */

/*
 * copy the byte representation of `data` into `out` (`data` has `size` bytes)
 * `cpy_bytes` ensures that the output is in big-endian
 * return the number of bytes written
 */
size_t cpy_bytes(const char *data, size_t size, char *out);

/*
 * writes a chunk of data, which can be identified by a 4-byte tag
 * return the number of bytes written
 */
size_t write_chunk(const char *tag, const char *data, uint8_t size, char *out);

// copy a variable's byte represenation to an output
#define cpy_bytes_from_var(var, out) cpy_bytes((char *)&(var), sizeof(var), out)
// copy some bytes from an input stream to a variable
#define cpy_bytes_to_var(in, var) cpy_bytes(in, sizeof(var), (char *)&(var))
// write a variable as a chunk
#define write_chunk_var(tag, var, out) write_chunk(tag, (char *)&(var), sizeof(var), out)

#endif // COOKIE_FILES_H
