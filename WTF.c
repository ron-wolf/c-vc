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

#include <arpa/inet.h>
#include <netinet/ip.h>
#include <sys/socket.h>

#include "macros.h"

char usage[] = "./WTF configure      {IP address} {port #}" "\n"
               "./WTF checkout       {project}"             "\n"
               "./WTF update         {project}"             "\n"
               "./WTF upgrade        {project}"             "\n"
               "./WTF commit         {project}"             "\n"
               "./WTF push           {project}"             "\n"
               "./WTF create         {project}"             "\n"
               "./WTF destroy        {project}"             "\n"
               "./WTF add            {project} {file}"      "\n"
               "./WTF remove         {project} {file}"      "\n"
               "./WTF currentversion {project}"             "\n"
               "./WTF history        {project}"             "\n"
               "./WTF rollback       {project} {version}"
;
               

static unsigned long hash(char * string) {
    unsigned long hash = 5381;
    unsigned char c; while ((c = (unsigned char) *(string++)))
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

#define TEST_PORT 9021
#define BUFF_SIZE 1024

int read_config(struct sockaddr_in * addr) {
    int matches;
    
    FILE * config_file = fopen(".configure", "r");
    if (! config_file) {
        memset(addr, 0, sizeof (struct sockaddr_in));
        matches = -1;
    } else {
        addr->sin_family = AF_INET;
        matches = fscanf(config_file, "%u\n%hu\n", & addr->sin_addr.s_addr, & addr->sin_port);
    }
    
    return matches;
}

int serv_conn(struct sockaddr_in * addr, size_t len) {
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        LOG("Couldn't create socket");
        return sock_fd;
    }
    
    int status = connect(sock_fd, (struct sockaddr *) & addr, (socklen_t) len);
    if (status < 0) {
        LOG("Could not connect to server");
        return status;
    }
    
    return sock_fd;
}

int main(int argc, char * const argv[]) {
    
    if (argc < 2) {
        fprintf(stderr, "%s", usage);
        return EX_USAGE;
    }
    
    struct sockaddr_in addr;
    int status;
    
    switch (hash(argv[1])) {
        case CONFIGURE: {
            if (argc < 4) EXIT(EX_USAGE, "%s", usage);
            
            status = inet_pton(AF_INET, argv[2], & addr.sin_addr);
            if (status <= 0) EXIT(EX_USAGE, "Invalid IP address");
            status = sscanf(argv[3], "%hu", & addr.sin_port);
            if (status < 1) EXIT(EX_USAGE, "Improperly formatted port");
            
            int fd = openat(AT_FDCWD, ".configure", O_CREAT | O_WRONLY);
            if (fd < 0) EXIT(EX_SOFTWARE, "Couldn't write to the \".configure\" file");
            dprintf(fd, "%u\n%hu\n", addr.sin_addr.s_addr, addr.sin_port);
            close(fd);
            
            EXIT(EX_OK, "The configuration was successfully written.");
        }
        
        case CHECKOUT: {
            if (argc < 3) EXIT(EX_USAGE, "%s", usage);
            
            char * project = argv[2];
            
            status = read_config(& addr);
            if (status < 1) EXIT(EX_CONFIG, "No valid \".configure\" file exists in project root");
            
            int sock_fd = serv_conn(& addr, sizeof addr);
            if (sock_fd < 0) EXIT(EX_NOHOST, "Server connection failed");
            else LOG("Connection succeeded");
            FILE * sock_file = fdopen(sock_fd, "a+");
            
            fprintf(sock_file, "ex:%s;", project);
            
            char ex_buf[2] = {'\0'};
            status = recv(sock_fd, ex_buf, 2, 0);
            if (status <= 0) {
                if (status == 0) EXIT(EX_PROTOCOL, "Connection closed");
                else EXIT(EX_TEMPFAIL, "Could not receive data");
            }
            if (strncmp(ex_buf, "ex", 2) == 0) LOG("Project \"%s\" was found", project);
            else if (strncmp(ex_buf, "ne", 2) == 0) EXIT(EX_OK, "No project \"%s\" on server", project);
            else EXIT(EX_PROTOCOL, "Couldn't check whether project exists");
            
            fprintf(sock_file, "co:%s;", project);
            int tar_fd = open("proj.tar", O_CREAT | O_WRONLY);
            char tar_buf[BUFF_SIZE] = {'\0'};
            status = 1;
            for (status = 1; status > 0; status = recv(sock_fd, tar_buf, 1024, 0)) {
                write(tar_fd, tar_buf, BUFF_SIZE);
            }
            if (status <= 0) {
                if (status == 0) LOG("Connection closed");
                else EXIT(EX_TEMPFAIL, "Could not read data from host");
            }
            
            system("tar -xvf proj.tar -C .");
            system("rm -f proj.tar");
            
            EXIT(EX_OK, "All files retrieved!");
        }
        
        case UPDATE:
            if (argc < 3) EXIT(EX_USAGE, "%s", usage);
            
            LOG("This command retrieves all remote changes and stores them in a file called \".Update\".\n");
            
            return 0;
        
        case UPGRADE:
            if (argc < 0) EXIT(EX_USAGE, "%s", usage);
            
            LOG("This command accepts the changes listed in the \".Update\" file.\n");
            
            return 0;
        
        case COMMIT:
            if (argc < 0) EXIT(EX_USAGE, "%s", usage);
            
            LOG("This command catalogs your local changes in a file called \".Commit\".\n");
            
            return 0;
        
        case PUSH:
            if (argc < 0) EXIT(EX_USAGE, "%s", usage);
            
            LOG("This command pushes the changes listed in the \".Update\" file.\n");
            
            return 0;
        
        case CREATE:
            if (argc < 0) EXIT(EX_USAGE, "%s", usage);
            
            LOG("This command makes a new project on the server.\n");
            
            return 0;
        
        case DESTROY:
            if (argc < 3) EXIT(EX_USAGE, "%s", usage);
            
            LOG("This command removes the project \"%s\" from the server.", argv[2]);
            
            return 0;
        
        case ADD:
            if (argc < 4) EXIT(EX_USAGE, "%s", usage);
            
            LOG("THis command adds the file \"%s\" to the project \"%s\".", argv[3], argv[2]);
            
            return 0;
        
        case REMOVE:
            if (argc < 4) EXIT(EX_USAGE, "%s", usage);
            
            LOG("This command removes the file \"%s\" from the project \"%s\".", argv[3], argv[2]);
            
            return 0;
        
        case CURRENTVERSION:
            if (argc < 3) EXIT(EX_USAGE, "%s", usage);
            
            LOG("This command retrieves all files and versions for the project \"%s\" from the server.", argv[2]);
            
            return 0;
        
        case HISTORY:
            if (argc < 3) EXIT(EX_USAGE, "%s", usage);
            
            LOG("This command retrieves the full history of the project \"%s\" from the server.", argv[2]);
            
            return 0;
        
        case ROLLBACK:
            if (argc < 4) EXIT(EX_USAGE, "%s", usage);
            
            LOG("This command rolls the local version of project \"%s\" back to version \"%s\".", argv[2], argv[3]);
            
            return 0;
        
        default: EXIT(EX_USAGE, "%s", usage);
    }
    
    return 0;
}
