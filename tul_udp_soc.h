/***
 Copyright (C) irfan

 Listening TCP socket api
**/

#ifndef _tul_udp_soc
#define _tul_udp_soc

#include <netdb.h>

int tul_udp_listen_init(const int port, int *sock, struct sockaddr_in *addr);
int tul_udp_connect(const char *host, const int port,
  int *sock, struct sockaddr_in *addr);


#endif
