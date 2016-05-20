/***
Copyright (C) irfan

Listening TCP socket api
**/

#include "tul_udp_soc.h"

#include <sys/fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

int tul_udp_listen_init(const int port, int *sock, struct sockaddr_in *addr)
{
  struct sockaddr_in6 _serv;
  int _ret_sock = 0;
  int _ret_bind = 0;
  int _ret_list = 0;
  int _fd_flags = 0;
  int _reuse = 1;

  memset(&_serv, 0, sizeof(_serv));

  /* bind setup */
  _serv.sin6_family = AF_INET6;
  _serv.sin6_addr = in6addr_any;
  _serv.sin6_port = htons(port);

  _ret_sock = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
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

  memcpy(addr, &_serv, sizeof(struct sockaddr_in));

  /* set non-blocking */
  _fd_flags = fcntl(_ret_sock, F_GETFL, 0);
  fcntl(_ret_sock, F_SETFL, _fd_flags | O_NONBLOCK);
  *sock = _ret_sock;

  return 0;
}

int tul_udp_connect(const char *host, const int port,
  int *sock, struct sockaddr_in *addr)
{
  struct sockaddr_in _serv;
  int _ret_sock = 0;
  int _fd_flags = 0;
  char _ipaddr[19] = {0};
  struct addrinfo *_addr_result = NULL;
  struct sockaddr_in *_addr_ptr = NULL;
  struct addrinfo _hints;
  short timeout = 0;

  _ret_sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if(_ret_sock < 1)
  {
    fprintf(stderr, "SOCKET ERROR: %s(%d)\n", __FILE__, __LINE__);
    return -1;
  }

  /* get DNS name ***************/
  memset(&_hints, 0, sizeof(_hints));
  _hints.ai_family = AF_INET;
  _hints.ai_socktype = SOCK_STREAM;

   if (getaddrinfo(host, NULL, &_hints, &_addr_result))
   {
     freeaddrinfo(_addr_result);
     fprintf(stderr, "DNS ERROR: %s(%d)\n", __FILE__, __LINE__);
     return -1;
   }
   _addr_ptr = (struct sockaddr_in *) _addr_result->ai_addr;
   strncpy(_ipaddr, inet_ntoa(_addr_ptr->sin_addr), 15);
   freeaddrinfo(_addr_result);

  if (strlen(_ipaddr) > 0)
  {
    memset(&_serv, 0, sizeof(_serv));
    _serv.sin_addr.s_addr = inet_addr(_ipaddr);
    _serv.sin_port = htons(port);
    _serv.sin_family = AF_INET;
  }
  else
  {
    return -1;
  }

  memcpy(addr, &_serv, sizeof(struct sockaddr_in));

  /* set non-blocking */
  _fd_flags = fcntl(_ret_sock, F_GETFL, 0);
  fcntl(_ret_sock, F_SETFL, _fd_flags | O_NONBLOCK);
  *sock = _ret_sock;

  return 0;
}
