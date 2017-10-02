/***
  Copyright (C) irfan
  module module callbacks
 **/

#include "tul_module.h"
#include "tul_net_context.h"

extern tul_read_callback tul_RD_callback;
extern tul_write_callback tul_WR_callback;

void configure_module()
{
  tul_RD_callback = (tul_read_callback) &module_read;
  tul_WR_callback = (tul_write_callback) &module_write;
}

void module_read(tul_net_context *c)
{
  if (c->_trecv < REQ_HSZ)
    return;

  if(!c->_ttrecv)
  {
    comm_req r_tmp;
    memcpy(&r_tmp, c->payload_in, REQ_HSZ);
    c->_ttrecv = REQ_HSZ + r_tmp.payload_sz;
  }
}


void module_write(tul_net_context *c)
{
  if(c->_tsend >= c->_ttsend)
  {
    c->_ttsend = 0;
    memset(c->payload_out, 0, DEF_SOCK_BUFF_SIZE);
  }
}


