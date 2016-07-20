/***
  Copyright (C) irfan

 **/

 #ifndef _tul_net_context
 #define _tul_net_context

#define CTX_BLOCK 20000

 typedef struct __tul_net_context
 {
   unsigned _sock;
   unsigned long long _tsend;
   unsigned payload_out_cnt;
   unsigned payload_in_cnt;
   unsigned char payload_in[CTX_BLOCK];
   unsigned char payload_out[CTX_BLOCK];
 } tul_net_context;

typedef struct __tul_int_context_struct
{
  tul_net_context *this;
  struct __tul_int_context_struct *next;
  struct __tul_int_context_struct *back;
} _tul_int_context_struct;

void tul_add_context(unsigned sock);
void tul_rem_context(unsigned sock);
void tul_init_context_list();
void tul_dest_context_list();


 #endif