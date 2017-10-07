/***
  Copyright (C) irfan
  user client functions
 **/


#ifndef _tul_userc
#define _tul_userc

#include "tul_net_context.h"

int client_connect(char *host, char *port, tul_net_context *conn);
int client_login(char *uid, char *pass, tul_net_context *conn);
int client_logout(char *uid, char *pass, tul_net_context *conn);
int client_message(char *uid, char *pass, tul_net_context *conn, char *msg, unsigned m_len);
int client_friend_req(char *uid, char *t_uid, char *pass, tul_net_context *conn);
int client_friend_res(char *uid, char *t_uid, char *pass, tul_net_context *conn);
int client_get_friendlist(char *uid, char *pass, tul_net_context *conn, char *list);

#endif
