/***
 Copyright (C) irfan
 Network processing functions
**/

#include "tul_service.h"

void tul_service(tul_net_context *ctx, short send_flg)
{
  if(send_flg)
  {
    if( ctx->_tsend == ctx->payload_out_cnt )
    {
      /* process after sent data */
       
      ctx->_tsend = 0;    
      ctx->payload_out_cnt = 0;
    }  
  }
  else 
  {
    /* process recieved data */

  }
}




