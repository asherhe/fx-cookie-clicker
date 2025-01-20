#include "files.h"
#include <stdint.h>
#include <stddef.h>
#include <string.h>

size_t cpy_bytes(const char *data, size_t size, char *out)
{
  int x = 1;
  // check endianness (we want to write in big endian)
  if (*(char *)&x == 1)
  {
    // little-endian: write in reverse order
    for (size_t i = 0; i < size; ++i)
      out[size - i - 1] = data[i];
  }
  else
  {
    // big endian: we can just copy
    memcpy(out, data, size);
  }
  return size;
}

size_t write_chunk(const char *tag, const char *data, uint8_t size, char *out)
{
  memcpy(out, tag, 4);
  *(out += 4) = size; // double
  cpy_bytes(data, size, ++out);
  return 5 + size;
}
