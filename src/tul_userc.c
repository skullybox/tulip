/***
  Copyright (C) irfan
  user client functions
 **/

#include "tul_userc.h"
#include "tul_tcp_soc.h"

int client_login(char *uid, char *pass, tul_net_context *conn)
{

  return 0;
}


int client_connect(char *host, char *port, tul_net_context *conn)
{
    memset(conn, 0, sizeof(tul_net_context));
    if(tul_tcp_connect(host, atoi(port),&(conn->_sock)))
        return 1;

    /* prep the rest of the socket context */
    conn->_use_tls = 1;

    return 0;
}


