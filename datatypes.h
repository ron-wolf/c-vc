#ifndef __DATATYPES_H__

typedef unsigned char byte;

typedef byte ip_address [4];

struct config {
    ip_address ip;
    unsigned port;
};

#define __DATATYPES_H__
#endif
