/***
  Copyright (C) irfan
  user client functions
 **/

#include <time.h>
#include "tul_userc.h"
#include "tul_tls_client.h"
#include "rc5_cipher.h"
#include "tul_tcp_soc.h"
#include "tul_random.h"
#include "whirlpool_hash.h"
#include "tul_tls_common.h"
#include "tul_module_common.h"

/* MAX_REQ_PAYLOAD minus payload
 * header of struct
 */

extern const char CA_CERT[];
extern const char CLIENT_CERT[];
extern const char CLIENT_KEY[];
extern int CLIENT_CERT_len;
extern int CA_CERT_len;
extern int CLIENT_KEY_len;

int client_login(char *uid, char *pass, tul_net_context *conn)
{
  comm_req r;
  comm_payload p;
  char _uid[30] = {0};
  char _pass[16] = {0};
  
  strncpy(_uid, uid, 30);
  strncpy(_pass, pass, 16);

  memset(&r, 0, REQ_HSZ);
  memset(&p, 0, sizeof(comm_payload));

  strncpy(r.user, _uid, 30);

  /* random encryption key and salt */
  tul_random(&(r.salt), 16);
  tul_random(&(r.kek), 16);

  p.action = LOGIN;

  /* data size with encryption
   * block size into account
   */
  if((strlen(_uid)+1)%16)
  {
    p.data_sz = 16*((strlen(_uid)+1)/16)+16;
    PAYLOAD_CHECK_SZ;
    p.data = calloc(p.data_sz, 1);
  }
  else
  {
    p.data_sz = ((strlen(_uid)+1)/16)*16;
    PAYLOAD_CHECK_SZ;
    p.data = calloc(p.data_sz,1);
  }

  r.payload_sz = sizeof(comm_payload) + p.data_sz;

  /* data stores uid as part of
   * payload to confirm login
   */
  strcpy(p.data, _uid);

  prep_transmission(_uid, _pass, &r, &p, conn);

  if(p.data)
    free(p.data);
  return 0;
}

#define CLEANUP_TLS mbedtls_net_free( &(conn->tls.server_fd) ); \
  mbedtls_x509_crt_free( &(conn->tls.cert) );\
mbedtls_ssl_free( &(conn->tls.ssl) );\
mbedtls_ssl_config_free( &(conn->tls.conf) );\
mbedtls_ctr_drbg_free( &(conn->tls.ctr_drbg) );\
mbedtls_entropy_free( &(conn->tls.entropy) );

int client_connect(char *host, char *port, tul_net_context *conn, int use_tls)
{
  memset(conn, 0, sizeof(tul_net_context));
  if(!use_tls && tul_tcp_connect(host, atoi(port),&(conn->_sock)))
  {
    return 1;
  }
  else if(!use_tls)
  {
    return 0;
  }

  conn->_use_tls = 1;
  int ret = tls_client_init(&conn->tls, 9443);

  if(ret)
  {
    CLEANUP_TLS;
    return 1;
  }

  return 0;

}


int client_logout(char *uid, char *pass, tul_net_context *conn)
{
  comm_req r;
  comm_payload p;
  char _uid[30] = {0};
  char _pass[16] = {0};
  
  strncpy(_uid, uid, 30);
  strncpy(_pass, pass, 16);

  memset(&r, 0, REQ_HSZ);
  memset(&p, 0, sizeof(comm_payload));

  strncpy(r.user, _uid, 30);

  /* random encryption key and salt */
  tul_random(&(r.salt), 16);
  tul_random(&(r.kek), 16);

  p.action = LOGOUT;

  /* data size with encryption
   * block size into account
   */
   if((strlen(_uid)+1)%16)
  {
    p.data_sz = 16*((strlen(_uid)+1)/16)+16;
    PAYLOAD_CHECK_SZ;
    p.data = calloc(p.data_sz, 1);
  }
  else
  {
    p.data_sz = ((strlen(_uid)+1)/16)*16;
    PAYLOAD_CHECK_SZ;
    p.data = calloc(p.data_sz,1);
  }

  r.payload_sz = sizeof(comm_payload) + p.data_sz;

  /* data stores uid as part of
   * payload to confirm login
   */
  strcpy(p.data, _uid);

  prep_transmission(_uid, _pass, &r, &p, conn);

  if(p.data)
    free(p.data);
  return 0;
}

int client_friend_req(char *uid, char *t_uid, char *pass, tul_net_context *conn)
{
  comm_req r;
  comm_payload p;
  char _uid[30] = {0};
  char _tuid[30] = {0};
  char _pass[16] = {0};

  strncpy(_uid, uid, 30);
  strncpy(_tuid, t_uid, 30);
  strncpy(_pass, pass, 16);

  memset(&r, 0, REQ_HSZ);
  memset(&p, 0, sizeof(comm_payload));

  strncpy(r.user, _uid, 30);

  /* random encryption key and salt */
  tul_random(&(r.salt), 16);
  tul_random(&(r.kek), 16);

  p.action = ADDFRIEND;

  /* data size with encryption
   * block size into account
   */
  p.data_sz = 32;
  p.data = calloc(32, 1);

  r.payload_sz = sizeof(comm_payload) + p.data_sz;

  /* data stores t_uid as part of
   * payload to confirm buddy request
   */
  strcpy(p.data, _tuid);

  int ret = prep_transmission(_uid, _pass, &r, &p, conn);
  if(ret)
    goto FRIEND_REQ_ERROR;

  ret = client_transmit(conn);
  if(ret)
    goto FRIEND_REQ_ERROR;

  if(p.data)
    free(p.data);
  return 0;

FRIEND_REQ_ERROR:
  if(p.data)
    free(p.data);
  return ret;
}

int client_get_addreqlist(char *uid, char *pass, tul_net_context *conn, char **list, unsigned *list_sz)
{
  comm_req r;
  comm_payload p;
  char _uid[30] = {0};
  char _pass[16] = {0};
  
  strncpy(_uid, uid, 30);
  strncpy(_pass, pass, 16);

  memset(&r, 0, REQ_HSZ);
  memset(&p, 0, sizeof(comm_payload));

  strncpy(r.user, _uid, 30);

  /* random encryption key and salt */
  tul_random(&(r.salt), 16);
  tul_random(&(r.kek), 16);

  p.action = GET_FREQ;

  /* data size with encryption
   * block size into account
   */
  p.data_sz = 32;
  p.data = calloc(32,1);
  r.payload_sz = sizeof(comm_payload) + p.data_sz;

  /* data stores uid as part of
   * payload to confirm as well as
   * offset value
   */
  strcpy(p.data, _uid);

  int ret = prep_transmission(_uid, _pass, &r, &p, conn);
  if(ret)
    goto RETURN_ADDREQLIST_ERROR;

  ret = client_transmit(conn);
  if(ret)
    goto RETURN_ADDREQLIST_ERROR;

  /* now get the list */
  ret = client_recieve(conn);
  ret = verify_client_payload(conn, &r, &p, _pass);

  if(ret)
    goto RETURN_ADDREQLIST_ERROR;

  if(p.data_sz >= 32)
    *list_sz = (p.data_sz)/30;
  else
    *list_sz = 0;

  if(*list_sz)
  {
    if(*list)
      free(*list);
    *list = calloc(p.data_sz, 1);
    memcpy(*list, p.data, p.data_sz);
  }

  if(p.data)
    free(p.data);
  return 0;

RETURN_ADDREQLIST_ERROR:

  if(p.data)
    free(p.data);
  return ret;
}


int client_get_friendlist(char *uid, char *pass, tul_net_context *conn, char **list,
    unsigned *list_sz, unsigned long long offset)
{
  comm_req r;
  comm_payload p;
  char _uid[30] = {0};
  char _pass[16] = {0};
  
  strncpy(_uid, uid, 30);
  strncpy(_pass, pass, 16);

  memset(&r, 0, REQ_HSZ);
  memset(&p, 0, sizeof(comm_payload));

  strncpy(r.user, _uid, 30);

  /* random encryption key and salt */
  tul_random(&(r.salt), 16);
  tul_random(&(r.kek), 16);

  p.action = GET_LIST;

  /* data size with encryption
   * block size into account
   */
  p.data_sz = 48;
  p.data = calloc(48,1);
  r.payload_sz = sizeof(comm_payload) + p.data_sz;

  /* data stores uid as part of
   * payload to confirm as well as
   * offset value
   */
  strcpy(p.data, _uid);
  memcpy(&((char*)p.data)[30], &offset, 8);

  int ret = prep_transmission(_uid, _pass, &r, &p, conn);
  if(ret)
    goto RETURN_FRIENDLIST_ERROR;

  ret = client_transmit(conn);
  if(ret)
    goto RETURN_FRIENDLIST_ERROR;

  /* now get the list */
  ret = client_recieve(conn);
  ret = verify_client_payload(conn, &r, &p, pass);

  if(ret)
    goto RETURN_FRIENDLIST_ERROR;

  if(p.data_sz >= 38)
    *list_sz = (p.data_sz-8)/30;
  else
    *list_sz = 0;

  if(*list_sz)
  {
    if(*list)
      free(*list);
    *list = calloc(p.data_sz, 1);
    memcpy(*list, p.data, p.data_sz);
  }

  if(p.data)
    free(p.data);
  return 0;

RETURN_FRIENDLIST_ERROR:

  if(p.data)
    free(p.data);
  return ret;
}

int client_transmit(tul_net_context *conn)
{
  int bwrite = 0;
  clock_t last_transmission = clock();
  clock_t current_time;

  while(conn->_tsend < conn->_ttsend)
  {
    if(conn->_use_tls)
    {
      bwrite = tls_write(&(conn->tls),
          &(conn->payload_out[conn->_tsend]),
          conn->_ttsend-conn->_tsend);
      if(bwrite > 0)
      {
        conn->_tsend+=bwrite;
        last_transmission = clock();
      }
    }
    else
    {
      bwrite = write(conn->_sock,
          &(conn->payload_out[conn->_tsend]),
          conn->_ttsend-conn->_tsend);
      if(bwrite > 0)
      {
        conn->_tsend+=bwrite;
        last_transmission = clock();
      }
    }

    /* 3000ms timeout */
    if(bwrite <= 0)
    {
      current_time = clock() - last_transmission;
      if( current_time/3000 )
        break;
    }
  }

  if(conn->_tsend < conn->_ttsend)
    return -1;
  else
    return 0;
}

int client_recieve(tul_net_context *conn)
{
  int bread = 0;
  clock_t last_transmission = clock();
  clock_t current_time;
  comm_resp t_resp;

  memset(&t_resp, 0, sizeof(comm_resp));
  memset(conn->payload_in, 0, DEF_SOCK_BUFF_SIZE);
  conn->_trecv = 0;
  conn->_ttrecv = 0;

  while(conn->_trecv < conn->_ttrecv || conn->_ttrecv == 0)
  {
    if(conn->_trecv >= RES_HSZ)
    {
      memcpy(&t_resp, conn->payload_in, RES_HSZ);
      conn->_ttrecv = RES_HSZ + t_resp.payload_sz;
    }

    if(conn->_use_tls)
    {

      mbedtls_net_set_nonblock(&(conn->tls.server_fd));
      if(conn->_ttrecv == 0)
        bread = tls_read(&(conn->tls),
            &(conn->payload_in[conn->_trecv]),
            RES_HSZ-conn->_trecv);
      else 
        bread = tls_read(&(conn->tls),
            &(conn->payload_in[conn->_trecv]),
            conn->_ttrecv-conn->_trecv);
      if(bread > 0)
      {
        conn->_trecv +=bread;
        last_transmission = clock();
      }
    }
    else
    {
      if(conn->_ttrecv == 0)
        bread = read(conn->_sock, &(conn->payload_out[conn->_trecv]), 
            RES_HSZ-conn->_trecv);
      else 
        bread = read(conn->_sock, &(conn->payload_out[conn->_trecv]), 
            conn->_ttrecv-conn->_trecv);

      if(bread > 0)
      {
        conn->_trecv +=bread;
        last_transmission = clock();
      }
    }

    /* 3s timeout */
    if(bread <= 0)
    {
      current_time = clock() - last_transmission;
      if( current_time/CLOCKS_PER_SEC > 3)
        break;
    }

  }

  if(conn->_trecv < conn->_ttrecv || conn->_ttrecv == 0)
    return -1;
  else
    return 0;

}

int client_ignore_friend(char *uid, char *pass, tul_net_context *conn, char *f_uid)
{
  comm_req r;
  comm_payload p;
  char _uid[30] = {0};
  char _fuid[30] = {0};
  char _pass[16] = {0};

  strncpy(_uid, uid, 30);
  strncpy(_pass, pass, 16);
  strncpy(_fuid, f_uid, 16);

  memset(&r, 0, REQ_HSZ);
  memset(&p, 0, sizeof(comm_payload));

  strncpy(r.user, _uid, 30);

  /* random encryption key and salt */
  tul_random(&(r.salt), 16);
  tul_random(&(r.kek), 16);

  p.action = IGNOREFRIEND;

  /* data size with encryption
   * block size into account
   */
  p.data_sz = 32;
  p.data = calloc(32,1);
  r.payload_sz = sizeof(comm_payload) + p.data_sz;

  /* data stores uid as part of
   * payload to confirm 
   */
  strcpy(p.data, _fuid);

  int ret = prep_transmission(_uid, _pass, &r, &p, conn);
  if(ret)
    return ret;

  ret = client_transmit(conn);
  return ret;
}

int client_accept_friend(char *uid, char *pass, tul_net_context *conn, char *f_uid)
{
  comm_req r;
  comm_payload p;
  char _uid[30] = {0};
  char _fuid[30] = {0};
  char _pass[16] = {0};
  
  strncpy(_uid, uid, 30);
  strncpy(_pass, pass, 16);
  strncpy(_fuid, f_uid, 16);

  memset(&r, 0, REQ_HSZ);
  memset(&p, 0, sizeof(comm_payload));

  strncpy(r.user, _uid, 30);

  /* random encryption key and salt */
  tul_random(&(r.salt), 16);
  tul_random(&(r.kek), 16);

  p.action = ACCEPTFRIEND;

  /* data size with encryption
   * block size into account
   */
  p.data_sz = 32;
  p.data = calloc(32,1);
  r.payload_sz = sizeof(comm_payload) + p.data_sz;

  /* data stores uid as part of
   * payload to confirm 
   */
  strcpy(p.data, _fuid);

  int ret = prep_transmission(_uid, _pass, &r, &p, conn);
  if(ret)
    return ret;

  ret = client_transmit(conn);
  if(ret)
    return ret;

  /* now get the confirmation */
  ret = 0;
  ret |= client_recieve(conn);
  if(!ret)
    ret |= verify_client_payload(conn, &r, &p, _pass);

  if(p.data)
    free(p.data);

  if(ret)
    return ret;

  if(p.action != OK)
    return -1;

  return 0;

}


int client_get_ok(tul_net_context *conn, char *pass) 
{
  /* validate response */
  comm_req r;
  comm_payload p;
  int ret = 0;
  char _pass[16] = {0};

  strncpy(_pass, pass, 16);
  memset(&r, 0, sizeof(comm_req));
  memset(&p, 0, sizeof(comm_payload));

  ret |= client_recieve(conn);
  if(!ret)
    ret |= verify_client_payload(conn, &r, &p, _pass);

  if(p.data)
  {
    free(p.data);
    p.data = NULL;
  }

  if(ret)
    return ret;

  if(p.action != OK)
    return -1;

  return 0;
}

/*
 * message byte setup:
 * 8 bytes message id
 * 4 bytes true / false new flag
 * 3 bytes type (SYS/USR)
 * 30 bytes to user
 * 50 bytes reserved
 * 4 bytes message length
 * message from user
 */

int client_message(char *uid, char *t_uid, char *pass, tul_net_context *conn, 
    char *msg, unsigned m_len)
{
  comm_req r;
  comm_payload p;
  char _uid[30] = {0};
  char _tuid[30] = {0};
  char _pass[16] = {0};

  if(m_len >= MAX_MESSAGE)
    return -1;

  strncpy(_uid, uid, 30);
  strncpy(_pass, pass, 16);
  strncpy(_tuid, t_uid, 16);

  memset(&r, 0, REQ_HSZ);
  memset(&p, 0, sizeof(comm_payload));

  strncpy(r.user, _uid, 30);

  /* random encryption key and salt */
  tul_random(&(r.salt), 16);
  tul_random(&(r.kek), 16);

  p.action = SEND_MSG;

  /* data size with encryption
   * block size into account
   */
  if((m_len+MESSAGE_META_SZ)%16)
    p.data_sz = m_len+MESSAGE_META_SZ+(16-((m_len+MESSAGE_META_SZ)%16));
  else 
    p.data_sz = m_len+MESSAGE_META_SZ;
  
  p.data = calloc(p.data_sz,1);
  r.payload_sz = sizeof(comm_payload) + p.data_sz;

  /* data stores uid as part of
   * payload to confirm 
   */
  memcpy(&((char*)p.data)[12], "USR", 3);
  memcpy(&((char*)p.data)[15], _tuid, strlen(_tuid));
  memcpy(&((char*)p.data)[MESSAGE_META_SZ-4], &m_len, 4);
  memcpy(&((char*)p.data)[MESSAGE_META_SZ], msg, m_len);

  int ret = prep_transmission(_uid, _pass, &r, &p, conn);
  if(ret)
    return ret;

  ret = client_transmit(conn);
  if(ret)
    return ret;

  /* now get the confirmation */
  ret = 0;
  ret |= client_recieve(conn);
  if(!ret)
    ret |= verify_client_payload(conn, &r, &p, _pass);

  if(p.data)
    free(p.data);

  if(ret)
    return ret;

  if(p.action != OK)
    return -1;

  return 0;
}

int client_get_message(char *uid, char *t_uid, char *pass, 
    tul_net_context *conn, unsigned long long offset, char **msg, unsigned long long *ret_offset, unsigned *_new, char *uname, char *frm, char *typ)
{
  comm_req r;
  comm_payload p;
  char _uid[30] = {0};
  char _tuid[30] = {0};
  char _pass[16] = {0};
  unsigned long long _offset = offset;

  if(*msg)
    return -1;

  if(t_uid)
    strncpy(_tuid, t_uid, 30);

  strncpy(_uid, uid, 30);
  strncpy(_pass, pass, 16);

  memset(&r, 0, REQ_HSZ);
  memset(&p, 0, sizeof(comm_payload));

  strncpy(r.user, _uid, 30);

  if(p.data)
    free(p.data);
  p.data = calloc(48,1);
  p.data_sz = 48;
  r.payload_sz = sizeof(comm_payload) + p.data_sz;

  /* random encryption key and salt */
  tul_random(&(r.salt), 16);
  tul_random(&(r.kek), 16);

  p.action = GET_MSG;

  /* data size with encryption
   * block size into account
   */
  memcpy(p.data, &_offset, 8);
  memcpy(&((char*)p.data)[8], &_tuid, 30);

  int ret = prep_transmission(_uid, _pass, &r, &p, conn);
  if(ret)
    return ret;

  ret = client_transmit(conn);
  if(ret)
    return ret;

  ret |= client_recieve(conn);
  if(!ret)
    ret |= verify_client_payload(conn, &r, &p, _pass);

  *msg = calloc(
      (unsigned)((char*)p.data)[95], 1);
  memcpy(*msg, &((char*)p.data)[99], 
      (unsigned)((char*)p.data)[95]);

  memcpy(typ, &((char*)p.data)[12], 3);
  memcpy(uname, &((char*)p.data)[15], 30);
  memcpy(frm, &((char*)p.data)[45], 30);
  memcpy(ret_offset, p.data, 8);
  memcpy(_new, &((char*)p.data)[8], 4);

  return 0;
}


