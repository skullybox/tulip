/***
 Copyright (C) irfan
 chat module callbacks
**/

#include "tul_module_chats.h"

extern tul_read_callback tul_RD_callback;
extern tul_write_callback tul_WR_callback;

void chat_configure_module()
{
  tul_RD_callback = chat_read;
  tul_WR_callback = chat_write;
}

void chat_read(char *buff, unsigned *recv)
{

}


void chat_write(char *buff, unsigned *sent, unsigned sz)
{

}


