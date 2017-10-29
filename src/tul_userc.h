/***
  Copyright (C) irfan
  user client functions
 **/


#ifndef _tul_userc
#define _tul_userc

#include "tul_net_context.h"

#ifdef __cplusplus
extern "C" {
#endif

int client_connect(char *host, char *port, tul_net_context *conn, int use_tls);

/* these functions return 0 when
 * they have succesfully constructed the payload
 */
int client_login(char *uid, char *pass, tul_net_context *conn);
int client_logout(char *uid, char *pass, tul_net_context *conn);
int client_message(char *uid, char *t_uid, char *pass, tul_net_context *conn, 
    char *msg, unsigned m_len);
int client_friend_req(char *uid, char *t_uid, char *pass, tul_net_context *conn);
int client_friend_res(char *uid, char *t_uid, char *pass, tul_net_context *conn);
int client_get_friendlist(char *uid, char *pass, tul_net_context *conn, char **list,
    unsigned *list_sz, unsigned long long offset);

int client_get_addreqlist(char *uid, char *pass, tul_net_context *conn, char **list, unsigned *list_sz);
int client_transmit(tul_net_context *conn);
int client_recieve(tul_net_context *conn);
int client_accept_friend(char *uid, char *pass, tul_net_context *conn, char *f_uid);

#endif

#ifdef __cplusplus
}
#endif
