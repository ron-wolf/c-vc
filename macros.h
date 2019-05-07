#ifndef __MACROS_H__

#include <stdio.h>     // fprintf()
#include <stdlib.h>    // exit()
#include <sys/errno.h> // errno

#define BUFF_SIZE         1024

#define LOG(MSG, ARGS...) fprintf(stderr, MSG "\n", ## ARGS)

#define EXIT(EX, MSG, ARGS...)                      \
    do {                                            \
        LOG(MSG, ## ARGS);                          \
        if (errno) LOG("WTF: %s", strerror(errno)); \
        exit(EX);                                   \
    } while (0)

#define __MACROS_H__
#endif
