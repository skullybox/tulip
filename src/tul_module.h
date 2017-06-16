/***
  Copyright (C) irfan
  module callbacks
 **/

#ifndef _tul_module
#define _tul_module

#include "tul_service.h"

void configure_module();

void module_read(char *buff, unsigned *recv);
void module_write(char *buff, unsigned *sent);

/*
 *  16 byte salt
 *  16 byte md5
 *  30 byte host
 *  30 byte user
 *  30 byte action
 * 210 byte password
 */ 

typedef unsigned char uchar;

typedef struct _comm_0
{
  uchar salt[16];
  uchar md5[16];
  uchar host[30];
  uchar user[30];
  uchar action[30];
  uchar password[210];
} comm_req;

typedef struct _comm_1
{
  uchar salt[16];
  uchar md5[16];
  uchar res[30];
} comm_resp;

static const unsigned RES_SZ = sizeof(comm_resp);
static const unsigned REQ_SZ = sizeof(comm_req);

#endif
