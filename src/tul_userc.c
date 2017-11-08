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

  strncpy(r.user, uid, 30);

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

  memset(&r, 0, REQ_HSZ);
  memset(&p, 0, sizeof(comm_payload));

  strncpy(r.user, uid, 30);

  /* random encryption key and salt */
  tul_random(&(r.salt), 16);
  tul_random(&(r.kek), 16);

  p.action = LOGOUT;

  /* data size with encryption
   * block size into account
   */
  if((strlen(uid)+1)%16)
  {
    p.data_sz = strlen(uid)+1+16;
    PAYLOAD_CHECK_SZ;
    p.data = calloc(strlen(uid)+1+16, 1);
  }
  else
  {
    p.data_sz = strlen(uid)+1;
    PAYLOAD_CHECK_SZ;
    p.data = calloc(strlen(uid)+1,1);
  }

  r.payload_sz = sizeof(comm_payload) + p.data_sz;

  /* data stores uid as part of
   * payload to confirm logout
   */
  strcpy(p.data, uid);

  int ret = prep_transmission(uid, pass, &r, &p, conn);

  if(p.data)
    free(p.data);
  if(ret)
    return ret;

  return 0;
}


int client_message(char *uid, char *t_uid, char *pass, 
    tul_net_context *conn, char *msg, unsigned m_len)
{
  comm_req r;
  comm_payload p;
  char to_user[30] = {0};

  memset(&r, 0, REQ_HSZ);
  memset(&p, 0, sizeof(comm_payload));

  strncpy(r.user, uid, 30);

  /* random encryption key and salt */
  tul_random(&(r.salt), 16);
  tul_random(&(r.kek), 16);

  p.action = SEND_MSG;

  /* message to user */
  strncpy(to_user, t_uid, 30);

  /* data size with encryption
   * block size into account
   *
   * first 30 bytes is target username
   * rest of the bytes are for the message
   */
  if((m_len+1+30)%16)
  {
    p.data_sz = m_len+1+30+16;
    PAYLOAD_CHECK_SZ;
    p.data = calloc(m_len+1+30+16,1);
  }
  else
  {
    p.data_sz = m_len+1+40;
    PAYLOAD_CHECK_SZ;
    p.data = calloc(m_len+1+30,1);
  }

  r.payload_sz = sizeof(comm_payload) + p.data_sz;

  memcpy(p.data, to_user, 30);
  char *tmp = (char*)p.data;
  memcpy(&(tmp[30]), msg, m_len);

  int ret = prep_transmission(uid, pass, &r, &p, conn);

  if(p.data)
    free(p.data);

  if(ret)
    return ret;

  return 0;
}


int client_friend_req(char *uid, char *t_uid, char *pass, tul_net_context *conn)
{
  comm_req r;
  comm_payload p;

  memset(&r, 0, REQ_HSZ);
  memset(&p, 0, sizeof(comm_payload));

  strncpy(r.user, uid, 30);

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
  strcpy(p.data, t_uid);

  int ret = prep_transmission(uid, pass, &r, &p, conn);
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

  memset(&r, 0, REQ_HSZ);
  memset(&p, 0, sizeof(comm_payload));

  strncpy(r.user, uid, 30);

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
  strcpy(p.data, uid);

  int ret = prep_transmission(uid, pass, &r, &p, conn);
  if(ret)
    goto RETURN_ADDREQLIST_ERROR;

  ret = client_transmit(conn);
  if(ret)
    goto RETURN_ADDREQLIST_ERROR;

  /* now get the list */
  ret = client_recieve(conn);
  ret = verify_client_payload(conn, &r, &p, pass);

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

  memset(&r, 0, REQ_HSZ);
  memset(&p, 0, sizeof(comm_payload));

  strncpy(r.user, uid, 30);

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
  strcpy(p.data, uid);
  memcpy(&((char*)p.data)[30], &offset, 8);

  int ret = prep_transmission(uid, pass, &r, &p, conn);
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

    /* 300ms timeout */
    if(bread <= 0)
    {
      current_time = clock() - last_transmission;
      if( current_time/3000 )
        break;
    }

  }

  if(conn->_trecv < conn->_ttrecv || conn->_ttrecv == 0)
  {
    return -1;
  }
  else
    return 0;

}


int client_accept_friend(char *uid, char *pass, tul_net_context *conn, char *f_uid)
{
  comm_req r;
  comm_payload p;

  memset(&r, 0, REQ_HSZ);
  memset(&p, 0, sizeof(comm_payload));

  strncpy(r.user, uid, 30);

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
  strcpy(p.data, f_uid);

  int ret = prep_transmission(uid, pass, &r, &p, conn);
  if(ret)
    return ret;

  ret = client_transmit(conn);
  if(ret)
    return ret;

  /* now get the confirmation */
  ret = 0;
  ret |= client_recieve(conn);
  if(!ret)
    ret |= verify_client_payload(conn, &r, &p, pass);

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

  memset(&r, 0, sizeof(comm_req));
  memset(&p, 0, sizeof(comm_payload));

  ret |= client_recieve(conn);
  if(!ret)
    ret |= verify_client_payload(conn, &r, &p, pass);

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


