/***
  Copyright (C) irfan
  mysql client
 **/

#include "tul_module.h"

#ifndef _tul_mysql
#define _tul_mysql

#ifdef USE_MYSQL
int db_init();
int db_query(uchar *q);
#endif

#endif

