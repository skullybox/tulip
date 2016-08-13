/***
 Copyright (C) irfan
 module module callbacks
**/

#include "tul_module.h"

extern tul_read_callback tul_RD_callback;
extern tul_write_callback tul_WR_callback;

void configure_module()
{
  tul_RD_callback = (tul_read_callback) &module_read;
  tul_WR_callback = (tul_write_callback) &module_write;
}

void module_read(char *buff, unsigned *recv)
{

}


void module_write(char *buff, unsigned *sent, unsigned sz)
{

}


