/***
Copyright (C) irfan

Listening TCP socket api
**/

#include "tul_udp_soc.h"

#include <sys/fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

int tul_udp_listen_init(const int port, int *sock)
{
  struct sockaddr_in _serv;
  int _ret_sock = 0;
  int _ret_bind = 0;
  int _ret_list = 0;
  int _fd_flags = 0;
  int _reuse = 1;

  memset(&_serv, 0, sizeof(_serv));

  /* bind setup */
  _serv.sin_family = AF_INET;
  _serv.sin_addr.s_addr = htonl(INADDR_ANY);
  _serv.sin_port = htons(port);

  _ret_sock = socket(PF_INET, SOCK_DGRAM, 0);
  if(_ret_sock < 1)
  {
    fprintf(stderr, "SOCKET ERROR: %s(%d)\n", __FILE__, __LINE__);
    return -1;
  }

  #if defined(__linux) | (__FreeBSD__) | (__APPLE__)
  setsockopt(_ret_sock, SOL_SOCKET, SO_REUSEPORT, &_reuse, sizeof(_reuse));
  #endif

  _ret_bind = bind(_ret_sock, (struct sockaddr*)&_serv, sizeof(_serv));
  if(_ret_bind)
  {
    fprintf(stderr, "BIND ERROR: %s(%d)\n", __FILE__, __LINE__);
    return -1;
  }

  /* set non-blocking */
  _fd_flags = fcntl(_ret_sock, F_GETFL, 0);
  fcntl(_ret_sock, F_SETFL, _fd_flags | O_NONBLOCK);
  *sock = _ret_sock;

  return 0;
}
