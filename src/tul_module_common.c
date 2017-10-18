/***
  Copyright (C) irfan
  common module functions
 **/

#include "rc5_cipher.h"
#include "tul_module_common.h"

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

/* verifies payload with decryption and
 * hmac verfication.
 *
 * return 0 on successfully validating payload
 * return 1 on failure to validate payload
 * return -1 when waiting on more data
 */
int verify_client_payload(tul_net_context *c, comm_req *r, comm_payload *p, char *pass)
{
  RC5_ctx rc5;
  int ret = 0;
  char t_kek[16] = {0};
  char *t_salt = 0;
  char uid[31] = {0};
  char c_pass[25] = {0};
  char salt[25] = {0};
  char *t_data = NULL;
  NESSIEstruct hash;
  unsigned char hash_r[DIGESTBYTES] = {0};

  if(c->_trecv < RES_HSZ)
    return -1;

  if(!c->_ttrecv)
  {
    comm_req r_tmp;
    memcpy(&r_tmp, c->payload_in, RES_HSZ);
    c->_ttrecv = RES_HSZ + r_tmp.payload_sz;
  }

  /* at this point the buffere is filled
   * with the expected payload we can
   * inspect the request sent
   */
  if(c->_trecv < c->_ttrecv)
    return -1;
  
  /* grab the request header */
  memset(r, 0, RES_HSZ);
  memcpy(r, c->payload_in, RES_HSZ);

  /* lookup the user and password */ 
  strncpy(uid, r->user, 30);

  strcpy(c_pass, pass);

  /* decrypt kek */
  salt_password(c_pass, r->salt, 16);

  RC5_SETUP(c_pass, &rc5);
  rc5_decrypt((unsigned*)r->kek, (unsigned *)t_kek, &rc5, 16);
  memcpy(r->kek, t_kek, 16);

  /* decrypt payload */
  memcpy(p, &(c->payload_in[RES_HSZ]), sizeof(comm_payload));
  if(p->data)
    free(p->data);
  p->data = calloc(1, p->data_sz);
  t_data = calloc(1, p->data_sz);

  memcpy(t_data, &(c->payload_in[RES_HSZ+sizeof(comm_payload)]), 
      p->data_sz);

  salt_password(t_kek, r->salt, 16);
  RC5_SETUP(t_kek, &rc5);
  rc5_decrypt((unsigned*)t_data, (unsigned*)p->data, &rc5, p->data_sz);

  free(t_data);
  t_data = NULL;

  /* verify header hash */
  NESSIEinit(&hash);
  NESSIEadd(r->user, 30*8, &hash);
  NESSIEadd((unsigned char *)&(r->salt), 16*8, &hash);
  NESSIEadd(r->kek, 16*8, &hash); 
  NESSIEadd(r->hmac, DIGESTBYTES*8, &hash); 
  NESSIEadd((unsigned char *)&(r->payload_sz), 
      sizeof(unsigned)*8, &hash); 
  NESSIEadd((unsigned char *)&(p->action), sizeof(unsigned)*8, &hash); 
  NESSIEadd((unsigned char*)&(p->data_sz), sizeof(unsigned), &hash); 

  NESSIEadd((unsigned char*)&(c->payload_in[RES_HSZ+sizeof(comm_payload)]), 
      p->data_sz*8, 
      &hash); 
  NESSIEfinalize(&hash, hash_r);

  if (verifyHash(hash_r, r->hmac2 ))
  {
    return 1;
  }

  /* hash payload and verify */
  NESSIEinit(&hash);
  NESSIEadd(p->data, p->data_sz*8, &hash);
  NESSIEfinalize(&hash, hash_r);

  if (verifyHash(hash_r, r->hmac ))
  {
    return 1;
  }

  return 0;
}

/* verifies payload with decryption and
 * hmac verfication.
 *
 * return 0 on successfully validating payload
 * return 1 on failure to validate payload
 * return -1 when waiting on more data
 */
int verify_payload(tul_net_context *c, comm_req *r, comm_payload *p)
{
  RC5_ctx rc5;
  int ret = 0;
  char t_kek[16] = {0};
  char *t_salt = 0;
  char uid[31] = {0};
  char pass[25] = {0};
  char salt[25] = {0};
  char *t_data = NULL;
  NESSIEstruct hash;
  unsigned char hash_r[DIGESTBYTES] = {0};

  if(c->_trecv < REQ_HSZ)
    return -1;

  if(!c->_ttrecv)
  {
    comm_req r_tmp;
    memcpy(&r_tmp, c->payload_in, REQ_HSZ);
    c->_ttrecv = REQ_HSZ + r_tmp.payload_sz;
  }

  /* at this point the buffere is filled
   * with the expected payload we can
   * inspect the request sent
   */
  if(c->_trecv < c->_ttrecv)
    return -1;
  
  /* grab the request header */
  memset(r, 0, REQ_HSZ);
  memcpy(r, c->payload_in, REQ_HSZ);

  /* lookup the user and password */ 
  strncpy(uid, r->user, 30);
  ret = get_user_pass(uid, pass, salt);
  if(ret)
  {
    return 1;
  }

  decrypt_user_pass(pass, salt);

  /* decrypt kek */
  salt_password(pass, r->salt, 16);

  RC5_SETUP(pass, &rc5);
  rc5_decrypt((unsigned*)r->kek, (unsigned *)t_kek, &rc5, 16);
  memcpy(r->kek, t_kek, 16);

  /* decrypt payload */
  memcpy(p, &(c->payload_in[REQ_HSZ]), sizeof(comm_payload));
  p->data = calloc(1, p->data_sz);
  t_data = calloc(1, p->data_sz);

  memcpy(t_data, &(c->payload_in[REQ_HSZ+sizeof(comm_payload)]), 
      p->data_sz);

  salt_password(t_kek, r->salt, 16);
  RC5_SETUP(t_kek, &rc5);
  rc5_decrypt((unsigned*)t_data, (unsigned*)p->data, &rc5, p->data_sz);

  free(t_data);
  t_data = NULL;

  /* verify header hash */
  NESSIEinit(&hash);
  NESSIEadd(r->user, 30*8, &hash);
  NESSIEadd((unsigned char *)&(r->salt), 16*8, &hash);
  NESSIEadd(r->kek, 16*8, &hash); 
  NESSIEadd(r->hmac, DIGESTBYTES*8, &hash); 
  NESSIEadd((unsigned char *)&(r->payload_sz), 
      sizeof(unsigned)*8, &hash); 
  NESSIEadd((unsigned char *)&(p->action), sizeof(unsigned)*8, &hash); 
  NESSIEadd((unsigned char*)&(p->data_sz), sizeof(unsigned), &hash); 

  NESSIEadd((unsigned char*)&(c->payload_in[REQ_HSZ+sizeof(comm_payload)]), 
      p->data_sz*8, 
      &hash); 
  NESSIEfinalize(&hash, hash_r);

  if (verifyHash(hash_r, r->hmac2 ))
  {
    return 1;
  }

  /* hash payload and verify */
  NESSIEinit(&hash);
  NESSIEadd(p->data, p->data_sz*8, &hash);
  NESSIEfinalize(&hash, hash_r);

  if (verifyHash(hash_r, r->hmac ))
  {
    return 1;
  }

  return 0;
}


