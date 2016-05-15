/***
Copyright (C) irfan

Listening TCP socket api
**/

#include "tul_tcp_soc.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>

int tul_tcp_listen_init(char const *host, const int port, int *sock)
{
  struct sockaddr_in _serv;
  int _ret_sock = 0;
  int _ret_bind = 0;
  int _ret_list = 0;

  memset(&_serv, 0, sizeof(_serv));

  /* bind setup */
  _serv.sin_family = AF_INET;
  _serv.sin_addr.s_addr = inet_addr(host);
  _serv.sin_port = port;

  _ret_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  if(_ret_sock < 1)
  {
    fprintf(stderr, "SOCKET ERROR: %s(%d)\n", __FILE__, __LINE__);
    return -1;
  }

  _ret_bind = bind(_ret_sock, (struct sockaddr*)&_serv, sizeof(_serv));
  if(_ret_bind)
  {
    fprintf(stderr, "BIND ERROR: %s(%d)\n", __FILE__, __LINE__);
    return -1;
  }

  _ret_list = listen(_ret_sock, 512);
  if(_ret_list)
  {
    fprintf(stderr, "SOCKET LISTEN: %s(%d)\n", __FILE__, __LINE__);
    return -1;
  }

  *sock = _ret_sock;

  return 0;
}
