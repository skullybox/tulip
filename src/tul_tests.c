/***
  Copyright (C) irfan
 **/

#include "tul_mysql.h"
#include "tul_user.h"
#include "tul_b64.h"
#include "tul_userc.h"
#include "tul_tests.h"
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

void _chat_size_limits_tests()
{
  fprintf(stdout, " max chat size: %d\n", MAX_MESSAGE);
  assert( MAX_MESSAGE < DEF_SOCK_BUFF_SIZE);
  assert( (MAX_MESSAGE % 16) == 0);
  fprintf(stdout, " PASS: chat_size_limits_test\n");
}


void _payload_limits_test()
{
  fprintf(stdout, " max payload: %d\n", MAX_REQ_PAYLOAD);
  assert( REQ_HSZ == RES_HSZ);
  assert( REQ_HSZ < DEF_SOCK_BUFF_SIZE);
  assert( (MAX_REQ_PAYLOAD % 16) == 0);
  fprintf(stdout, " PASS: pay_load_limits_test\n");
}

void _tcp_listen_test()
{
  int _sock = 0;

  if(tul_tcp_listen_init(9080, &_sock))
  {
    fprintf(stderr, " FAIL: tcp_listen_test\n");
    return;
  }

  fprintf(stdout, " PASS: tcp_listen_test\n");
  close(_sock);
}

void _tcp_connect_test()
{
  int _sock;

  if(tul_tcp_connect("apple.com", 80, &_sock))
  {
    fprintf(stderr, " FAIL: tcp_connect_test\n");
    return;
  }

  fprintf(stdout, " PASS: tcp_connect_test\n");
  close(_sock);
}

void _b64_test()
{

  char t[] = "any carnal pleasure.";
  char u[] = "any carnal pleasure";
  char v[] = "any carnal pleasur";
  char w[] = "N*5-/uw_I(Ye91},)";

  char *r = base64_enc(t, strlen(t));

  /* encoding test --- */
  r = base64_enc(t, strlen(t));
  if(strcmp(r, "YW55IGNhcm5hbCBwbGVhc3VyZS4="))
  {
    fprintf(stderr, " FAIL: base64_test\n");
    return;
  }

  free(r);
  r = base64_enc(u, strlen(u));
  if(strcmp(r, "YW55IGNhcm5hbCBwbGVhc3VyZQ=="))
  {
    fprintf(stderr, " FAIL: base64_test\n");
    return;
  }

  free(r); 
  r = base64_enc(v, strlen(v));
  if(strcmp(r, "YW55IGNhcm5hbCBwbGVhc3Vy"))
  {
    fprintf(stderr, " FAIL: base64_test\n");
    return;
  }

  free(r); 
  r = base64_enc(w, strlen(w));
  if(strcmp(r, "Tio1LS91d19JKFllOTF9LCk="))
  {
    fprintf(stderr, " FAIL: base64_test\n");
    return;
  }

  /* decoding test --- */
  free(r); 
  r = base64_dec("YW55IGNhcm5hbCBwbGVhc3VyZS4=", 
      strlen("YW55IGNhcm5hbCBwbGVhc3VyZS4="));
  if(strcmp(r, t))
  {
    fprintf(stderr, " FAIL: base64_test(1)\n");
    return;
  }

  free(r); 
  r = base64_dec("YW55IGNhcm5hbCBwbGVhc3VyZQ==", 
      strlen("YW55IGNhcm5hbCBwbGVhc3VyZQ=="));
  if(strcmp(r, u))
  {
    fprintf(stderr, " FAIL: base64_test(2)\n");
    return;
  }

  free(r); 
  r = base64_dec("YW55IGNhcm5hbCBwbGVhc3Vy", 
      strlen("YW55IGNhcm5hbCBwbGVhc3Vy"));
  if(strcmp(r, v))
  {
    fprintf(stderr, " FAIL: base64_test(3)\n");
    return;
  }
  free(r); 

  fprintf(stdout, " PASS: base64_test\n");
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

  tul_dbinit();

  if(ret)
  {
    fprintf(stderr, " FAIL: tls_server_test\n");
    return;
  }
  fprintf(stdout, " PASS: tls_server_test\n");
  sleep(1);
}

void _tls_rc5_test()
{
  RC5_ctx c;
  unsigned char key[16] = "G57^h20S'yLR3>mG";
  char text[16] = "ooHeiw3a";
  char ct[16] = {0};

  RC5_SETUP(key, &c);
  rc5_encrypt((unsigned*)text, (unsigned*)ct, &c, 16);

  memset(text, 0, 8);
  rc5_decrypt((unsigned*)ct, (unsigned*)text, &c, 16);

  if(strcmp(text, "ooHeiw3a") == 0)
  {
    fprintf(stdout, " PASS: tls_rc5_test\n");
    return;
  }
  fprintf(stderr, " FAIL: tls_rc5_test\n");
}

void _rand_test()
{
  unsigned check_fail = 0;
  char b[2048] = {0};
  tul_random(&b[0], 2048);

  for(int i = 0; i < 2048; i+=4)
  {
    if( b[i] == 0 && b[i+1] == 0 && 
        b[i+2] == 0 && b[i+3] == 0)
      check_fail = 1;
  }

  if(check_fail)
  {
    fprintf(stderr, " FAIL: random_test\n");
    return;
  }
  fprintf(stdout, " PASS: random_test\n");
}

void _whirlpool_test()
{
  unsigned char test[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
  unsigned char hash_result[DIGESTBYTES] = {0};
  char test_vector[] = "DC37E008CF9EE69B F11F00ED9ABA2690 1DD7C28CDEC066CC 6AF42E40F82F3A1E 08EBA26629129D8F B7CB57211B9281A6 5517CC879D7B9621 42C65F5A7AF01467";
  char *hash_str = NULL;
  NESSIEstruct hash;

  NESSIEinit(&hash);
  NESSIEadd(test, strlen((char*)test)*8, &hash);
  NESSIEfinalize(&hash, hash_result);

  hashTostring(&hash_str, hash_result);

  if(strcmp(hash_str, test_vector))
  {
    fprintf(stderr, " FAIL: whirlpool_test\n");
    return;
  }

  fprintf(stdout, " PASS: whirlpool_test\n");
}

void _tls_client_test_login()
{
  int ret = 1;
  tul_net_context conn;
  char uid[30] = "t_admin";
  char pass[16] = "tulip!2345";

  conn._use_tls = 1;
  strcpy(conn.tls.host, "127.0.0.1");
  while(ret)
  {
    ret = tls_client_init(&conn.tls, 9443);
    if(ret)
    {
      fprintf(stdout, " FAIL: client_test_login - TLS ERROR - %d\n", ret);
    }
  }

  if(client_login(uid, pass, &conn))
  {
    fprintf(stdout, " FAIL: client_test_login\n");
    return;
  }

  ret |= client_transmit(&conn);
  if(!ret)
    ret |= client_get_ok(&conn, pass);

  if(ret)
  {
    fprintf(stdout, " FAIL: client_test_login\n");
    return;
  }
  else {
    fprintf(stdout, " PASS: client_test_login\n");
  }

}

void _create_users()
{
  int ret = 0; 

  ret |= create_user("t_admin", "root@project-tulip.org", "tulip!2345");
  ret |= create_user("tommychuckles", "tommy@chuckles.ca", "chuckles123");
  ret |= create_user("salma99", "salma@latin.ca", "salma123");
  ret |= create_user("tiacarrer", "tiaC@hotmail.ca", "tia12345");


  if(ret)
    fprintf(stdout, " FAIL: create_user_test\n");
  else 
    fprintf(stdout, " PASS: create_user_test\n");

}

void _create_friend_requests()
{
  int ret = 1;
  tul_net_context conn;
  char *list = NULL;
  unsigned list_sz = 0;
  char uid[30] = "t_admin";
  char pass[16] = "tulip!2345";

  conn._use_tls = 1;
  strcpy(conn.tls.host, "127.0.0.1");
  while(ret)
  {
    ret = tls_client_init(&conn.tls, 9443);
    if(ret)
    {
      fprintf(stdout, " FAIL: create_friend_requests_test - TLS ERROR - %d\n", ret);
    }
  }

  if(client_login(uid, pass, &conn))
  {
    fprintf(stdout, " FAIL: create_friend_requests_test - login error\n");
    return;
  }

  ret = 0;
  ret |= client_friend_req(uid, "salma99", pass, &conn);
  ret |= client_recieve(&conn);

  ret |= client_friend_req(uid, "tiacarrer", pass, &conn);
  ret |= client_recieve(&conn);

  ret |= client_friend_req(uid, "tommychuckles", pass, &conn);
  ret |= client_recieve(&conn);

  if(ret)
  {
    fprintf(stdout, " FAIL: create_friend_requests_test - login error\n");
    return;
  }

  fprintf(stdout, " PASS: create_friend_requests_test\n");
  sleep(2);
}

void _test_get_friends()
{
  int ret = 1;
  tul_net_context conn;
  char *list = NULL;
  unsigned list_sz = 0;
  char uid[30] = "t_admin";
  char pass[16] = "tulip!2345";

  conn._use_tls = 1;
  strcpy(conn.tls.host, "127.0.0.1");
  while(ret)
  {
    ret = tls_client_init(&conn.tls, 9443);
    if(ret)
    {
      fprintf(stdout, " FAIL: get_friends_test - TLS ERROR - %d\n", ret);
    }
  }

  if(client_login(uid, pass, &conn))
  {
    fprintf(stdout, " FAIL: get_friends_test - login error\n");
    return;
  }

  ret = 0;
  ret |= client_get_friendlist(uid, pass, &conn, &list, &list_sz, 0);

  if(list_sz > 0)
    fprintf(stdout, " PASS: get_friends_test\n");
  else
    fprintf(stdout, " FAIL: get_friends_test\n");

  fprintf(stdout, " friends:\n");
  for(int i = 0; i < 30*list_sz; i+=30)
  {
    fprintf(stdout, "  >%s\n", &list[8+i]);
  }
  free(list);
}


void _test_get_add_reqests()
{
  int ret = 1;
  tul_net_context conn;
  char *list = NULL;
  unsigned list_sz = 0;
  char uid[30] = "salma99";
  char pass[16] = "salma123";

  conn._use_tls = 1;
  strcpy(conn.tls.host, "127.0.0.1");
  while(ret)
  {
    ret = tls_client_init(&conn.tls, 9443);
    if(ret)
    {
      fprintf(stdout, " FAIL: get_add_request_test - TLS ERROR - %d\n", ret);
    }
  }

  if(client_login(uid, pass, &conn))
  {
    fprintf(stdout, " FAIL: get_add_request_test - login error\n");
    return;
  }

  ret = 0;
  ret |= client_get_addreqlist(uid, pass, &conn, &list, &list_sz);


  if(list_sz > 0)
    fprintf(stdout, " PASS: get_add_request_test\n");
  else
    fprintf(stdout, " FAIL: get_add_request_test\n");

  fprintf(stdout, " add friend requests:\n");
  for(int i = 0; i < 30*list_sz; i+=30)
  {
    fprintf(stdout, "  >%s\n", &list[i]);
  }
  free(list);

}

void _send_friend_accept_tests()
{
  int ret = 0;
  int cret = 1;
  tul_net_context conn1;
  tul_net_context conn2;
  tul_net_context conn3;
  char *list = NULL;
  unsigned list_sz = 0;
  char uid[30] = "t_admin";
  char pass[16] = "tulip!2345";

  conn1._use_tls = 1;
  strcpy(conn1.tls.host, "127.0.0.1");
  conn2._use_tls = 1;
  strcpy(conn2.tls.host, "127.0.0.1");
  conn3._use_tls = 1;
  strcpy(conn3.tls.host, "127.0.0.1");
  
  //salma
  while(cret)
  {
    cret = tls_client_init(&conn1.tls, 9443);
    if(cret)
    {
      fprintf(stdout, " FAIL: send_friend_accept_test - TLS ERROR - %d\n", cret);
    }
  }

  if(client_login("salma99", "salma123", &conn1))
  {
    fprintf(stdout, " FAIL: send_friend_accept_test - login error\n");
    return;
  }
  ret |= client_transmit(&conn1);
  if(!ret)
    ret |= client_get_ok(&conn1, "salma123");


  ret |= client_accept_friend("salma99", "salma123", &conn1, "t_admin");
  ret |= client_logout("salma99", "salma123", &conn1);
  ret |= client_transmit(&conn1);
  close(conn1.tls.server_fd.fd);
  cret = 1;

  //tiacarrer
  while(cret)
  {
    cret = tls_client_init(&conn2.tls, 9443);
    if(cret)
    {
      fprintf(stdout, " FAIL: send_friend_accept_test - TLS ERROR - %d\n", cret);
    }
  }

  if(client_login("tiacarrer", "tia12345", &conn2))
  {
    fprintf(stdout, " FAIL: send_friend_accept_test - login error\n");
    return;
  }
  ret |= client_transmit(&conn2);
  if(!ret)
    ret |= client_get_ok(&conn2, "tia12345");


  ret |= client_accept_friend("tiacarrer", "tia12345", &conn2, "t_admin");
  ret |= client_logout("tiacarrer", "tia12345", &conn2);
  ret |= client_transmit(&conn2);
  close(conn2.tls.server_fd.fd);
  cret = 1;

  //tommy
  while(cret)
  {
    cret = tls_client_init(&conn3.tls, 9443);
    if(cret)
    {
      fprintf(stdout, " FAIL: send_friend_accept_test - TLS ERROR - %d\n", cret);
    }
  }

  if(client_login("tommychuckles", "chuckles123", &conn3))
  {
    fprintf(stdout, " FAIL: send_friend_accept_test - login error\n");
    return;
  }
  ret |= client_transmit(&conn3);
  if(!ret)
    ret |= client_get_ok(&conn3, "chuckles123");


  ret |= client_accept_friend("tommychuckles", "chuckles123", &conn3, "t_admin");
  ret |= client_logout("tommychuckles", "chuckles123", &conn3);
  ret |= client_transmit(&conn3);
  close(conn3.tls.server_fd.fd);

  if(ret)
    fprintf(stdout, " FAIL: send_friend_accept_test\n");
  else
    fprintf(stdout, " PASS: send_friend_accept_test\n");
}

void _login_logout_test()
{
  int ret = 1;
  tul_net_context conn;
  char uid[30] = "t_admin";
  char pass[16] = "tulip!2345";

  conn._use_tls = 1;
  strcpy(conn.tls.host, "127.0.0.1");
  while(ret)
  {
    ret = tls_client_init(&conn.tls, 9443);
    if(ret)
    {
      fprintf(stdout, " FAIL: client_test_login - TLS ERROR - %d\n", ret);
    }
  }

  if(client_login(uid, pass, &conn))
  {
    fprintf(stdout, " FAIL: client_test_login_logout\n");
    return;
  }

  ret |= client_transmit(&conn);
  if(!ret)
    ret |= client_get_ok(&conn, pass);

  if(!ret)
  {
    ret |= client_logout(uid, pass, &conn);
    ret |= client_transmit(&conn);
  }

  if(ret)
  {
    fprintf(stdout, " FAIL: client_test_logout\n");
    return;
  }
  else {
    fprintf(stdout, " PASS: client_test_logout\n");
  }


}

void _test_msg_send()
{
  int ret = 0;
  int cret = 1;
  tul_net_context conn1;
  char *list = NULL;
  unsigned list_sz = 0;
  char uid[30] = "t_admin";
  char pass[16] = "tulip!2345";

  conn1._use_tls = 1;
  strcpy(conn1.tls.host, "127.0.0.1");

  while(cret)
  {
    cret = tls_client_init(&conn1.tls, 9443);
    if(cret)
    {
      fprintf(stdout, " FAIL: msg_send_test - TLS ERROR - %d\n", cret);
    }
  }

  if(client_login(uid, pass, &conn1))
  {
    fprintf(stdout, " FAIL: msg_send_test - login error\n");
    return;
  }
  ret |= client_transmit(&conn1);
  if(!ret)
    ret |= client_get_ok(&conn1, pass);

  ret |= client_message(
      uid, "salma99", pass, &conn1, "hello there!", strlen("hello there!"));

  if(!ret)
  {
      fprintf(stdout, " PASS: msg_send_test \n");
  }
  else {
      fprintf(stdout, " FAIL: msg_send_test \n");
  }


}

void _test_msg_get()
{
  int ret = 0;
  int cret = 1;
  tul_net_context conn1;
  char *list = NULL;
  unsigned list_sz = 0;
  char uid[30] = "salma99";
  char pass[16] = "salma123";
  char *MESG1 = NULL;
  char *MESG2 = NULL;

  conn1._use_tls = 1;
  strcpy(conn1.tls.host, "127.0.0.1");

  while(cret)
  {
    cret = tls_client_init(&conn1.tls, 9443);
    if(cret)
    {
      fprintf(stdout, " FAIL: msg_get_test - TLS ERROR - %d\n", cret);
    }
  }

  if(client_login(uid, pass, &conn1))
  {
    fprintf(stdout, " FAIL: msg_get_test - login error\n");
    return;
  }
  ret |= client_transmit(&conn1);
  if(!ret)
    ret |= client_get_ok(&conn1, pass);

  ret |= client_get_message(uid, "", pass, &conn1, 0, &MESG1);
  ret |= client_get_message(uid, "t_admin", pass, &conn1, 0, &MESG2);

  if(!ret && MESG1 && MESG2)
  {
    fprintf(stdout, " PASS: msg_get_test \n");
  }
  else {
    fprintf(stdout, " FAIL: msg_get_test \n");
  }

  if(MESG1)
    free(MESG1);
  if(MESG2)
    free(MESG2);

  fprintf(stdout, "MSG(S): %s\n%s\n", MESG1, MESG2);

}

void _cleanup()
{
  sleep(10);
  tul_query(8,
      "delete from friend_list where friend='t_admin'",
      "delete from friend_list where uname='t_admin'",
      "delete from friend_request where user_from='t_admin'",
      "delete from message where uname='salma99'",
      "delete from user where uname = 'salma99'",
      "delete from user where uname = 'tiacarrer'",
      "delete from user where uname = 'tommychuckles'",
      "delete from user where uname = 't_admin'"
      );
  tul_dbclean();
  TUL_SIGNAL_INT = 1;
}


