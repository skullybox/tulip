/***
  Copyright (C) irfan
  db interface definition
 **/

#include "tul_log.h"
#include "tul_db.h"
#include <sqlite3.h>
#include <stdio.h>

extern int TUL_SIGNAL_INT;

void tul_query(char *x,...)
{
  sqlite3 *db = NULL; 
  int rc = 0;

  rc = sqlite3_open(TUL_DBNAME, &db);

  if(rc != SQLITE_OK)
  {
    tul_log("ERROR: database IO error - Exiting"); 
    TUL_SIGNAL_INT = 1;
    return;
  }

  /* TODO: execute queries */

}

void tul_init()
{
  /* check if db file exists */
  FILE *fp = fopen(TUL_DBNAME, "r");

  if(fp)
  {
    tul_log(" tulip_boot >>>> found existing database");
    fclose(fp);
    return;
  }

  tul_log(" tulip_boot >>>> initializing database");


  /* open database connection
   * this will create the empty DB file
   */
  sqlite3 *db = NULL; 
  int rc = 0;

  rc = sqlite3_open(TUL_DBNAME, &db);

  if(rc != SQLITE_OK)
  {
    tul_log(" tulip_boot >>>> ERROR: db file IO error");
    return;
  }
  sqlite3_close(db);

  /* initialize the database at this point
   * execute queries to create tables and initialized
   * data.
   */

  
}


