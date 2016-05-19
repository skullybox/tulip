/***
 Copyright (C) irfan

 place globals here
**/

#ifndef _tul_globals
#define _tul_globals

#include <netdb.h>
#include <string.h>

typedef struct __app_context
{
  unsigned int sock;
  struct sockaddr_in addr;
  short udp_mode;
  char dat_in[512];
  char dat_out[512];
  unsigned int s;
  unsigned int r;
} tul_app_ctx;

tul_app_ctx tul_context_list[120];

extern int TUL_SIGNAL_INT;

#endif
