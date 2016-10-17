#ifndef __MD5__
#define __MD5__

#include <stdint.h>

void md5(uint8_t *initial_msg, size_t initial_len, uint8_t *digest);

#endif
