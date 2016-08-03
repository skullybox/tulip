/***
  Copyright (C) irfan
  chat module callbacks
 **/

#ifndef _tul_module_chats
#define _tul_module_chats

#include "tul_service.h"

void chat_configure_module();

void chat_read(char *buff, unsigned *recv);
void chat_write(char *buff, unsigned *sent, unsigned sz);

#endif
