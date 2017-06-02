/***
  Copyright (C) irfan
  module callbacks
 **/

#ifndef _tul_module
#define _tul_module

#include "tul_service.h"

void configure_module();

void module_read(char *buff, unsigned *recv);
void module_write(char *buff, unsigned *sent, unsigned sz);

#endif
