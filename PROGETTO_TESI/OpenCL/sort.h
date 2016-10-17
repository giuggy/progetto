#ifndef __SORT__
#define __SORT__

#include <stdlib.h>


// macro to simplify error handling
#define ERROR_HELPER(ret, message)  do {                                \
            if (ret < 0) {                                              \
                fprintf(stderr, "%s: %s\n", message, strerror(errno));  \
                exit(EXIT_FAILURE);                                     \
            }                                                           \
        } while (0)


void sortArrayOfString(char** strings, size_t numLines);
size_t find(char** str, char* str_hash, size_t numL, size_t stringLength);
size_t findAndRemove(char** strings, char* s, size_t numLines);



#endif
