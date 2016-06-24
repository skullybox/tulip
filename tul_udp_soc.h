/***
 Copyright (C) irfan

 Listening TCP socket api
**/

#ifndef _tul_udp_soc
#define _tul_udp_soc
#include "tul_globals.h"

int tul_udp_listen_init(const int port, int *sock, struct sockaddr_in6 *addr);
int tul_udp_connect(const char *host, const int port,
  int *sock, struct sockaddr_in6 *addr);


#endif
