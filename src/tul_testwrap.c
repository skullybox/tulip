/***
  Copyright (C) irfan
  tulip main.
 **/


#include "tul_log.h"
#include "tul_user.h"
#include "tul_tests.h"
#include "tul_globals.h"
#include "tul_testwrap.h"

int RUN_TESTS = 0;
void run_tests()
{
  RUN_TESTS = 1;
  _payload_limits_test();
  _chat_size_limits_tests();
  _rand_test();
  _tcp_listen_test();
  _tcp_connect_test();
  _b64_test();
  _tls_rc5_test();
  _whirlpool_test();
  _tls_server_test();
  _create_users();
  _tls_client_test_login();
  _login_logout_test();
  _create_friend_requests();
  _test_get_add_reqests();
  _send_friend_accept_tests();
  _test_get_friends();
  _test_msg_send();

  tul_log("\n clean-up...\n");
  _cleanup();

  tul_log("\n waiting 10s for threads ...\n");
  sleep(10);

}


