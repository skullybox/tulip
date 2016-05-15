/***
 Copyright (C) irfan
**/

#include "tulip.h"
#include "tul_tcp_soc.h"
#include "tul_globals.h"
#include "tul_signal_handler.h"

#include <stdio.h>
#include <string.h>


int main(int argc, char **argv)
{
  tul_global_signal_handle_init();

  if(argc == 2 && !strcmp(argv[1], "-D"))
  {
    run_tests();
    return 0;
  }

  return 0;
}

void run_tests()
{
  _tcp_listen_test();

}

void _tcp_listen_test()
{
  int ret = 0;
  int _ret_sock = 0;

  ret = tul_tcp_listen_init("0.0.0.0", 8080, &_ret_sock);

  if(ret)
  {
    fprintf(stderr, "FAIL: TCP_LISTEN_TEST\n");
  }
  else
  {
    fprintf(stdout, "PASS: TCP_LISTEN_TEST\n");
  }

}
