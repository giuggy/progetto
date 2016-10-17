#ifndef __COMMON_H__ // accorgimento per evitare inclusioni multiple di un header
#define __COMMON_H__

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <semaphore.h>

// macro to simplify error handling
#define ERROR_HELPER(ret, message)  do {                                \
            if (ret < 0) {                                              \
                fprintf(stderr, "%s: %s\n", message, strerror(errno));  \
                exit(EXIT_FAILURE);                                     \
            }                                                           \
        } while (0)

#define MAX_PWD_LENGTH      10
#define FILE_BUF_LENGTH     256

extern sem_t x;
extern sem_t wsem;
extern int readcount;
extern size_t numH;


#endif



