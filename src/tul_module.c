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

void module_read(tul_net_context *c)
{
  RC5_ctx rc5;
  comm_req r; 
  comm_payload p;
  int ret = 0;
  char t_kek[16] = {0};
  char *t_salt = 0;
  char uid[31] = {0};
  char pass[25] = {0};
  char salt[25] = {0};
  NESSIEstruct hash;
  unsigned char hash_r[DIGESTBYTES] = {0};

  printf("T: %d - %d\n", c->_trecv, c->_ttrecv);
  if(c->_trecv < REQ_HSZ)
    return;

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
    return;
  
  /* grab the request header */
  memset(&r, 0, REQ_HSZ);
  memcpy(&r, c->payload_in, REQ_HSZ);

  /* lookup the user and password */ 
  strncpy(uid, r.user, 30);
  ret = get_user_pass(uid, pass, salt);
  if(ret)
  {
    /* TODO: error case */
  }

  decrypt_user_pass(pass, salt);

  /* decrypt kek */
  t_salt = base64_dec(salt, strlen(salt));
  salt_password(pass, t_salt, 16);
  RC5_SETUP(pass, &rc5);
  rc5_decrypt((unsigned*)r.kek, (unsigned *)t_kek, &rc5, 16);
  memcpy(r.kek, t_kek, 16);

  memcpy(&p, &(c->payload_in[REQ_HSZ]), sizeof(comm_req));
  p.data = calloc(1, p.data_sz);

  /* verify header hash */
  NESSIEinit(&hash);
  NESSIEadd(r.user, 30*8, &hash);
  NESSIEadd((unsigned char *)&(r.salt), 16*8, &hash);
  NESSIEadd(r.kek, 16*8, &hash); 
  NESSIEadd(r.hmac, DIGESTBYTES*8, &hash); 
  NESSIEadd((unsigned char *)&(r.payload_sz), 
      sizeof(unsigned)*8, &hash); 
  NESSIEadd((unsigned char *)&(p.action), sizeof(unsigned)*8, &hash); 
  NESSIEadd((unsigned char*)&(p.data_sz), sizeof(unsigned), &hash); 
  NESSIEadd((unsigned char*)&(p.tag), sizeof(unsigned)*8, &hash); 

  NESSIEadd((unsigned char*)&(c->payload_in[REQ_HSZ+sizeof(comm_req)]), 
      p.data_sz*8, 
      &hash); 
  NESSIEfinalize(&hash, hash_r);

  if (verifyHash(hash_r, r.hmac2 ))
  {
    /* TODO: error case */
    goto RESET_REQ;
  }


  /* use the password to decrypt
   * the payload 
   */
  RC5_SETUP(t_kek, &rc5);
  rc5_decrypt((unsigned *)&(c->payload_in[REQ_HSZ+sizeof(comm_req)]), 
      (unsigned*)p.data, &rc5, p.data_sz);


  /* hash payload and verify */
  NESSIEinit(&hash);
  NESSIEadd(p.data, p.data_sz*8, &hash);
  NESSIEfinalize(&hash, hash_r);

  if (verifyHash(hash_r, r.hmac ))
  {
    /* TODO: error case */
    goto RESET_REQ;
  }


  tul_log("LOGIN!");

RESET_REQ:
  /* reset */
  c->_trecv = 0;
  c->_ttrecv = 0;
  memset(c->payload_in, 0, DEF_SOCK_BUFF_SIZE);

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


