/***
  Copyright (C) irfan
  database interface
 **/

#ifndef _tul_db
#define _tul_db

#include <sqlite3.h>

static char TUL_DBNAME[] = "tulip.sql";

void tul_query(unsigned mode, char *x,...);
void tul_dbinit();
void tul_dbclean();

#endif


