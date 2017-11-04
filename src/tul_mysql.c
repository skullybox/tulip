/***
  Copyright (C) irfan
  mysql client
 **/

#include "tul_log.h"
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
static pthread_mutex_t pool_locks[30];
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
  va_list ap;
  unsigned pos;
  char *q = NULL;
  unsigned ret = 0;
  char buff[2500] = {0};
  unsigned errors_found = 0;

  while(1)
  {
    if(!pthread_mutex_trylock(&pool_locks[pos]))
      break;
    pos++;
    if(pos >= MAX_MYSQL_POOL)
      pos = 0;
  }

  va_start(ap, num_q);
  for(int i = 0; i < num_q; i++)
  {
    q = va_arg(ap, char*);
    sprintf(buff, "%s; ", q);
    ret = mysql_query(conn_pool[pos],"begin transaction");
    ret |= mysql_query(conn_pool[pos], buff);
    ret |= mysql_query(conn_pool[pos],"commit");

    /* stop on error */
    if(ret)
    {
      tul_log("db query error");
      tul_log(buff);
      errors_found = 1;
      pthread_mutex_unlock(&pool_locks[pos]);
      return errors_found;
    }
  }
  va_end(ap);

  pthread_mutex_unlock(&pool_locks[pos]);
  return 0;
}


int tul_query_get(char *SQL, MYSQL_RES **res)
{
  unsigned ret = 0;
  unsigned pos = 0;
  unsigned sz = 0;

  *res = NULL;

  while(1)
  {
    if(!pthread_mutex_trylock(&pool_locks[pos]))
      break;
    pos++;
    if(pos >= MAX_MYSQL_POOL)
      pos = 0;
  }

  ret = mysql_query(conn_pool[pos], SQL);
  if( ret || (!ret && mysql_field_count(conn_pool[pos]) == 0))
  {
    pthread_mutex_unlock(&pool_locks[pos]);
    return 1;
  }

  *res = mysql_store_result(conn_pool[pos]);
  pthread_mutex_unlock(&pool_locks[pos]);
  return 0;
}


