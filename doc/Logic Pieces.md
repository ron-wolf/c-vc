# Logic Pieces

## Making the path to a file

```c
#include <stdlib.h> // calloc, system
#include <string.h> // strlen

#ifndef APPEND_INTO
#define APPEND_INTO(DESTNAME, PREFIX, STRNAME, SUFFIX)                  \
  do {                                                                  \
    int result_len = sizeof (PREFIX) - 1                                \
                   + strlen(STRNAME)                                    \
                   + sizeof (SUFFIX) - 1;                               \
    DESTNAME = calloc(result_len + 1, sizeof (char));                   \
    snprintf(DESTNAME, result_len, PREFIX "%s" SUFFIX, STRNAME);        \
  } while (0)
#endif

int mkfile(char const * file_path) {
  char * cmd = NULL; int status = 0;
  
  APPEND_INTO(cmd, "mkdir -p \"$(dirname '", file_path, "')\"");
  status = system(cmd); free(cmd);
  if (status) return status;
  
  APPEND_INTO(cmd, "touch '", file_path, "'");
  status = system(cmd); free(cmd);
  return status;
}
```

## Starting a socket

```c
#include <netinet/ip.h> // IPPROTO_*, struct sockaddr_in
#include <netinet/tcp.h>
#include <stdio.h>
#include <strings.h> // strlen
#include <sysexits.h>
#include <sys/errno.h> // errno
#include <sys/socket.h> // socket, AF_*, SOCK_*
#include <sys/types.h>

#ifndef REPORT
#define REPORT(EXIT, MSG, ARGS...)     \
  do {                                 \
    fprintf(stderr, MSG "\n" ## ARGS); \
    return (EXIT);                     \
  } while (0)

#define MAX_QUEUED_CONNS 20

#define ADDR_COUNT 3
uint32_t addresses[ADDR_COUNT] = {123, 456, 789};

int sock_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
{ 
  struct sockaddr_in target = { sin_family: AF_INET, sin_port: 21 };
  int i; for (i = 0; i < ADDR_COUNT; ++i) {
    target.sin_addr = { s_addr: addresses[i] };
    bind(sock_fd, & target, sizeof target);
  }
}
listen(sock_fd, MAX_QUEUED_CONNS);

struct sockaddr recv_addr;
socklen_t addr_len;
while (0 <= accept(sock_fd, & recv_addr, & addr_len)) { // blocking call
  // do stuff
}
```

### Notes

Port 21 is for “passive FTP”; [see `setcap(8)`](https://stackoverflow.com/a/414258/8617859) for caveats. According to `ip(7)`:

> The port numbers below 124 are called *privileged ports* (or sometimes: *reserved ports*). Only privileged processes (i.e., those having the `CAP_NET_BIND_SERVICE` capability) may `bind(2)` to these sockets.