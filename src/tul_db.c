/***
  Copyright (C) irfan
  db interface definition
 **/

#include "tul_log.h"
#include "tul_db.h"
#include "tul_user.h"
#include <stdio.h>
#include <stdarg.h>
#include <pthread.h>

#define SQLPOOL 30
extern int TUL_SIGNAL_INT;

#ifndef USE_MSYQL
sqlite3 *sql_pool[SQLPOOL];
#endif

pthread_mutex_t pool_locks[SQLPOOL];


/* query followed by list of queries
 * num_q = number of queries
 */
int tul_query(int num_q,...)
{

#ifndef USE_MYSQL
  /* get a econnection
   * from the connection pool
   */
  char *q = NULL;
  int ret = 0;
  int errors_found = 0;
  unsigned pos = 0;
  unsigned locked = 0;
  va_list ap;


  while(!locked)
  {
    if(!pthread_mutex_trylock(&pool_locks[pos]))
    {
      locked = 1;
      break;
    }
    pos++;
    if(pos >= SQLPOOL)
      pos = 0;
  }

  /* execute queries */
  va_start(ap, num_q);
  for(int i = 0; i < num_q; i++)
  {
    q = va_arg(ap, char*);
    ret = sqlite3_exec(sql_pool[pos], q, NULL, NULL, NULL);
    if(ret)
      errors_found = 1;
  }
  va_end(ap);

  /* release lock */
  pthread_mutex_unlock(&pool_locks[pos]);
#endif

}

/* query followed by list of queries
 * num_q = number of queries
 */
void tul_query_get(char *q, char ***res, int *row, int *col)
{

#ifndef USE_MYSQL
  /* get a econnection
   * from the connection pool
   */
  int ret = 0;
  int pos = 0;
  unsigned locked = 0;

  while(!locked)
  {
    if(!pthread_mutex_trylock(&pool_locks[pos]))
    {
      locked = 1;
      break;
    }
    pos++;
    if(pos >= SQLPOOL)
      pos = 0;
  }

  /* execute query */
  sqlite3_get_table( sql_pool[pos], q, res, row, col, NULL);

  /* release lock */
  pthread_mutex_unlock(&pool_locks[pos]);
#endif

}


void tul_dbinit()
{
#ifndef USE_MYSQL
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

  /* initialize database connection pools 
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

  /* initialize the database at this point
   * execute queries to create tables and initialized
   * data.
   */
  tul_query(4,
      "create table user(uid varchar(15) unique not null, name varchar(50) not null, email varchar(50) unique not null, password varchar(24) not null, salt varchar(24) not null, ctime DATETIME DEFAULT CURRENT_TIMESTAMP, primary key(uid));",

      "create table friend_request(uid integer, user_from varchar(15), ctime DATETIME DEFAULT CURRENT_TIMESTAMP);",

      "create table friend_list(uid integer, friend integer, ctime DATETIME DEFAULT CURRENT_TIMESTAMP, foreign key(uid) references user(uid), foreign key(friend) references user(uid));",

      "create table message(msg_id integer not null, uid integer, frm integer, ctime DATETIME DEFAULT CURRENT_TIMESTAMP, msg text, primary key(msg_id), foreign key (uid) references user(uid), foreign key(frm) references user(uid));"
      );
  
  if(create_user("admin", "admin", "admin@root", "tulip!2345"))
    tul_log(" tulip_boot >>>> ERROR: db init error");

#endif
}

void tul_dbclean()
{
#ifndef USE_MYSQL
  for (int i = 0; i < SQLPOOL; i++)
  {
    if(sql_pool[i])
    {
      sqlite3_close(sql_pool[i]);
    }
  }
#endif
}



