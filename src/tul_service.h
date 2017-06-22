/***
  Copyright (C) irfan
  Network processing functions
 **/

#include "tul_net_context.h"

#ifndef _tul_service
#define _tul_service

typedef void (*tul_read_callback)();

typedef void (*tul_write_callback)();

void tul_service(tul_net_context *ctx, short send_flg);

#endif
