/***
  Copyright (C) irfan
 **/

#include "tul_b64.h"
#include "tul_tests.h"
#include "rc5_cipher.h"
#include "tul_random.h"
#include "tul_module.h"
#include "tul_tls_common.h"
#include "tul_tls_client.h"
#include "tul_tls_server.h"
#include "whirlpool_hash.h"
#include "tul_listen_thread.h"
#include "tul_tcp_soc.h"
#include "tul_globals.h"
#include "tul_module.h"
#include <assert.h>

void _payload_limits_test()
{
  assert( REQ_SZ < 1024);
  assert( RES_SZ < 1024);
  fprintf(stdout, "PASS: pay_load_limits_test\n");
}

void _tcp_listen_test()
{
  int _sock = 0;

  if(tul_tcp_listen_init(9080, &_sock))
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

void _b64_test()
{

  char t[] = "any carnal pleasure.";
  char u[] = "any carnal pleasure";
  char v[] = "any carnal pleasur";

  char *r = base64_enc(t, strlen(t));

  /* encoding test --- */
  if(strcmp(r, "YW55IGNhcm5hbCBwbGVhc3VyZS4="))
  {
    fprintf(stderr, "FAIL: base64_test\n");
    return;
  }

  free(r);
  r = base64_enc(u, strlen(u));
  if(strcmp(r, "YW55IGNhcm5hbCBwbGVhc3VyZQ=="))
  {
    fprintf(stderr, "FAIL: base64_test\n");
    return;
  }

  free(r); 
  r = base64_enc(v, strlen(v));
  if(strcmp(r, "YW55IGNhcm5hbCBwbGVhc3Vy"))
  {
    fprintf(stderr, "FAIL: base64_test\n");
    return;
  }

  /* decoding test --- */
  free(r); 
  r = base64_dec("YW55IGNhcm5hbCBwbGVhc3VyZS4=", 
      strlen("YW55IGNhcm5hbCBwbGVhc3VyZS4="));
  if(strcmp(r, t))
  {
    fprintf(stderr, "FAIL: base64_test(1)\n");
    return;
  }

  free(r); 
  r = base64_dec("YW55IGNhcm5hbCBwbGVhc3VyZQ==", 
      strlen("YW55IGNhcm5hbCBwbGVhc3VyZQ=="));
  if(strcmp(r, u))
  {
    fprintf(stderr, "FAIL: base64_test(2)\n");
    return;
  }

  free(r); 
  r = base64_dec("YW55IGNhcm5hbCBwbGVhc3Vy", 
      strlen("YW55IGNhcm5hbCBwbGVhc3Vy"));
  if(strcmp(r, v))
  {
    fprintf(stderr, "FAIL: base64_test(3)\n");
    return;
  }

  fprintf(stdout, "PASS: base64_test\n");
}

extern int tls;
void _tls_server_test()
{
  int ret = 0;
  tul_tls_ctx c;

  ret = tls_server_init(&c, 9443);
  tls_server_free(&c);

  run_listener(9443, tls);
  configure_module();

  if(ret)
  {
    fprintf(stderr, "FAIL: tls_server_test\n");
    return;
  }
  fprintf(stdout, "PASS: tls_server_test\n");
}

void _tls_client_test()
{
  int ret = 0;
  char buf[1025] = {0};
  tul_tls_ctx c;

  sleep(1);
  strcpy(c.host, "127.0.0.1");
  ret = tls_client_init(&c, 9443);
  if(ret)
  {
    fprintf(stderr, "FAIL: tls_client_test\n");
    return;
  }

  fprintf(stderr, "PASS: tls_client_test\n");

}

void _tls_rc5_test()
{
  RC5_ctx c;
  unsigned char key[16] = "G57^h20S'yLR3>mG";
  char text[8] = "ooHeiw3a";
  char ct[8] = {0};

  RC5_SETUP(key, &c);
  RC5_ENCRYPT((unsigned *)text, (unsigned *)ct, &c);

  memset(text, 0, 8);
  RC5_DECRYPT((unsigned *)ct, (unsigned *)text, &c);

  if(strncmp(text, "ooHeiw3a", 
  strlen("ooHeiw3a")) == 0)
  {
    fprintf(stdout, "PASS: tls_rc5_test\n");
    return;
  }
  fprintf(stderr, "FAIL: tls_rc5_test\n");
}

void _rand_test()
{
  unsigned check_fail = 0;
  char b[2048] = {0};
  tul_random(&b[0], 2048);

  for(int i = 0; i < 2048; i+=4)
  {
    if( b[i] == b[i+1] == b[i+2] == b[i+3] == 0)
      check_fail = 1;
  }

  if(check_fail)
  {
    fprintf(stderr, "FAIL: random_test\n");
    return;
  }
  fprintf(stdout, "PASS: random_test\n");
}

void _whirlpool_test()
{
  unsigned char test[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
  unsigned char hash_result[DIGESTBYTES] = {0};
  char test_vector[] = "DC37E008CF9EE69B F11F00ED9ABA2690 1DD7C28CDEC066CC 6AF42E40F82F3A1E\n08EBA26629129D8F B7CB57211B9281A6 5517CC879D7B9621 42C65F5A7AF01467";
  char *hash_str = NULL;
  NESSIEstruct hash;

  NESSIEinit(&hash);
  NESSIEadd(test, strlen((char*)test)*8, &hash);
  NESSIEfinalize(&hash, hash_result);

  hashTostring(&hash_str, hash_result);

  if(strcmp(hash_str, test_vector))
  {
    fprintf(stderr, "FAIL: whirlpool_test\n");
    return;
  }

  fprintf(stdout, "PASS: whirlpool_test\n");
}


