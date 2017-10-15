/***
  Copyright (C) irfan
  module module callbacks
 **/

#include "tul_b64.h"
#include "tul_log.h"
#include "tul_random.h"
#include "tul_user.h"
#include "tul_module.h"
#include "rc5_cipher.h"
#include "whirlpool_hash.h"
#include "tul_net_context.h"
#include "tul_module_common.h"

extern tul_read_callback tul_RD_callback;
extern tul_write_callback tul_WR_callback;

/* forward declarations */
int do_login(char *user, comm_payload *p);
int send_response(char *u, unsigned s, tul_net_context *c);

void configure_module()
{
  tul_RD_callback = (tul_read_callback) &module_read;
  tul_WR_callback = (tul_write_callback) &module_write;
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

        /* send OK */
        send_response(r.user, OK, c);
      }
      else {
        /* this should not happen! */
        /* send INVALID */
        send_response(r.user, INVALID, c);
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
      send_response(r.user, ERROR, c);
      break;
  }
  
}

int do_login(char *user, comm_payload *p)
{
  return strcmp(user, p->data);
}

int send_response(char *u, unsigned s, tul_net_context *c)
{
  RC5_ctx rc5;
  comm_resp r;
  comm_payload p;
  NESSIEstruct hash;

  char _user[31] = {0};
  char _pass[25] = {0};
  char _salt[25] = {0};
  int ret = 0;

  strncpy(_user, u, 30);

  ret = get_user_pass(_user, _pass, _salt);
  if(ret)
  {
      c->_teardown = 1;
      return 1;
  }
  decrypt_user_pass(_pass, _salt);

  /* random salt and kek
   * used to encrypt payload data */
  tul_random(&r.salt, 16);
  tul_random(&r.kek, 16);

  p.action = RESPONSE;

  /* data size with encryption
   * block size into account
   */
  if((sizeof(unsigned))%16)
  {
    p.data_sz = 16*((sizeof(unsigned)/16))+16;
    PAYLOAD_CHECK_SZ;
    p.data = calloc(p.data_sz, 1);
  }
  else
  {
    p.data_sz = (sizeof(unsigned)/16)*16;
    PAYLOAD_CHECK_SZ;
    p.data = calloc(p.data_sz,1);
  }

  r.payload_sz = sizeof(comm_payload) + p.data_sz;

  /* copy data */
  memcpy(p.data, &s, sizeof(s));

  return prep_transmission(u, _pass, (comm_req*)&r, &p, c);
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


