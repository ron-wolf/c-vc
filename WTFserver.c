#include "macros.h"

#include <stdio.h>
#include <sysexits.h>

char usage[] = "./WTFserver {port #}";

int main(int argc, char * const argv[]) {
    if (argc < 2) EXIT(EX_USAGE, usage);
    
    unsigned port;
    int status = sscanf(argv[1], "%u", & port);
    if (status < 1) EXIT(EX_DATAERR, "Invalid port: %s", argv[1]);
    
    NOTIF("This command opens a server on port %u", port);
    return 0;
}
