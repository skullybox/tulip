/***
  Copyright (C) irfan

 **/

#include "tul_tls_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _tul_tls_client
#define _tul_tls_client

int tls_client_init(tul_tls_ctx *c, int lport);
int tls_client_free(tul_tls_ctx *c);

#endif


#ifdef __cplusplus
}
#endif
