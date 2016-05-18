/***
 Copyright (C) irfan
**/

#include "tulip.h"
#include "tul_tests.h"
#include "tul_globals.h"
#include "tul_signal_handler.h"

#include <string.h>

int main(int argc, char **argv)
{
  if(argc == 2 && !strcmp(argv[1], "-D"))
  {
    run_tests();
    return 0;
  }
  tul_global_signal_handle_init();
  return 0;
}

void run_tests()
{
  _tcp_listen_test();
  _tcp_connect_test();
  _udp_listen_test();

}
