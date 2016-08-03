/***
 Copyright (C) irfan
 Network processing functions
**/

#ifndef _tul_service
#define _tul_service

#include "tul_globals.h"
#include "tul_net_context.h"

#define MAX_NET_BLOCK 20000

typedef void (*tul_read_callback)(char *buff, 
    unsigned *trecv);

typedef void (*tul_write_callback)(char *buff,
    unsigned *tsend,
    unsigned *sz);

void tul_service(tul_net_context *ctx, short send_flg);

#endif
