/***
  Copyright (C) irfan
  module module callbacks
 **/

#include "tul_b64.h"
#include "tul_db.h"
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
int send_response(char *u, unsigned s, tul_net_context *c, comm_payload *_p);
int do_get_msg(char *user, comm_payload *p);
int do_send_msg(char *user, comm_payload *p);
int do_get_list(char *user, comm_payload *p, unsigned long long offset);
int do_add_friend(char *user, comm_payload *p);
int do_rem_friend(char *user, comm_payload *p);

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
  unsigned offset = 0;

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
        send_response(r.user, OK, c, NULL);
      }
      else {
        /* this should not happen!
         * send INVALID
         */
        send_response(r.user, INVALID, c, NULL);
      }

      break;
    case LOGOUT:
      sprintf(buff, " user logout: %s", r.user);
      tul_log(buff);
      c->_teardown = 1;
      break;
    case GET_LIST:
      memcpy(&offset,&((char*)p.data)[4], sizeof(unsigned));
      sprintf(buff, " user get_list: %s", r.user);
      tul_log(buff);

      do_get_list(r.user, &p, offset);
      send_response(r.user, p.action, c, &p);
      break;
    case SEND_MSG:
      break;
    case GET_MSG:
      break;
    case GET_FREQ:
      break;
    case ADDFRIEND:
        if(!do_add_friend(r.user, &p))
          send_response(r.user, OK, c, NULL);
        else 
          send_response(r.user, ERROR, c, NULL);
      break;
    case DELFRIEND:
      break;
    default:
      /* send error */
      send_response(r.user, ERROR, c, NULL);
      break;
  }

}

int do_get_msg(char *user, comm_payload *p)
{

  return 0;
}

int do_send_msg(char *user, comm_payload *p)
{

  return 0;
}

int do_get_list(char *user, comm_payload *p, unsigned long long offset)
{
  unsigned long long id = 0ULL;
  int row, col = 0;
  char **res = NULL;
  char SQL[4096] = {0};
  char uid[30] = {0};
  sprintf(SQL, "select id, friend from friend_list where uid='%s' and id > %llu limit 200", user, offset);

  tul_query_get(SQL, &res, &row, &col);
  if(row <= 0)
  {
    /* send END */

    p->action = END;
    p->data_sz = 0;
    p->data = NULL;
    goto GETLIST_END;
  }

  /* maximum of query based on friend column
   * 30 bytes * 200 records
   */

  /* allocate based on row response for
   * payload
   */
  p->data = calloc(30*row+8+30, 1);
  p->data_sz = 30*row+8+30;

  for(int i = col; i < (col*row)+col; i+=col )
  {
    memset(uid, 0, 30);
    strncpy(uid, res[i+1], 30);

    memcpy(&((char*)p->data)[30+8+30*((i-col)/col)],
        uid,
        30);

    if(i+col >= ((col*row)+col))
    {
      /* store last id in payload
       * for future offset
       */
      id = strtoull(res[i], NULL, 10);
      if(id == 0)
      {
        if(p->data)
        {
          free(p->data);
          p->data = NULL;
        }
        p->data_sz = 0;
        p->action = ERROR;
        sqlite3_free_table(res);
        return -1;
      }

      memcpy(&((char*)p->data)[30], &id, 8);

      if(row == 200)
        p->action = PAGING;
      else
        p->action = END;
    }
  }


GETLIST_END:
  sqlite3_free_table(res);
  return 0;
}

int do_add_friend(char *user, comm_payload *p)
{
  char SQL[4096] = {0};
  char uid[30] = {0};
  char t_uid[30] = {0};

  strncpy(uid, user, 30);
  if(p->data_sz == 32)
    strncpy(t_uid, p->data, 30);
  else 
    return -1;

  if(!user_exists(t_uid))
    return -1;

  sprintf(SQL, "insert into friend_request(uid, user_from) values ('%s', '%s')", 
      uid, t_uid); 
  return tul_query(1, SQL);
}

int do_rem_friend(char *user, comm_payload *p)
{

  return 0;
}

int do_login(char *user, comm_payload *p)
{
  return strcmp(user, p->data);
}

int send_response(char *u, unsigned s, tul_net_context *c, comm_payload *_p)
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

  p.action = s;

  /* data size with encryption
   * block size into account
   */
  if(_p == NULL)
  {
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
  }
  else
  {
    r.payload_sz = sizeof(comm_payload) + _p->data_sz;
  }


  if(_p)
    return prep_transmission(u, _pass, (comm_req*)&r, _p, c);
  else
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


