/***
  Copyright (C) irfan

 **/

#ifndef _tul_tls_client
#define _tul_tls_client

#include "tul_tls_common.h"

int tls_client_init(tul_tls_ctx *c, int lport);
int tls_client_free(tul_tls_ctx *c);

#endif



