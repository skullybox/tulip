/***
Copyright (C) irfan

Listening TCP socket api
**/

#include "tul_tcp_soc.h"
#include "tul_globals.h"

int tul_tcp_listen_init(const int port, int *sock)
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

  _ret_sock = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
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

  _ret_list = listen(_ret_sock, 512);
  if(_ret_list)
  {
    fprintf(stderr, "SOCKET LISTEN: %s(%d)\n", __FILE__, __LINE__);
    return -1;
  }

  /* set non-blocking */
  #if defined(__APPLE__) || defined(__LINUX__)
    _fd_flags = fcntl(_ret_sock, F_GETFL, 0);
    fcntl(_ret_sock, F_SETFL, _fd_flags | O_NONBLOCK);
  #else
    ULONG NON_BLOCK = 1;
    ioctlsocket(_ret_sock, FIONBIO, &NON_BLOCK);
  #endif

  *sock = _ret_sock;
  return 0;
}

int tul_tcp_connect(const char *host, const int port, int *sock)
{
  struct sockaddr_in6 _serv;
  int _ret_sock = 0;
  int _fd_flags = 0;
  char _ipaddr[INET6_ADDRSTRLEN] = {0};
  struct addrinfo *_addr_result = NULL;
  struct sockaddr_in6 *_addr_ptr = NULL;
  struct addrinfo _hints;
  short timeout = 0;

  _ret_sock = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
  if(_ret_sock < 1)
  {
    fprintf(stderr, "SOCKET ERROR: %s(%d)\n", __FILE__, __LINE__);
    return -1;
  }

  /* get DNS name ***************/
  memset(&_hints, 0, sizeof(_hints));
  _hints.ai_socktype = SOCK_STREAM;
  _hints.ai_family = AF_INET6;
  _hints.ai_flags = _hints.ai_flags | AI_V4MAPPED;

  if (getaddrinfo(host, NULL, &_hints, &_addr_result))
  {
   freeaddrinfo(_addr_result);
   fprintf(stderr, "DNS ERROR: %s(%d)\n", __FILE__, __LINE__);
   return -1;
  }
  _addr_ptr = (struct sockaddr_in6 *) _addr_result->ai_addr;
  inet_ntop(AF_INET6, &(_addr_ptr->sin6_addr), _ipaddr, 40);
  freeaddrinfo(_addr_result);

  if (strlen(_ipaddr) > 0)
  {
    memset(&_serv, 0, sizeof(_serv));
    inet_pton(AF_INET6, _ipaddr, &(_serv.sin6_addr));
    _serv.sin6_port = htons(port);
    _serv.sin6_family = AF_INET6;
  }
  else
  {
    return -1;
  }

  /* socket connect to resource ***************/
  while(connect(_ret_sock, (struct sockaddr *)&_serv, sizeof(_serv)))
  {
    usleep(1000);
    timeout++;

    if(timeout >= 10000)
    {
      fprintf(stderr, "CONNECT TIMEOUT ERROR: %s(%d) - %s\n", __FILE__,
      __LINE__, _ipaddr);
      return -1;
    }
  }

  /* set non-blocking */
#if defined(__APPLE__) || defined(__LINUX__)
  _fd_flags = fcntl(_ret_sock, F_GETFL, 0);
  fcntl(_ret_sock, F_SETFL, _fd_flags | O_NONBLOCK);
#else
  ULONG NON_BLOCK = 1;
  ioctlsocket(_ret_sock, FIONBIO, &NON_BLOCK);
#endif
  *sock = _ret_sock;

  return 0;
}
