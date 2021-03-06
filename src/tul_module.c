/***
  Copyright (C) irfan
  module module callbacks
 **/

#include "tul_b64.h"
#include "tul_mysql.h"
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
extern unsigned char dbk[17];

/* forward declarations */
int do_ignore_friend(char *user, comm_payload *p);
int do_login(char *user, comm_payload *p);
int send_response(char *u, unsigned s, tul_net_context *c, comm_payload *_p);
int do_get_msg(char *user, comm_payload *p);
int do_send_msg(char *user, comm_payload *p);
int do_get_list(char *user, comm_payload *p, unsigned long long offset);
int do_get_addreq(char *user, comm_payload *p);
int do_add_friend(char *user, comm_payload *p);
int do_rem_friend(char *user, comm_payload *p);
int do_accept_friend(char *user, comm_payload *p);
int do_msg_read(char *user, comm_payload *p);

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
  unsigned long long offset = 0;

  memset(&r, 0, sizeof(comm_req));
  memset(&p, 0, sizeof(comm_payload));
  ret = verify_payload(c, &r, &p);

  if(ret == -1)
  {
    tul_log(" data pending...");
    return;
  }

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
      tul_log(" payload error...");
      return;
    }
  }

  /* proceed to action request */
  switch(p.action)
  {
    case LOGIN:
      if(!do_login(r.user, &p))
      {
        c->_user_auth = 1;
        sprintf(buff, " login - OK <<< %s", r.user);
        tul_log(buff);

        /* send OK */
        send_response(r.user, OK, c, NULL);
      }
      else {
        /* this should not happen!
         * failed logins drop the connection above
         * send INVALID
         */
        send_response(r.user, INVALID, c, NULL);
      }

      break;
    case LOGOUT:
      sprintf(buff, " logout - OK <<< %s", r.user);
      tul_log(buff);
      c->_user_auth = 0;
      c->_teardown = 1;
      break;
    case GET_LIST:
      memcpy(&offset,&((char*)p.data)[30], sizeof(unsigned long long));
      sprintf(buff, " getlist >>> %s", r.user);
      tul_log(buff);

      do_get_list(r.user, &p, offset);
      send_response(r.user, p.action, c, &p);
      break;
    case ACCEPTFRIEND:
      if(!do_accept_friend(r.user, &p))
      {
        sprintf(buff, " add friend - OK <<< %s", r.user);
        tul_log(buff);
        send_response(r.user, OK, c, NULL);
      }
      else
      {
        sprintf(buff, " add friend - ERROR >>> %s", r.user);
        tul_log(buff);
        send_response(r.user, INVALID, c, NULL);
      }
      break;
    case SEND_MSG:
      if(!do_send_msg(r.user, &p))
      {
        sprintf(buff, " msg friend - OK >>> %s", r.user);
        send_response(r.user, OK, c, NULL);
      }
      else
      {
        sprintf(buff, " msg friend - ERROR >>> %s", r.user);
        tul_log(buff);
        send_response(r.user, INVALID, c, NULL);
      }
      break;
    case GET_MSG:
      sprintf(buff, " get msg <<< %s", r.user);
      tul_log(buff);
      do_get_msg(r.user, &p);
      send_response(r.user, p.action, c, &p);
      break;
    case GET_FREQ:
      sprintf(buff, " friend request <<< %s", r.user);
      tul_log(buff);
      do_get_addreq(r.user, &p);
      send_response(r.user, p.action, c, &p);
      break;
    case ADDFRIEND:
      if(!do_add_friend(r.user, &p))
      {
        sprintf(buff, " add user - OK <<< %s", r.user);
        tul_log(buff);
        send_response(r.user, OK, c, NULL);
      }
      else 
      {
        sprintf(buff, " add user - ERROR >>> %s", r.user);
        tul_log(buff);
        send_response(r.user, ERROR, c, NULL);
      }
      break;
    case IGNOREFRIEND:
      do_ignore_friend(r.user, &p);
      break;
    case DELFRIEND:
      sprintf(buff, " del friend <<< %s", r.user);
      tul_log(buff);
      break;
    default:
      /* send error */
      sprintf(buff, " error >>> %s", r.user);
      tul_log(buff);
      send_response(r.user, ERROR, c, NULL);
      break;
  }

}

/*
 * getting a message involves the
 * following in the request:
 * offset message ID > 0 [optional]
 * the target uid [optional]
 *
 * one message is returned per request
 */

int do_get_msg(char *user, comm_payload *p)
{
  RC5_ctx rc5;
  unsigned sz = 0;
  unsigned nsz = 0;
  char _uid[30] = {0};
  char _tuid[30] = {0};
  unsigned msg_len = 0;
  char SQL[5048] = {0};
  unsigned _tuid_set = 0;
  unsigned long long _offset = 0;
  MYSQL_RES *res = NULL;
  MYSQL_ROW irow;
  unsigned ret = 0;
  unsigned long long id, rows = 0ULL;
  unsigned long long rowid = 0;
  char uname[30] = {0};
  char frm[30] = {0};
  char typ[4] = {0};
  unsigned new = 0;
  char msg[MAX_MESSAGE];
  unsigned char l_dbk[17] = {0};
  char *tmp1 = NULL;
  char *tmp2 = NULL;

  // when no offset or t_uid is set or not
  char QL0[] = "select rowid, uname, frm, typ, new, msg, salt from message where uname='%s' or frm='%s' order by rowid asc limit 1";
  char QL1[] = "select rowid, uname, frm, typ, new, msg, salt from message where (uname='%s' or frm='%s') and (uname='%s' or frm='%s') order by rowid asc limit 1";

  // when offset is provided
  char QL2[] = "select rowid, uname, frm, typ, new, msg, salt from message where rowid > %llu and (uname='%s' or frm='%s') order by rowid asc limit 1";

  // when offset is provided and t_uid is set
  char QL3[] = "select rowid, uname, frm, typ, new, msg, salt from message where rowid > %llu and (uname='%s' or frm='%s') and (uname='%s' or frm='%s') order by rowid asc limit 1";

  strncpy(_uid, user, 30);
  strncpy(_tuid, &((char*)p->data)[8], 30);

  memcpy(&_offset, &((char*)p->data)[0], 8);

  if(strlen(_tuid))
    _tuid_set = 1;

  if(_tuid_set && !user_exists(_tuid))
    return -1;

  // friend exists
  if(_tuid_set && !friend_in_list(_uid, _tuid))
    return -1;

  switch(_offset)
  {
    case 0:
      if(!_tuid_set)
        sprintf(SQL, QL0, _uid, _uid);
      else 
        sprintf(SQL, QL1, _uid, _uid, _tuid, _tuid);
      break;
    default:
      if(!_tuid_set)
        sprintf(SQL, QL2, _offset, _uid, _uid);
      else 
        sprintf(SQL, QL3, _offset, _uid, _uid, _tuid, _tuid);
      break;
  }

  ret = tul_query_get(SQL, &res);
  if(res == NULL || mysql_num_rows(res) == 0)
  {
    /* send END */
    p->action = END;
    p->data_sz = 0;
    if(p->data)
      free(p->data);
    p->data = NULL;
    if(res)
      mysql_free_result(res);
    return 0;
  }

  rows = mysql_num_rows(res);
  if(rows == 1)
  {
    // rowid, uname, frm, typ, new, msg 
    irow = mysql_fetch_row(res);
    memset(msg, 0, MAX_MESSAGE);

    // copy offset
    rowid = strtoull(irow[0], NULL, 10);

    // uname
    strncpy(uname, irow[1], 30);

    // frm
    strncpy(frm, irow[2], 30);
    
    // typ
    strncpy(typ, irow[3], 3);

    // new
    new = atoi(irow[4]);

    // msg -- decrypt
    tmp1 = base64_dec(irow[6], strlen(irow[6])); // salt
    tmp2 = base64_dec_gt_sz(irow[5], strlen(irow[5]), &nsz); // msg

    memcpy(l_dbk, dbk, 16);
    salt_password(l_dbk, tmp1, 16);
    free(tmp1);
    tmp1 = NULL;

    RC5_SETUP(l_dbk, &rc5);
    rc5_decrypt((unsigned*)tmp2, (unsigned*)msg, &rc5, nsz);
    
    free(tmp2);
    msg_len = strlen(msg);

    p->action = PAGING;
  }
  else {
    p->action = END;
  }
  mysql_free_result(res);

/*
 * message byte setup:
 * 8 bytes message id
 * 4 bytes true / false new flag
 * 3 bytes type (SYS/USR)
 * 30 bytes to user
 * 30 bytes from user
 * 20 bytes reserved
 * 4 bytes message length
 * message from user
 */
 
  sz = (8+4+3+30+50+4+(strlen(msg)));

  if(sz%16)
    sz += (16-(sz%16));

  p->data_sz = sz;
  if(p->data)
    free(p->data);
  p->data = calloc(p->data_sz, 1);

  memcpy(p->data, &rowid, 8);
  memcpy(&((char*)p->data)[8], &new, 4);
  memcpy(&((char*)p->data)[12], typ, 3);
  memcpy(&((char*)p->data)[15], uname, 30);
  memcpy(&((char*)p->data)[45], frm, 30);
  memcpy(&((char*)p->data)[95], &msg_len, 4);
  memcpy(&((char*)p->data)[99], msg, msg_len);

  return 0;
}

int do_send_msg(char *user, comm_payload *p)
{
  RC5_ctx rc5;
  char _uid[30] = {0};
  char _tuid[30] = {0};
  unsigned msg_len = 0;
  unsigned sz = 0;
  char *msg = NULL;
  char SQL[5048] = {0};
  unsigned char salt[25] = {0};
  unsigned char l_dbk[17] = {0};
  char *tmp1 = NULL;
  char *tmp2 = NULL;
  char *tmp3 = NULL;

  strncpy(_uid, user, 30);

  // message should be USR type
  if(strncmp(&((char*)p->data)[12], "USR", 3))
    return -1;

  // to user
  strncpy(_tuid, &((char*)p->data)[15], 30);
  if(!user_exists(_tuid))
    return -1;

  // friend exists
  if(!friend_in_list(_uid, _tuid))
    return -1;

  // message pointer
  msg = &((char*)p->data)[MESSAGE_META_SZ];


  // encrypt message before insert
  tul_random(&salt[0], 16); 
  tmp1 = base64_enc(salt, 16);
  memcpy(l_dbk, dbk, 16);
  salt_password(l_dbk, salt,16);

  // copy base64 salt back to salt variable
  memset(salt, 0, 25);
  strcpy(salt, tmp1);
  free(tmp1);
  tmp1 = NULL;

  if(strlen(msg)%16)
    sz = strlen(msg)+(16-(strlen(msg)%16));
  else 
    sz = strlen(msg);

  tmp1 = calloc(sz, 1);
  tmp2 = calloc(sz, 1);
  strcpy(tmp1, msg);

  RC5_SETUP(l_dbk, &rc5);
  rc5_encrypt((unsigned*)tmp1, (unsigned*)tmp2, &rc5, sz);
  free(tmp1);
  tmp1 = base64_enc(tmp2, sz);
  free(tmp2);

  sprintf(SQL, "insert into message (uname, frm, typ, msg, salt) values ('%s', '%s', '%s', '%s', '%s')",
      _tuid, _uid, "USR", tmp1, salt);

  free(tmp1);
  return tul_query(1, SQL);
}

int do_get_addreq(char *user, comm_payload *p)
{
  unsigned long long rows = 0ULL;
  MYSQL_RES *res = NULL;
  MYSQL_ROW irow;
  char SQL[2048] = {0};
  char uid[30] = {0};
  sprintf(SQL, "select user_from from friend_request where uname='%s' limit 20", user);
  int ret = 0;

  ret = tul_query_get(SQL, &res);
  if(res == NULL || mysql_num_rows(res) == 0)
  {
    /* send END */
    p->action = END;
    p->data_sz = 0;
    if(p->data)
      free(p->data);
    p->data = NULL;
    goto GETADDRQ_END;
  }

  /* maximum of query based on friend column
   * 30 bytes * 20 records
   */

  /* allocate based on row response for
   * payload
   */
  rows = mysql_num_rows(res);
  if((30*rows)%16)
    p->data_sz = 16;
  else 
    p->data_sz = 0;
  p->data_sz += ((30*rows)/16)*16;

  if(p->data)
    free(p->data);
  p->data = calloc(p->data_sz, 1);

  for(int i = 0; i < rows; i++)
  {
    irow = mysql_fetch_row(res);
    memset(uid, 0, 30);
    strncpy(uid, irow[0], 30);

    memcpy(&((char*)p->data)[30*i],
        uid,
        30);

  }
  if(rows == 20)
    p->action = PAGING;
  else
    p->action = END;

GETADDRQ_END:
  if(res)
    mysql_free_result(res);
  return 0;

}


int do_get_list(char *user, comm_payload *p, unsigned long long offset)
{
  MYSQL_RES *res = NULL;
  MYSQL_ROW irow;
  unsigned ret = 0;
  unsigned long long id, rows = 0ULL;
  char SQL[2048] = {0};
  char uid[30] = {0};
  sprintf(SQL, "select rowid, friend from friend_list where uname='%s' and rowid > %llu limit 200", user, offset);

  ret = tul_query_get(SQL, &res);
  if(res == NULL || mysql_num_fields(res) == 0)
  {
    /* send END */
    if(res)
      mysql_free_result(res);

    p->action = END;
    p->data_sz = 0;
    if(p->data)
      free(p->data);
    p->data = NULL;
    goto GETLIST_END;
  }

  /* maximum of query based on friend column
   * 30 bytes * 200 records
   */

  /* allocate based on row response for
   * payload
   */
  rows = mysql_num_rows(res);
  if((30*rows+8)%16)
    p->data_sz = 16;
  else 
    p->data_sz = 0;
  p->data_sz += ((30*rows+8)/16)*16;

  if(p->data)
    free(p->data);
  p->data = calloc(p->data_sz, 1);

  for(int i = 0; i < rows; i++)
  {
    irow = mysql_fetch_row(res);
    memset(uid, 0, 30);
    strncpy(uid, irow[1], 30);

    memcpy(&((char*)p->data)[8+30*i],
        uid,
        30);

    if(i+1 >= rows)
    {
      /* store last id in payload
       * for future offset
       */
      id = strtoull(irow[0], NULL, 10);
      if(id == 0)
      {
        if(p->data)
        {
          free(p->data);
          p->data = NULL;
        }
        p->data_sz = 0;
        p->action = ERROR;
        mysql_free_result(res);
        return -1;
      }

      memcpy(&((char*)p->data)[0], &id, 8);

      if(rows == 200)
        p->action = PAGING;
      else
        p->action = END;
    }
  }


GETLIST_END:
  mysql_free_result(res);
  return 0;
}

int do_accept_friend(char *user, comm_payload *p)
{
  char SQL1[4096] = {0};
  char SQL2[4096] = {0};
  char SQL3[4096] = {0};
  char SQL4[4096] = {0};
  char uid[30] = {0};
  char t_uid[30] = {0};
  unsigned ret = 0;

  strncpy(uid, user, 30);
  if(p->data_sz == 32)
    strncpy(t_uid, p->data, 30);
  else 
    return -1;

  if(!friend_request_exists(t_uid, uid))
    return -1;

  if(!friend_in_list(user, t_uid))
  {
    sprintf(SQL1, "insert into friend_list(uname, friend) values ('%s', '%s')", 
        uid, t_uid); 
    ret |= tul_query(1, SQL1);
  }

  if(!friend_in_list(t_uid, user))
  {
    sprintf(SQL2, "insert into friend_list(uname, friend) values ('%s', '%s')", 
        t_uid, uid); 
    ret |= tul_query(1, SQL2);
  }

  sprintf(SQL3, "delete from friend_request where uname='%s' and user_from='%s'",
      uid, t_uid); 
  sprintf(SQL4, "delete from friend_request where uname='%s' and user_from='%s'",
      t_uid, uid); 

  ret |= tul_query(2, SQL3, SQL4);
  return ret;

}

int do_ignore_friend(char *user, comm_payload *p)
{
  char SQL[2048] = {0};
  char uid[30] = {0};
  char t_uid[30] = {0};

  strncpy(uid, user, 30);
  if(p->data_sz == 32)
    strncpy(t_uid, p->data, 30);
  else 
    return -1;

  sprintf(SQL, "delete from friend_request where uname='%s' and user_from='%s'", 
      uid, t_uid); 
  return tul_query(1, SQL);
}

int do_add_friend(char *user, comm_payload *p)
{
  char SQL[2048] = {0};
  char uid[30] = {0};
  char t_uid[30] = {0};

  strncpy(uid, user, 30);
  if(p->data_sz == 32)
    strncpy(t_uid, p->data, 30);
  else 
    return -1;

  if(!user_exists(t_uid))
    return -1;

  if(friend_in_list(uid, t_uid))
    return 0;

  sprintf(SQL, "insert into friend_request(uname, user_from) values ('%s', '%s')", 
      t_uid, uid); 
  
  if(!friend_request_exists(uid, t_uid))
    return tul_query(1, SQL);
  else 
    return -1;
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


