/***
  Copyright (C) irfan
  Network processing functions
 **/

#include "tul_service.h"
tul_read_callback tul_RD_callback = NULL;
tul_write_callback tul_WR_callback = NULL;


void tul_service(tul_net_context *ctx, short send_flg)
{
  if(send_flg)
  {
    /* process written data
     * call back function will reset buffers
     * and counters after processing recieved data */
    if(tul_WR_callback)
      tul_WR_callback(&(ctx->payload_out), 
          &(ctx->_tsend),
          &(ctx->payload_out_cnt));
  }
  else 
  {
    /* process recieved data
     * call back function will reset buffers
     * and counters after processing recieved data */
    if(tul_RD_callback)
      tul_RD_callback(&(ctx->payload_in), &(ctx->_trecv));
  }
}




