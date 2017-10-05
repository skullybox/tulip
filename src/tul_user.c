/***
  Copyright (C) irfan
  user functions
 **/

#include <time.h>
#include <string.h>
#include <stdio.h>
#include "tul_b64.h"
#include "tul_log.h"
#include "tul_db.h"
#include "tul_user.h"
#include "tul_random.h"
#include "rc5_cipher.h"

char dbk[16]="sxUq##X~$ml/<|6";
int user_exists(char *uid)
{
  char ** res;
  int row = 0;
  int col = 0;
  char SQL[4096] = {0};

  sprintf(SQL,
      "select uid from user where uid='%s'", uid);

  tul_query_get(SQL, &res, &row, &col);

  if(row)
    return 1;

  return 0;
}

int email_exists(char *email)
{
  char ** res;
  int row = 0;
  int col = 0;
  char SQL[4096] = {0};

  sprintf(SQL,
      "select email from user where email='%s'", email);

  tul_query_get(SQL, &res, &row, &col);

  if(row)
    return 1;

  return 0;
}

/* creates the user
 * returns:
 * 1 - uid taken
 * 2 - email taken
 * 3 - uid length
 * 4 - name length
 * 5 - email length
 * 6 - pass length
 * 7 - db error
 */
int create_user(char *uid, char *name, char *email, char *pass)
{
    RC5_ctx rc5;
    char SQL[4096] = {0};
    char *tmp = NULL;
    char salt[24] = {0};
    char epass[24] = {0};
    char l_dbk[16] = {0};

    /* if everything looks good
     * we can create the user
     */ 
    if(user_exists(uid))
      return 1;
    if(email_exists(email))
      return 2;
    if(strlen(uid) < 3)
      return 3;
    if(strlen(name) < 3)
      return 4;
    if(strlen(email) < 5)
      return 5;
    if(strlen(pass) < 10)
      return 6;

    /* encrypt password
     */
    tul_random(&salt[0], 16); 
    memcpy(l_dbk, dbk, 16);
    salt_password(l_dbk, salt,16);

    RC5_SETUP(l_dbk, &rc5);
    rc5_encrypt((unsigned*)&pass[0], (unsigned*)&epass[0], &rc5, 16);

    /* take base64 of encrypted password
     */
    tmp = base64_enc(epass, 16);
    strncpy(epass, tmp, strlen(tmp));
    free(tmp);

    /* base64 encode salt
     */
    tmp = base64_enc(salt, 16);
    strncpy(salt, tmp, strlen(tmp));
    free(tmp);

    /* store user
     */
    sprintf(SQL, "BEGIN; insert into user(uid, name, email, password, salt) values ('%s', '%s', '%s','%s','%s'); COMMIT;",
        uid, name, email, epass, salt);

    if(tul_query(1,SQL))
      return 7;
    return 0;
}


