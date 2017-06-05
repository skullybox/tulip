/***
  Copyright (C) irfan

 **/

#ifndef _tul_tls
#define _tul_tls

#include "mbedtls/net_sockets.h"
#include "mbedtls/debug.h"
#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/error.h"
#include "mbedtls/certs.h"
#if defined(MBEDTLS_SSL_CACHE_C)
#include "mbedtls/ssl_cache.h"
#endif

typedef struct _tls_ctx
{
  char host[1024];
  mbedtls_net_context server_fd;
  mbedtls_entropy_context entropy;
  mbedtls_ctr_drbg_context ctr_drbg;
  mbedtls_ssl_context ssl;
  mbedtls_ssl_config conf;
  mbedtls_x509_crt cert;
  mbedtls_pk_context pkey;
#if defined(MBEDTLS_SSL_CACHE_C)
  mbedtls_ssl_cache_context cache;
#endif

} tul_tls_ctx;


int tls_client_init(tul_tls_ctx *c, int lport);
int tls_client_free(tul_tls_ctx *c);
int tls_server_init(tul_tls_ctx *c, int lport);
int tls_server_free(tul_tls_ctx *c);
int tls_read(tul_tls_ctx *c, char *buf, unsigned len);
int tls_write(tul_tls_ctx *c, char *buf, unsigned len);

#endif



