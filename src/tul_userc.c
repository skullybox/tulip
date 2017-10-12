/***
  Copyright (C) irfan
  user client functions
 **/

#include "tul_userc.h"
#include "tul_tls_client.h"
#include "rc5_cipher.h"
#include "tul_tcp_soc.h"
#include "tul_random.h"
#include "whirlpool_hash.h"
#include "tul_tls_common.h"

/* MAX_REQ_PAYLOAD minus payload
 * header of struct
 */
#define PAYLOAD_CHECK_SZ if(p.data_sz > MAX_REQ_PAYLOAD - sizeof(comm_payload)) return 1;

extern const char CA_CERT[];
extern const char CLIENT_CERT[];
extern const char CLIENT_KEY[];
extern int CLIENT_CERT_len;
extern int CA_CERT_len;
extern int CLIENT_KEY_len;

int prep_transmission(char *uid, char *pass, 
    comm_req *r, comm_payload *p, tul_net_context *conn)
{
  RC5_ctx rc5;
  char tmp[16] = {0};
  char *t_data = NULL;
  NESSIEstruct hash;
  unsigned char hash_r[DIGESTBYTES] = {0};

  /* take data hash prior to 
   * encryption
   */
  NESSIEinit(&hash);
  NESSIEadd(p->data, p->data_sz*8, &hash);
  NESSIEfinalize(&hash, hash_r);
  
  /* copy payload hash to payload
   * header
   */
  memcpy(r->hmac, hash_r, DIGESTBYTES);

  /* encrypt payload using kek */
  memcpy(tmp, r->kek, 16);
  salt_password(tmp, r->salt, 16);
  RC5_SETUP(tmp, &rc5);
  t_data = calloc(1, p->data_sz);
  rc5_encrypt((unsigned*)p->data,(unsigned*)t_data, &rc5, p->data_sz);

  memcpy(p->data, t_data, p->data_sz);
  free(t_data);
  t_data = NULL;

  /* take hash of header */
  NESSIEinit(&hash);

  NESSIEinit(&hash);
  NESSIEadd(r->user, 30*8, &hash);
  NESSIEadd((unsigned char *)&(r->salt), 16*8, &hash);

  /* hash kek before kek is encrypted */
  NESSIEadd(r->kek, 16*8, &hash); 

  NESSIEadd(r->hmac, DIGESTBYTES*8, &hash); 
  NESSIEadd((unsigned char *)&(r->payload_sz), 
      sizeof(unsigned)*8, &hash); 

  NESSIEadd((unsigned char *)&(p->action), sizeof(unsigned)*8, &hash); 
  NESSIEadd((unsigned char*)&(p->data_sz), sizeof(unsigned), &hash); 
  NESSIEadd((unsigned char*)&(p->tag), sizeof(unsigned)*8, &hash); 
  NESSIEadd((unsigned char*)p->data, p->data_sz*8, &hash); 
  NESSIEfinalize(&hash, hash_r);

  memcpy(r->hmac2, hash_r, DIGESTBYTES);

  /* encrypt kek */
  memcpy(tmp, pass, 16);
  salt_password(tmp, r->salt, 16);

  RC5_SETUP(tmp, &rc5);
  memset(tmp, 0, 16);
  memcpy(tmp, r->kek, 16);
  rc5_encrypt((unsigned*)tmp, (unsigned *)r->kek, &rc5, 16);
  

  /* copy to network context
   * and clean-up any allocation
   */
  conn->_ttsend = REQ_HSZ + sizeof(comm_payload) + p->data_sz;
  conn->_tsend = 0;

  /* copy header */
  memcpy(&(conn->payload_out[0]), r, REQ_HSZ);

  /* copy payload */
  memcpy(&(conn->payload_out[REQ_HSZ]), p, sizeof(comm_payload));
  memcpy(&(conn->payload_out[REQ_HSZ+sizeof(comm_payload)]), 
      p->data, p->data_sz);
  
  /* clean-up allocation */
  free(p->data);
  p->data = NULL;
  
  memset(r, 0, REQ_HSZ);
  memset(p, 0, sizeof(comm_payload));
  return 0;
}


int client_login(char *uid, char *pass, tul_net_context *conn)
{
  comm_req r;
  comm_payload p;

  memset(&r, 0, REQ_HSZ);
  memset(&p, 0, sizeof(comm_payload));
  
  strncpy(r.user, uid, 30);

  /* random encryption key and salt */
  tul_random(&(r.salt), 16);
  tul_random(&(r.kek), 16);

  memset(r.kek, 0, 16);
  strcpy(r.kek, "apple");

  p.action = LOGIN;

  /* data size with encryption
   * block size into account
   */
  if((strlen(uid)+1)%16)
  {
    p.data_sz = 16*((strlen(uid)+1)/16)+16;
    PAYLOAD_CHECK_SZ;
    p.data = calloc(p.data_sz, 1);
  }
  else
  {
    p.data_sz = ((strlen(uid)+1)/16)*16;
    PAYLOAD_CHECK_SZ;
    p.data = calloc(p.data_sz,1);
  }

  r.payload_sz = sizeof(comm_payload) + p.data_sz;

  /* data stores uid as part of
   * payload to confirm login
   */
  strcpy(p.data, uid);

  prep_transmission(uid, pass, &r, &p, conn);

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
  if((strlen(t_uid)+1)%16)
  {
    p.data_sz = strlen(t_uid)+1+16;
    PAYLOAD_CHECK_SZ;
    p.data = calloc(strlen(t_uid)+1+16,1);
  }
  else
  {
    p.data_sz = strlen(t_uid)+1;
    PAYLOAD_CHECK_SZ;
    p.data = calloc(strlen(t_uid)+1,1);
  }

  r.payload_sz = sizeof(comm_payload) + p.data_sz;

  /* data stores t_uid as part of
   * payload to confirm buddy request
   */
  strcpy(p.data, t_uid);

  int ret = prep_transmission(uid, pass, &r, &p, conn);
  if(ret)
    return ret;

  return 0;

}


int client_get_friendlist(char *uid, char *pass, tul_net_context *conn, char *list)
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
  if((strlen(uid)+1)%16)
  {
    p.data_sz = strlen(uid)+1+16;
    PAYLOAD_CHECK_SZ;
    p.data = calloc(strlen(uid)+1+16,1);
  }
  else
  {
    p.data_sz = strlen(uid)+1;
    PAYLOAD_CHECK_SZ;
    p.data = calloc(strlen(uid)+1,1);
  }

  r.payload_sz = sizeof(comm_payload) + p.data_sz;

  /* data stores uid as part of
   * payload to confirm 
   */
  strcpy(p.data, uid);

  int ret = prep_transmission(uid, pass, &r, &p, conn);
  if(ret)
    return ret;

  return 0;
}

int client_transmit(tul_net_context *conn)
{
  int bwrite = 0;

  while(conn->_tsend < conn->_ttsend)
  {
    if(conn->_use_tls)
    {
      bwrite = tls_write(&(conn->tls),
          &(conn->payload_out[conn->_tsend]),
          conn->_ttsend-conn->_tsend);
      if(bwrite > 0)
        conn->_tsend+=bwrite;
    }
    else
    {
      bwrite = write(conn->_sock,
          &(conn->payload_out[conn->_tsend]),
          conn->_ttsend-conn->_tsend);
      if(bwrite > 0)
        conn->_tsend+=bwrite;

    }
  }

  return 0;
}

