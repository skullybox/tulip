/***
  Copyright (C) irfan
  user functions
 **/

#ifndef _tul_user
#define _tul_user

int create_user(char *uid, char *name, char *email, char *pass);
int user_exists(char *uid);
int email_exists(char *email);

#endif

