/***
  Copyright (C) irfan
  tulip main.
 **/


#include "tul_tests.h"
#include "tul_testwrap.h"

void run_tests()
{
  _tcp_listen_test();
  _tcp_connect_test();
  _rsa_keygen_test();
  _rsa_writekey_test();
}


