/***
 Copyright (C) irfan

 Listening TCP socket api
**/

#ifndef _tul_tcp_soc
#define _tul_tcp_soc

int tul_tcp_listen_init(const int port, int *sock);
int tul_tcp_connect(const char *host, const int port, int *sock);

#endif
