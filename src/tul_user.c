/***
  Copyright (C) irfan
  user functions
 **/

#include <time.h>
#include <string.h>
#include <stdio.h>
#include "tul_b64.h"
#include "tul_log.h"
#include "tul_mysql.h"
#include "tul_user.h"
#include "tul_random.h"
#include "rc5_cipher.h"

unsigned char dbk[17]="sxUq##X~$ml/<|6.";
int user_exists(char *uid)
{
  MYSQL_RES *res = NULL;
  int row = 0;
  int col = 0;
  char SQL[4096] = {0};

  sprintf(SQL,
      "select uname from user where uname='%s'", uid);

  tul_query_get(SQL, &res);
  if(res == NULL)
    return 0;
  row = mysql_num_rows(res);
  mysql_free_result(res);

  if(row)
    return 1;

  return 0;
}

int friend_in_list(char*uid, char *n_uid)
{
  MYSQL_RES *res = NULL;
  int row = 0;
  int col = 0;
  char SQL[4096] = {0};

  sprintf(SQL, 
      "select uname, friend from friend_list where uname='%s' and friend = '%s'",
      uid, 
      n_uid);

  tul_query_get(SQL, &res);
  if(res == NULL)
    return 1;
  row = mysql_num_rows(res);
  mysql_free_result(res);

  if(row)
    return 1;

  return 0;
}

int friend_request_exists(char *uid, char *n_uid)
{
  MYSQL_RES *res = NULL;
  int row = 0;
  int col = 0;
  char SQL[4096] = {0};

  sprintf(SQL, 
      "select uname, user_from from friend_request where uname='%s' and user_from = '%s'",
      n_uid, 
      uid);

  tul_query_get(SQL, &res);
  if(res == NULL)
    return 1;
  row = mysql_num_rows(res);
  mysql_free_result(res);

  if(row)
    return 1;

  return 0;

}

int email_exists(char *email)
{
  MYSQL_RES *res = NULL;
  int row = 0;
  int col = 0;
  char SQL[4096] = {0};

  sprintf(SQL,
      "select email from user where email='%s'", email);

  tul_query_get(SQL, &res);
  if(res == NULL)
    return 1;
  row = mysql_num_rows(res);
  mysql_free_result(res);

  if(row)
    return 1;

  return 0;
}

/* creates the user
 * returns:
 * 1 - uid taken
 * 2 - email taken
 * 3 - uid length
 * 4 - email length
 * 5 - pass length
 * 6 - db error
 */
int create_user(char *uid, char *email, char *pass)
{
  RC5_ctx rc5;
  unsigned char *tmp = NULL;
  unsigned char SQL[4096] = {0};
  unsigned char salt[25] = {0};
  unsigned char epass[25] = {0};
  unsigned char l_dbk[17] = {0};
  unsigned char _pass[17] = {0};

  /* if everything looks good
   * we can create the user
   */ 
  if(user_exists(uid))
    return 1;
  if(email_exists(email))
    return 2;
  if(strlen(uid) < 3)
    return 3;
  if(strlen(email) < 5)
    return 4;
  if(strlen(pass) < 8)
    return 5;


  strncpy(_pass, pass, 16);

  /* encrypt password
  */
  tul_random(&salt[0], 16); 
  memcpy(l_dbk, dbk, 16);
  salt_password(l_dbk, salt,16);

  RC5_SETUP(l_dbk, &rc5);
  rc5_encrypt((unsigned*)&_pass[0], (unsigned*)&epass[0], &rc5, 16);

  /* take base64 of encrypted password
  */
  tmp = base64_enc(epass, 16);
  memset(epass, 0, 25);
  strncpy(epass, tmp, strlen(tmp));
  free(tmp);

  /* base64 encode salt
  */
  tmp = base64_enc(salt, 16);
  memset(salt, 0, 25);
  strncpy(salt, tmp, strlen(tmp));
  free(tmp);

  /* store user
  */
  sprintf(SQL, "insert into user(uname, email, password, salt) values ('%s', '%s', '%s','%s') ",
      uid, email, epass, salt);

  if(tul_query(1,SQL))
    return 6;
  if(!user_exists(uid))
    return 6;

  return 0;
}

/* retrieves user password from the db
 * returns 0 when found
 * returns 1 when not found
 */
int get_user_pass(char *uid, char *pass, char *salt)
{
  char SQL[4096] = {0};
  int col = 0;
  int row = 0;
  unsigned ret = 0;
  MYSQL_RES *res;
  MYSQL_ROW irow;

  sprintf(SQL, "select salt, password from user where uname='%s'", uid);

  ret = tul_query_get(SQL, &res);
  if(res == NULL)
    return 1;
  if(!ret)
    row = mysql_num_rows(res);
  if(!ret && row == 1)
  {
    irow = mysql_fetch_row(res);
    strcpy(salt, irow[0]);
    strcpy(pass, irow[1]);
    mysql_free_result(res);
    return 0;
  }

  mysql_free_result(res);
  return 1;
}


void decrypt_user_pass(char *pass, char *salt)
{
  RC5_ctx rc5;
  char *_t = NULL;
  unsigned char l_dbk[17] = {0};
  unsigned char l_salt[25] = {0};
  unsigned char d_pass[25] = {0};

  /* convert from base64 */
  _t = base64_dec(salt, strlen(salt));
  memset(salt, 0, 25);
  memcpy(salt, _t, 16);
  free(_t);

  _t = base64_dec(pass, strlen(pass));
  memset(pass, 0, 25);
  memcpy(pass, _t, 16);
  free(_t);

  memcpy(l_dbk, dbk, 16);
  salt_password(l_dbk, salt,16);

  RC5_SETUP(l_dbk, &rc5);
  rc5_decrypt((unsigned*)&pass[0], (unsigned*)&d_pass[0], &rc5, 16);

  memset(pass, 0, 25);
  memcpy(pass, d_pass, 16);

}

