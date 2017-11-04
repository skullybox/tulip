/***
  Copyright (C) irfan
  mysql client
 **/

#include "tul_module.h"
#include "mysql.h"

#ifndef _tul_mysql
#define _tul_mysql

int tul_dbinit();
int tul_query(int num_q, ...);
int tul_query_get(char *SQL, MYSQL_RES **res);
void tul_dbclean();
#endif

