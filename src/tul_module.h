/***
  Copyright (C) irfan
  module callbacks
 **/


#ifndef _tul_module
#define _tul_module

#include "tul_service.h"
#include "tul_net_context.h"

void configure_module();

void module_read(tul_net_context *c);
void module_write(tul_net_context *c);

typedef unsigned char uchar;

typedef struct _comm_0
{
  uchar salt[16];
  uchar hmac[64];
  uchar user[30];
  uchar kek[16];
  uint64_t payload_sz;
} comm_req;

typedef struct _comm_1
{
  uchar salt[16];
  uchar hmac[64];
  uchar kek[16];
  uint64_t payload_sz;
} comm_resp;

static const unsigned RES_SZ = sizeof(comm_resp); 
static const unsigned REQ_SZ = sizeof(comm_req); 

#define MAX_SEND_PAYLOAD = DEF_SOCK_BUFF_SIZE - REQ_SZ
#define MAX_REC_PAYLOAD = DEF_SOCK_BUFF_SIZE - RES_SZ


#endif
