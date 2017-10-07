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
  unsigned payload_sz;
} comm_req;

typedef struct _comm_1
{
  uchar salt[16];
  uchar hmac[64];
  uchar kek[16];
  unsigned payload_sz;
} comm_resp;

typedef struct _payload
{
  unsigned action;
  unsigned data_sz;
  void *data;
} comm_payload; 

enum actions
{
    LOGIN = 1000,
    LOGOUT,
    GET_LIST, /* list of friends */
    SEND_MSG,
    ADDFRIEND, /* friend request */
    DELFRIEND
};

static const unsigned RES_HSZ = sizeof(comm_resp); 
static const unsigned REQ_HSZ = sizeof(comm_req); 

#define MAX_REQ_PAYLOAD DEF_SOCK_BUFF_SIZE - REQ_HSZ
#define MAX_RES_PAYLOAD DEF_SOCK_BUFF_SIZE - RES_HSZ


#endif
