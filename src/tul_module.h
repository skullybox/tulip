/***
  Copyright (C) irfan
  module callbacks
 **/


#ifndef _tul_module
#define _tul_module

#include "tul_service.h"
#include "tul_net_context.h"
#include "whirlpool_hash.h"

void configure_module();

void module_read(tul_net_context *c);
void module_write(tul_net_context *c);

typedef unsigned char uchar;

typedef struct _comm_0
{
  uchar user[30];
  uchar salt[16];
  uchar kek[16];
  uchar hmac[DIGESTBYTES]; /* hmac of payload */
  uchar hmac2[DIGESTBYTES]; /* hmac of header (this struct) */
  unsigned payload_sz;
} comm_req;

typedef struct _comm_1
{
  uchar user[30];
  uchar salt[16];
  uchar kek[16];
  uchar hmac[DIGESTBYTES];
  uchar hmac2[DIGESTBYTES];
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
    GET_MSG,
    GET_FREQ,
    ADDFRIEND, /* friend request */
    ACCEPTFRIEND,
    IGNOREFRIEND,
    DELFRIEND,
    RESPONSE   /* indicates status payload */
};

enum status
{
  OK = 2000,
  ERROR,
  INVALID,
  PAGING,
  END
};

static const unsigned RES_HSZ = sizeof(comm_resp); 
static const unsigned REQ_HSZ = sizeof(comm_req); 

#define _MAX_REQ_PAYLOAD (DEF_SOCK_BUFF_SIZE - REQ_HSZ)
#define _MAX_RES_PAYLOAD (DEF_SOCK_BUFF_SIZE - RES_HSZ)

#define MAX_REQ_PAYLOAD ((_MAX_REQ_PAYLOAD / 16)*16)
#define MAX_RES_PAYLOAD ((_MAX_REQ_PAYLOAD / 16)*16)

/*
 * message byte setup:
 * 8 bytes message id
 * 4 bytes true / false new flag
 * 3 bytes type (SYS/USR)
 * 30 bytes to user
 * 50 bytes reserved
 * 4 bytes message length
 * message from user
 *
 */
#define MESSAGE_META_SZ (8+4+3+30+50+4)
#define _MAX_MESSAGE (MAX_REQ_PAYLOAD - MESSAGE_META_SZ)
#define MAX_MESSAGE ((_MAX_MESSAGE/16)*16)


#endif
