/***
Copyright (C) irfan
**/

#include "tul_tests.h"
#include "tul_tcp_soc.h"

#include <stdio.h>
#include <string.h>
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
