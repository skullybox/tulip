/***
  Copyright (C) irfan
  db interface definition
 **/

#include "tul_log.h"
#include "tul_db.h"
#include <stdio.h>

extern int TUL_SIGNAL_INT;
#define SQLPOOL 30
sqlite3 *sql_pool[SQLPOOL];
pthread_mutex_t pool_locks[SQLPOOL];


/* query mode, followed by list of queries
 * mode 0 = read
 * mode 1 = write
 */
void tul_query(unsigned mode, char *x,...)
{

  /* TODO: get a econnection
   * from the connection pool
   */

 
  /* TODO: execute queries */

}

void tul_dbinit()
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
    for (int i = 0; i < SQLPOOL; i++)
    {
      rc = sqlite3_open(TUL_DBNAME, &sql_pool[i]);
      if(rc != SQLITE_OK)
      {
        tul_log(" tulip_boot >>>> ERROR: db connection pool IO error");
        tul_dbclean();

        return;
      }
    }


}

void tul_dbclean()
{
  for (int i = 0; i < SQLPOOL; i++)
  {
    if(sql_pool[i])
    {
      sqlite3_close(sql_pool[i]);
    }
  }
}



