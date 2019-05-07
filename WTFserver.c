#include <stdio.h>
#include <stdlib.h>
#include <string.h> // memset
#include <sysexits.h>

#include <ifaddrs.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <sys/socket.h>

#include "macros.h"
#include "repos.h"


#define MAX_QUEUED_CONNS  20
#define DELAY             3
#define MAX_PROJ_NAME_LEN 32

#define SHORT(char_ptr) (* (unsigned short *) (char_ptr))
enum cmd_abbrevs {
    CHECKOUT = SHORT("co"),
};

char usage[] = "./WTFserver {port #}";

void print_local_ips() {
    struct ifaddrs * info_list;
    getifaddrs(& info_list);
    while (info_list) {
        struct sockaddr_in * addr = (struct sockaddr_in *) info_list->ifa_addr;
        printf("The IP address for interface %s is %s\n", info_list->ifa_name, inet_ntoa(addr->sin_addr));
        info_list = info_list->ifa_next;
    }
    freeifaddrs(info_list);
}

void print_sock(int sock_fd) {
    char * buffer; for (
      buffer = calloc(sizeof (char), BUFF_SIZE);
      read(sock_fd, buffer, BUFF_SIZE) > 0;
      memset(buffer, '\0', BUFF_SIZE)) {
        printf("%s\n", buffer);
    }
    free(buffer);
    send(sock_fd, "Got it!", 7, 0);
}

void accept_sock(int sock_fd) {
    FILE * sock_file = fdopen(sock_fd, "r+");
    char * cmd = calloc(sizeof (char), 2 + 1);
    char * proj = calloc(sizeof (char), MAX_PROJ_NAME_LEN + 1);
    int status = fscanf(sock_file, "%2s:%s;", cmd, proj);
    if (status < 2) EXIT(EX_PROTOCOL, "Invalid command and project sent");
    if (proj[MAX_PROJ_NAME_LEN] != '\0') EXIT(EX_PROTOCOL, "Project name too long");
    switch (SHORT(cmd)) {
        case CHECKOUT: checkout(proj, sock_fd);
    }
}

int main(int argc, char * const argv[]) {
    if (argc < 2) EXIT(EX_USAGE, "%s", usage);
    
    int status;
    
    unsigned short port;
    status = sscanf(argv[1], "%hu", & port);
    if (status < 1) EXIT(EX_DATAERR, "Invalid port: %s", argv[1]);
    int sock_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (! sock_fd) EXIT(EX_PROTOCOL, "Couldn't open a socket");
    
    struct sockaddr_in local_addr = {
        .sin_family = AF_INET,
        .sin_port   = port,
        .sin_addr   = INADDR_ANY
    };
    struct sockaddr * addr_ptr = (struct sockaddr *) & local_addr;
    socklen_t addr_len = (socklen_t) sizeof local_addr;
    
    status = bind(sock_fd, addr_ptr, addr_len);
    if (status < 0) EXIT(EX_PROTOCOL, "Couldn't bind a socket");
    status = listen(sock_fd, MAX_QUEUED_CONNS);
    if (status < 0) EXIT(EX_PROTOCOL, "Couldn't listen on a socket");
    
    print_local_ips();
    
    int new_sock;
    while ((new_sock = accept(sock_fd, addr_ptr, & addr_len)) >= 0) {
        LOG("Got a message at file descriptor %d", new_sock);
        if (fork() == 0) {
            print_sock(new_sock);
            return EX_OK;
        }
        sleep(DELAY);
    }
    
    EXIT(EX_OK, "All done.");
}
