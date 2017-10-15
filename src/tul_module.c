/***
  Copyright (C) irfan
  module module callbacks
 **/

#include "tul_b64.h"
#include "tul_log.h"
#include "tul_user.h"
#include "tul_module.h"
#include "rc5_cipher.h"
#include "whirlpool_hash.h"
#include "tul_net_context.h"

extern tul_read_callback tul_RD_callback;
extern tul_write_callback tul_WR_callback;

void configure_module()
{
  tul_RD_callback = (tul_read_callback) &module_read;
  tul_WR_callback = (tul_write_callback) &module_write;
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
  NESSIEadd((unsigned char*)&(p->tag), sizeof(unsigned)*8, &hash); 

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

void module_read(tul_net_context *c)
{
  comm_req r; 
  comm_payload p;
  char buff[200] = {0};
  int ret = 0;

  memset(&r, 0, sizeof(comm_req));
  memset(&p, 0, sizeof(comm_payload));
  ret = verify_payload(c, &r, &p);

  /* waiting for more data */
  if(ret == -1)
    return;

  /* if request is validated or error 
   * during validation reset recieve buffers
   * and counters
   */
  if(ret == 1 || ret == 0)
  {
    /* reset */
    c->_trecv = 0;
    c->_ttrecv = 0;
    memset(c->payload_in, 0, DEF_SOCK_BUFF_SIZE);

    if(ret && !(c->_user_auth))
    {
      c->_teardown = 1;
      return;
    }
  }

  /* proceed to action request */
  switch(p.action)
  {
    case LOGIN:
      c->_user_auth = 1;
      if(!do_login(r.user, &p))
      {
        sprintf(buff, " user login: %s", r.user);
        tul_log(buff);
        /* TODO: send OK */
      }
      else {
        /* this should not happen! */
        /* TODO: send INVALID */
      }


      break;
    case LOGOUT:
      sprintf(buff, " user logout: %s", r.user);
      tul_log(buff);
      break;
    case GET_LIST:
      break;
    case SEND_MSG:
      break;
    case ADDFRIEND:
      break;
    case DELFRIEND:
      break;
    default:
      /* TODO: send error */
      break;
  }
  
}

int do_login(char *user, comm_payload *p)
{
  return strcmp(user, p->data);
}

int send_response(char *u, int s, tul_net_context *c)
{
  RC5_ctx rc5;
  NESSIEstruct hash;

  char _user[31] = {0};
  char _pass[25] = {0};
  char _salt[25] = {0};
  char r_salt[16] = {0};
  char r_kek[16] = {0};
  int ret = 0;

  strncpy(_user, u, 30);

  ret = get_user_pass(_user, _pass, _salt);

  /* random salt and kek
   * used to encrypt payload data */
  tul_random(&r_salt, 16);
  tul_random(&r_kek, 16);
  

  return 0;
}


void module_write(tul_net_context *c)
{
  if(c->_tsend >= c->_ttsend)
  {
    c->_ttsend = 0;
    c->_tsend = 0;
    memset(c->payload_out, 0, DEF_SOCK_BUFF_SIZE);
  }
}


