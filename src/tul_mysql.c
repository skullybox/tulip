/***
  Copyright (C) irfan
  mysql client
 **/

#include "mysql.h"
#include "tul_mysql.h"

#define SQL_USR "tulip"
#define SQL_PAS "TuliPP455#"
#define SQL_DB "tulip"
#define MAX_MYSQL_POOL 30

static char SQL_HST[100] = {0};
static unsigned SQL_PORT = 0;
static pthread_mutex_t my_mutex;
static unsigned my_inited = 0;
static MYSQL *conn_pool[30] = {0};

int tul_dbinit()
{
  int ret = 0;
  char *_ptr = NULL;
  char SQL_HST[201]={0};
  unsigned SQL_PORT = 0;

  /* grab environment variable values
   * MYSQL_HOST
   * MYSQL_PORT
   */
  _ptr = getenv("MYSQL_HOST");
  if(_ptr)
    strncpy(SQL_HST, _ptr, 200);
  _ptr = getenv("MYSQL_PORT");
  if(_ptr)
    SQL_PORT = atoi(_ptr);


  pthread_mutex_lock(&my_mutex);

  /* already initialized */
  if(my_inited)
  {
    pthread_mutex_unlock(&my_mutex);
    return 0;
  }

  /* initialize structure */
  for(int i = 0; i < MAX_MYSQL_POOL; i++)
  {
    if( (conn_pool[i] = mysql_init(conn_pool[i])) == NULL)
      ret = 1;
  }

  /* attempt connections */
  for(int i = 0; i < MAX_MYSQL_POOL; i++)
  {
    if(conn_pool[i])
    {
      if( mysql_real_connect(
            conn_pool[i],
            SQL_HST, SQL_USR, 
            SQL_PAS, SQL_DB,
            SQL_PORT, NULL, 0) == NULL)
      {
        mysql_close(conn_pool[i]);
        conn_pool[i] = NULL;
        ret = 1;
      }
    }

  }
  my_inited = 1;
  pthread_mutex_unlock(&my_mutex);

  return ret;
}

void tul_dbclean()
{
  /* already initialized */
  if(!my_inited)
    return;

  for(int i = 0; i < MAX_MYSQL_POOL; i++)
  {
    if(conn_pool[i])
      mysql_close(conn_pool[i]);
  }

  my_inited = 0;
}

int tul_query(int num_q, ...)
{
  /*
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

  */

  /*
  va_start(ap, num_q);
  for(int i = 0; i < num_q; i++)
  {
    q = va_arg(ap, char*);
    ret = sqlite3_exec(sql_pool[pos], q, NULL, NULL, NULL);
    if(ret)
      errors_found = 1;
  }
  va_end(ap);
  */

  return 1;
}


int tul_query_get()
{

}
