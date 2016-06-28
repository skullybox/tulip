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




 #endif
