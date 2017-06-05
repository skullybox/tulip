/***
  Copyright (C) irfan
 **/

#include "tul_b64.h"
#include "tul_tests.h"
#include "tul_tls.h"
#include "tul_tcp_soc.h"
#include "tul_globals.h"

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

void _tls_server_test()
{
  int ret = 0;
  tul_tls_ctx c;

  ret = tls_server_init(&c, 9443);
  tls_server_free(&c);

  if(ret)
  {
    fprintf(stderr, "FAIL: tls_server_test\n");
    return;
  }
  fprintf(stderr, "PASS: tls_server_test\n");
}

void _tls_client_test()
{
  int ret = 0;
  char buf[1025] = {0};
  tul_tls_ctx c;

  strcpy(c.host, "apple.com");
  tls_client_init(&c, 443);
  tls_write(&c, "GET / HTTP/1.1\r\nHost: apple.com\r\n\r\n", 
      strlen("GET / HTTP/1.1\r\nHost: apple.com\r\n\r\n"));
  ret = tls_read(&c, buf, 1024);
  tls_client_free(&c);

  if(!ret)
  {
    fprintf(stderr, "FAIL: tls_client_test\n");
    return;
  }
  fprintf(stderr, "PASS: tls_client_test\n");

}


