#ifndef __MACROS_H__

#include <stdlib.h>

#ifdef DEBUG_OUTPUT
#define NOTIF(MSG, ARGS...) fprintf(stderr, MSG "\n", ## ARGS)
#else
#define NOTIF(MSG, ARGS...) fprintf(stderr, "");
#endif

#define EXIT(EX, MSG, ARGS...) \
    do {                       \
        NOTIF(MSG, ## ARGS);   \
        exit(EX);              \
    } while (0)

#define __MACROS_H__
#endif
