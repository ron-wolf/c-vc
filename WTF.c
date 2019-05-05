#include <dirent.h>
#include <fcntl.h>    // O_*
#include <string.h>
#include <stdio.h>    // popen(), pclose()
#include <stdlib.h>   // system()
#include <sysexits.h> // EX_*
#include <unistd.h>
#include <sys/errno.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "datatypes.h"
#include "macros.h"

char usage[] = "./WTF configure {IP address} {port #}" "\n"
               "./WTF checkout {project}"              "\n"
               "./WTF update {project}"                "\n"
               "./WTF upgrade {project}"               "\n"
               "./WTF commit {project}"                "\n"
               "./WTF push {project}"                  "\n"
               "./WTF create {project}"                "\n"
               "./WTF destroy {project}"               "\n"
               "./WTF add    {project} {file}"         "\n"
               "./WTF remove {project} {file}"         "\n"
               "./WTF currentversion {project}"        "\n"
               "./WTF history {project}"               "\n"
               "./WTF rollback {project} {version}"
;
               

static unsigned long hash(char * string) {
    unsigned long hash = 5381;
    int c; while ((c = *(string++)))
        hash = ((hash << 5) + hash) + c;
    return hash;
}

/*
# python
def hash(string):
  hash = 5381
  for char in string:
    hash *= 33
    hash += ord(char)
  hash %= 2 ** 64
  return hash
*/

enum command_hashes {
    CONFIGURE      =   249884308848926567ul,
    CHECKOUT       =     7572242387329307ul,
    UPDATE         =        6954104508584ul,
    UPGRADE        =      229485452931341ul,
    COMMIT         =        6953399221742ul,
    PUSH           =           6385597157ul,
    CREATE         =        6953402479289ul,
    DESTROY        =      229463081808367ul,
    ADD            =            193486030ul,
    REMOVE         =        6953974396019ul,
    CURRENTVERSION = 10849296509759198024ul,
    HISTORY        =      229468404218647ul,
    ROLLBACK       =     7572890988531503ul,
};

int main(int argc, char * const argv[]) {
    
    if (argc < 2) {
        fprintf(stderr, "%s", usage);
        return EX_USAGE;
    }
    
    switch (hash((unsigned char *) argv[1])) {
        case CONFIGURE: {
            if (argc < 4) EXIT(EX_USAGE, usage);
            
            NOTIF("This command stores the server configuration in a file called \".configure\".");
            
            struct config config; int status;
            status = sscanf(argv[2], "%hhu.%hhu.%hhu.%hhu", config.ip, config.ip+1, config.ip+2, config.ip+3);
            if (status < 4) EXIT(EX_USAGE, "IP address was formatted incorrectly.");
            status = sscanf(argv[3], "%u", &config.port);
            if (status < 1) EXIT(EX_USAGE, "Port was formatted incorrectly.");
            
            int fd = openat(AT_FDCWD, ".configure", O_CREAT | O_WRONLY);
            if (fd < 0) EXIT(EX_SOFTWARE, "Couldn't write to the \".configure\" file: %s", strerror(errno));
            
            dprintf(fd, "%lu\n%u\n", * (unsigned long *) config.ip, config.port);
            close(fd);
            
            EXIT(EX_OK, "The configuration was successfully written.");
        }
        
        case CHECKOUT:
            if (argc < 3) EXIT(EX_USAGE, usage);
            
            NOTIF("This command retrieves all files for the project \"%s\" from the server.", argv[2]);
            
            return 0;
        
        case UPDATE:
            if (argc < 3) EXIT(EX_USAGE, usage);
            
            NOTIF("This command retrieves all remote changes and stores them in a file called \".Update\".\n");
            
            return 0;
        
        case UPGRADE:
            if (argc < 0) EXIT(EX_USAGE, usage);
            
            NOTIF("This command accepts the changes listed in the \".Update\" file.\n");
            
            return 0;
        
        case COMMIT:
            if (argc < 0) EXIT(EX_USAGE, usage);
            
            NOTIF("This command catalogs your local changes in a file called \".Commit\".\n");
            
            return 0;
        
        case PUSH:
            if (argc < 0) EXIT(EX_USAGE, usage);
            
            NOTIF("This command pushes the changes listed in the \".Update\" file.\n");
            
            return 0;
        
        case CREATE:
            if (argc < 0) EXIT(EX_USAGE, usage);
            
            NOTIF("This command makes a new project on the server.\n");
            
            return 0;
        
        case DESTROY:
            if (argc < 3) EXIT(EX_USAGE, usage);
            
            NOTIF("This command removes the project \"%s\" from the server.", argv[2])
            
            return 0;
        
        case ADD:
            if (argc < 4) EXIT(EX_USAGE, usage);
            
            NOTIF("THis command adds the file \"%s\" to the project \"%s\".", argv[3], argv[2]);
            
            return 0;
        
        case REMOVE:
            if (argc < 4) EXIT(EX_USAGE, usage);
            
            NOTIF("This command removes the file \"%s\" from the project \"%s\".", argv[3], argv[2]);
            
            return 0;
        
        case CURRENTVERSION:
            if (argc < 3) EXIT(EX_USAGE, usage);
            
            NOTIF("This command retrieves all files and versions for the project \"%s\" from the server.", argv[2]);
            
            return 0;
        
        case HISTORY:
            if (argc < 3) EXIT(EX_USAGE, usage);
            
            NOTIF("This command retrieves the full history of the project \"%s\" from the server.", argv[2]);
            
            return 0;
        
        case ROLLBACK:
            if (argc < 4) EXIT(EX_USAGE, usage);
            
            NOTIF("This command rolls the local version of project \"%s\" back to version \"%s\".", argv[2], argv[3]);
            
            return 0;
        
        default: EXIT(EX_USAGE, usage);
    }
    
    return 0;
}
