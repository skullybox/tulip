/***
  Copyright (C) irfan
  user client functions
 **/

#include "tul_userc.h"
#include "tul_tcp_soc.h"
#include "tul_random.h"
#include "whirlpool_hash.h"

int client_login(char *uid, char *pass, tul_net_context *conn)
{

  comm_req r;
  comm_payload p;
  NESSIEstruct hash;
  unsigned char hash_r[DIGESTBYTES] = {0};

  memset(&r, 0, REQ_HSZ);
  memset(&p, 0, sizeof(comm_payload));
  
  strncpy(r.user, uid, 30);

  /* random encryption key and salt */
  tul_random(r.salt, 16);
  tul_random(r.kek, 16);

  p.action = LOGIN;

  /* data size with encryption
   * block size into account
   */
  if(strlen(uid)%16)
  {
    p.data = calloc(1,strlen(uid)+1+16);
    p.data_sz = strlen(uid)+1+16;
  }
  else
  {
    p.data = calloc(1,strlen(uid)+1);
    p.data_sz = strlen(uid)+1;
  }


  /* data stores uid as part of
   * payload to confirm login
   */
  strcpy(p.data, uid);

  /* take data hash prior to 
   * encryption
   */
  NESSIEinit(&hash);
  NESSIEadd(p.data, p.data_sz*8, &hash);
  NESSIEfinalize(&hash, hash_r);
  
  /* copy payload hash to payload
   * header
   */
  memcpy(r.hmac, hash_r, DIGESTBYTES);

  /* TODO: encrypt payload using kek */

  /* TODO: take hash of header */

  /* TODO: copy to network context */


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


