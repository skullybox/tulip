/***
  Copyright (C) irfan

 **/

#include "tul_tls_client.h"
#include <string.h>
#include <assert.h>

#define CHECK_RET if(ret) assert(ret == 0);

int tls_client_init(tul_tls_ctx *c, int lport)
{
  int ret = 0;
  char buff[13] = {0};
  sprintf(buff, "%d", lport);

  mbedtls_net_init( &(c->server_fd) );
  mbedtls_ssl_init( &(c->ssl) );
  mbedtls_ssl_config_init( &(c->conf) );
#if defined(MBEDTLS_SSL_CACHE_C)
  mbedtls_ssl_cache_init( &(c->cache) );
#endif
  mbedtls_x509_crt_init( &(c->cert) );
  mbedtls_ctr_drbg_init( &(c->ctr_drbg) );
  mbedtls_pk_init( &(c->pkey) );

  mbedtls_entropy_init( &(c->entropy) );

  ret = mbedtls_ctr_drbg_seed( &(c->ctr_drbg), 
      mbedtls_entropy_func, &(c->entropy),
      NULL, 0);
  CHECK_RET;

  ret = mbedtls_x509_crt_parse( &(c->cert), 
      (const unsigned char *) mbedtls_test_cas_pem,
      mbedtls_test_cas_pem_len );
  CHECK_RET;

  ret = mbedtls_net_connect( &(c->server_fd), 
      c->host, buff, MBEDTLS_NET_PROTO_TCP );
  CHECK_RET;

#if defined(MBEDTLS_SSL_CACHE_C)
  mbedtls_ssl_conf_session_cache( &(c->conf), &(c->cache),
      mbedtls_ssl_cache_get,
      mbedtls_ssl_cache_set );
#endif

  ret = mbedtls_pk_parse_key( &(c->pkey), 
      (const unsigned char *) mbedtls_test_srv_key,
      mbedtls_test_srv_key_len, NULL, 0 );
  CHECK_RET;

  ret = mbedtls_ssl_config_defaults( &(c->conf),
      MBEDTLS_SSL_IS_CLIENT,
      MBEDTLS_SSL_TRANSPORT_STREAM,
      MBEDTLS_SSL_PRESET_DEFAULT );
  CHECK_RET;

  mbedtls_ssl_conf_rng( &(c->conf), 
      mbedtls_ctr_drbg_random, &(c->ctr_drbg) );

  mbedtls_ssl_conf_authmode( &(c->conf), MBEDTLS_SSL_VERIFY_NONE);
  mbedtls_ssl_conf_ca_chain( &(c->conf), &(c->cert), NULL );

  ret = mbedtls_ssl_setup( &(c->ssl), &(c->conf) );
  CHECK_RET;

  ret = mbedtls_ssl_set_hostname(&(c->ssl), "tls-c");
  CHECK_RET;
 
  mbedtls_ssl_set_bio( &(c->ssl), 
      &(c->server_fd), mbedtls_net_send, mbedtls_net_recv, NULL );

  ret = mbedtls_ssl_handshake( &(c->ssl) );
  CHECK_RET;

  return 0;
}


int tls_client_free(tul_tls_ctx *c)
{
  mbedtls_net_free( &(c->server_fd) );
  mbedtls_x509_crt_free( &(c->cert) );
  mbedtls_pk_free( &(c->pkey) );
  mbedtls_ssl_free( &(c->ssl) );
  mbedtls_ssl_config_free( &(c->conf) );
#if defined(MBEDTLS_SSL_CACHE_C)
  mbedtls_ssl_cache_free( &(c->cache) );
#endif
  mbedtls_ctr_drbg_free( &(c->ctr_drbg) );
  mbedtls_entropy_free( &(c->entropy) );

  return 0;
}

