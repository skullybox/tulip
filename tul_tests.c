/***
  Copyright (C) irfan
 **/

#include "tul_tests.h"
#include "tul_tcp_soc.h"
#include "tul_globals.h"
#include "tul_pub_key.h"

void _tcp_listen_test()
{
  int _sock = 0;

  if(tul_tcp_listen_init(8080, &_sock))
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

void _rsa_keygen_test()
{
  tul_keypair k;

  if(tul_gen_keypair(&k))
  {
    fprintf(stderr, "FAIL: tul_gen_keypair\n");
    return;
  }
  fprintf(stdout, "PASS: tul_gen_keypair\n");
}

void _rsa_writekey_test()
{
  tul_keypair k;
  tul_gen_keypair(&k);

  if(tul_write_keys(&k, "/tmp/", "key"))
  {
    fprintf(stderr, "FAIL: tul_write_keys\n");
    return;
  }
  fprintf(stdout, "PASS: tul_write_keys\n");


}




