/***
  Copyright (C) irfan
  user client functions
 **/

#include "tul_userc.h"
#include "rc5_cipher.h"
#include "tul_tcp_soc.h"
#include "tul_random.h"
#include "whirlpool_hash.h"

/* MAX_REQ_PAYLOAD minus payload
 * header of struct
 */
#define PAYLOAD_CHECK_SZ if(p.data_sz > MAX_REQ_PAYLOAD - sizeof(comm_payload)) return 1;

int prep_transmission(char *uid, char *pass, 
    comm_req *r, comm_payload *p, tul_net_context *conn)
{
  RC5_ctx rc5;
  char tmp[16] = {0};
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
  salt_password(r->kek, r->salt, 16);
  RC5_SETUP(r->kek, &rc5);
  rc5_encrypt((unsigned*)r->kek, (unsigned*)p->data, &rc5, p->data_sz);

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

  p.action = LOGIN;

  /* data size with encryption
   * block size into account
   */
  if((strlen(uid)+1)%16)
  {
    p.data_sz = 16*((strlen(uid)+1)/16)+16;
    PAYLOAD_CHECK_SZ;
    p.data = calloc(1,p.data_sz);
  }
  else
  {
    p.data_sz = ((strlen(uid)+1)/16)*16;
    PAYLOAD_CHECK_SZ;
    p.data = calloc(1,p.data_sz);
  }

  r.payload_sz = sizeof(comm_payload) + p.data_sz;

  /* data stores uid as part of
   * payload to confirm login
   */
  strcpy(p.data, uid);

  prep_transmission(uid, pass, &r, &p, conn);

  return 0;
}


int client_connect(char *host, char *port, tul_net_context *conn)
{
    memset(conn, 0, sizeof(tul_net_context));
    if(tul_tcp_connect(host, atoi(port),&(conn->_sock)))
        return 1;

    /* prep the rest of the socket context */
    conn->_use_tls = 1;

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
    p.data = calloc(1,strlen(uid)+1+16);
  }
  else
  {
    p.data_sz = strlen(uid)+1;
    PAYLOAD_CHECK_SZ;
    p.data = calloc(1,strlen(uid)+1);
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
    p.data = calloc(1,m_len+1+30+16);
  }
  else
  {
    p.data_sz = m_len+1+40;
    PAYLOAD_CHECK_SZ;
    p.data = calloc(1,m_len+1+30);
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
    p.data = calloc(1,strlen(t_uid)+1+16);
  }
  else
  {
    p.data_sz = strlen(t_uid)+1;
    PAYLOAD_CHECK_SZ;
    p.data = calloc(1,strlen(t_uid)+1);
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
    p.data = calloc(1,strlen(uid)+1+16);
  }
  else
  {
    p.data_sz = strlen(uid)+1;
    PAYLOAD_CHECK_SZ;
    p.data = calloc(1,strlen(uid)+1);
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


