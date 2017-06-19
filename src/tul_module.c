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
  if (c->_trecv != REQ_SZ)
    return;
}


void module_write(tul_net_context *c)
{
  if(c->_tsend != RES_SZ)
    return;

}


