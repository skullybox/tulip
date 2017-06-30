/***
  Copyright (C) irfan
  mysql client
 **/

#include "mysql.h"
#include "tul_mysql.h"

#define SQL_HST "db001"
#define SQL_USR "root"
#define SQL_PAS "04d8bpgxdadOlZfIQFw0pm"
#define SQL_DB  "cloud_auth"

#define MAX_MYSQL_POOL 30

static pthread_mutex_t my_mutex;
static unsigned my_inited = 0;
MYSQL *conn_pool[30] = {0};

int db_init()
{
  int ret = 0;
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
    if( (conn_pool[i] = mysql_init(NULL)) == NULL)
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
            3306, NULL, 0) == NULL)
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


int db_query(uchar *q)
{


  return 1;
}

