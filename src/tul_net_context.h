/***
  Copyright (C) irfan

 **/

#include "tul_globals.h"
#include "tul_tls_common.h"
#include "tul_module.h"

#ifndef _tul_net_context
#define _tul_net_context

typedef struct  __tul_net_context
{
  unsigned _use_tls;
  unsigned _sock;
  unsigned _tsend;
  unsigned _trecv;
  unsigned _ttsend;
  unsigned long long timestamp;
  char payload_in[DEF_SOCK_BUFF_SIZE];
  char payload_out[DEF_SOCK_BUFF_SIZE];
  tul_tls_ctx tls;
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
