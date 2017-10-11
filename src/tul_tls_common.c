/***
  Copyright (C) irfan

 **/

#include "tul_tls_common.h"
#include <string.h>

#define CHECK_RET if(ret) return -1; 

/* returns bytes read. -1 closed connection 
 * -2 error
 */
int tls_read(tul_tls_ctx *c, char *buf, unsigned len)
{
  int ret = 0;

  ret = mbedtls_ssl_read( &(c->ssl), (unsigned char *)buf, len );

  if(ret < 0)
    return -2;

  return ret;
}

/* returns bytes written. -1 closed connection 
 * -2 error
 */
int tls_write(tul_tls_ctx *c, char *buf, unsigned len)
{
  int ret = 0;

  ret = mbedtls_ssl_write( &(c->ssl), (unsigned char *)buf, len );

  if(ret < 0)
    return -2;

  return ret;
}



