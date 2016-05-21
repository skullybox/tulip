/***
 Copyright (C) irfan

 place globals here
**/

#ifndef _tul_globals
#define _tul_globals

#include <netdb.h>
#include <string.h>

#define DEF_SOCK_BUFF_SIZE 2048
#define DEF_CTX_LIST_SZ 120

typedef struct __app_context
{
  unsigned int sock;
  struct sockaddr_in addr;
  short udp_mode;
  char dat_in[DEF_SOCK_BUFF_SIZE];
  char dat_out[DEF_SOCK_BUFF_SIZE];
  unsigned int s;
  unsigned int r;
} tul_app_ctx;

tul_app_ctx tul_context_list[DEF_CTX_LIST_SZ];

extern int TUL_SIGNAL_INT;

#endif
