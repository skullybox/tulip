/***
  Copyright (C) irfan
  database interface
 **/

#ifndef _tul_db
#define _tul_db

#include <sqlite3.h>

static char TUL_DBNAME[] = "tulip.db";

int tul_query(int num_q, ...); 
void tul_dbinit();
void tul_dbclean();

#endif


