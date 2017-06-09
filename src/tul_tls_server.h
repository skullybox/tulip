/***
  Copyright (C) irfan

 **/

#ifndef _tul_tls_server
#define _tul_tls_server

#include "tul_tls_common.h"

int tls_server_init(tul_tls_ctx *c, int lport);
int tls_server_free(tul_tls_ctx *c);

#endif



