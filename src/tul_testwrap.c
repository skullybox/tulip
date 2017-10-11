/***
  Copyright (C) irfan
  tulip main.
 **/


#include "tul_tests.h"
#include "tul_globals.h"
#include "tul_testwrap.h"

int RUN_TESTS = 0;
void run_tests()
{
  RUN_TESTS = 1;
  _rand_test();
  _tcp_listen_test();
  _tcp_connect_test();
  _b64_test();
  _tls_rc5_test();
  _whirlpool_test();
  _payload_limits_test();
  _tls_server_test();
  _tls_client_test();
  _tls_client_test_login();

  printf("sleeping 30s...\n");
  sleep(30);

}


