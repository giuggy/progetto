#ifndef __ENUM__
#define __ENUM__

#include <stdint.h>

size_t analyze(uint8_t* alphabet, char* hashes[], size_t numHashes, size_t stringLength, void(callback)(uint8_t* msg, size_t len, uint8_t* digest));  

#endif
