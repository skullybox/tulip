/***
  Copyright (C) irfan
  public key wrapper to gcrypt
 **/

#include "tul_pub_key.h"

int tul_gen_keypair(tul_keypair *k)
{
  gcry_sexp_t key_spec;
  int size = 0;
  int rc = 0;

  rc = gcry_sexp_new (&key_spec,
      "(genkey (rsa (nbits 4:4096)(rsa-use-e 1:3)))", 0, 1);

  /* RSA param error */
  if(rc)
  {
    gcry_sexp_release (key_spec);
    return 1;
  }

  rc = gcry_pk_genkey(k, key_spec);
  gcry_sexp_release (key_spec);

  if(rc)
  {
    return 1;
  }

  return 0;
}

int tul_write_keys(tul_keypair *k, char *path, char *fn)
{
  int rc = 0;
  int size = 0;
  char p[4096] = {0};
  char *buff = NULL;
  FILE *fp = NULL;
  
  sprintf(p, "%s%s.keys", path, fn);
  
  fp = fopen(p, "w+b");
  if(!fp)
  {
    return 1;
  }

  size = gcry_sexp_sprint (*k, GCRYSEXP_FMT_ADVANCED, NULL, 0);
  buff = gcry_xmalloc(size);
  gcry_sexp_sprint (*k, GCRYSEXP_FMT_ADVANCED, buff, size);
  rc = fwrite(buff, 1, size, fp);
  fclose(fp);

  if(rc != size)
    return 1;

  return 0;
}

void tul_kill_keys(tul_keypair *k)
{
 gcry_free(k); 
}

