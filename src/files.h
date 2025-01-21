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
#define CPY_BYTES_FROM_VAR(var, out) cpy_bytes((char *)&(var), sizeof(var), out)
// copy some bytes from an input stream to a variable
#define CPY_BYTES_TO_VAR(in, var) cpy_bytes(in, sizeof(var), (char *)&(var))
// write a variable as a chunk
#define WRITE_CHUNK(tag, var, out) write_chunk(tag, (char *)&(var), sizeof(var), out)

#endif // COOKIE_FILES_H
