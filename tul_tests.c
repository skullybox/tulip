/***
Copyright (C) irfan
**/

#include "tul_tests.h"
#include "tul_tcp_soc.h"
#include "tul_udp_soc.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>


void _tcp_listen_test()
{
  int _sock = 0;

  if(tul_tcp_listen_init("0.0.0.0", 8080, &_sock))
  {
    fprintf(stderr, "FAIL: tcp_listen_test\n");
    return;
  }

  fprintf(stdout, "PASS: tcp_listen_test\n");
  close(_sock);
}

void _tcp_connect_test()
{
  int _sock;

  if(tul_tcp_connect("apple.com", 80, &_sock))
  {
    fprintf(stderr, "FAIL: tcp_connect_test\n");
    return;
  }

  fprintf(stdout, "PASS: tcp_connect_test\n");
  close(_sock);
}

void _udp_listen_test()
{
  int _sock = 0;
  struct sockaddr_in _hints;
  struct sockaddr_in _addr_store;
  socklen_t addr_store_sz = sizeof(_addr_store);
  char buff[2048]={0};
  int ret = 0;

  if(tul_udp_listen_init("0.0.0.0", 8080, &_sock))
  {
    fprintf(stderr, "FAIL: udp_listen_test\n");
    return;
  }

  fprintf(stdout, "PASS: udp_listen_test\n");

  while(1){
    ret = recvfrom(_sock, buff, 2048, 0, (struct sockaddr *) &_addr_store, &addr_store_sz);
    
  }

  close(_sock);
}
