/***
  Copyright (C) irfan
  tulip main.
 **/


#include "tul_tests.h"
#include "tul_testwrap.h"

int RUN_TESTS = 0;
void run_tests()
{
  RUN_TESTS = 1;
  _tcp_listen_test();
  _tcp_connect_test();
  _b64_test();
  _tls_server_test();
  _tls_client_test();
  _tls_rc5_test();
}


