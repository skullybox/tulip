/***
  Copyright (C) irfan
  public key wrapper to gcrypt
 **/

#include "gcrypt.h"

#ifndef _tul_pub_key
#define _tul_pub_key

typedef gcry_sexp_t tul_keypair;

void tul_kill_keys(tul_keypair *k);
int tul_gen_keypair(tul_keypair *k);
int tul_write_keys(tul_keypair *par, char *path, char *fn);

#endif

