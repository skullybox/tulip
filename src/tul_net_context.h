/***
  Copyright (C) irfan

 **/

#ifndef _tul_net_context
#define _tul_net_context

#include "tul_globals.h"

#define CTX_BLOCK 32768

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


typedef struct __tul_net_context
{
  unsigned _use_tls;
  unsigned _sock;
  unsigned _tsend;
  unsigned _trecv;
  unsigned payload_out_cnt;
  unsigned long long timestamp;
  char payload_in[CTX_BLOCK];
  char payload_out[CTX_BLOCK];
  mbedtls_ssl_context ssl;
  mbedtls_ssl_config conf;
  mbedtls_net_context net_c;
} tul_net_context;

typedef struct __tul_int_context_struct
{
  tul_net_context *this;
  struct __tul_int_context_struct *next;
  struct __tul_int_context_struct *back;
} _tul_int_context_struct;

int tul_get_sock(unsigned pos);
int tul_add_context(unsigned sock, int tls);
void tul_rem_context(unsigned sock);
void tul_init_context_list();
void tul_dest_context_list();
tul_net_context* tul_find_context(unsigned sock);

#endif
