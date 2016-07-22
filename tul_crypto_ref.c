/***
  Copyright (C) irfan
  Crypto functions referecing libgcrypt
 **/

#include "tul_crypto_ref.h"
#include "gcrypt.h"

int crypto_init()
{
  gcry_cipher_hd_t handle_test;
  gcry_control(GCRYCTL_SUSPEND_SECMEM_WARN); 
  gcry_control(GCRYCTL_INIT_SECMEM, 102400, 0);
#if defined(__APPLE__) || defined(__linux__)
  gcry_control(GCRYCTL_RESUME_SECMEM_WARN);
#endif
  gcry_control(GCRYCTL_INITIALIZATION_FINISHED, 0);

  if( !gcry_check_version(GCRYPT_VERSION) || gcry_cipher_open(&handle_test, GCRY_CIPHER_AES256, GCRY_CIPHER_MODE_CBC, 0))
    return 1;
  else
    gcry_cipher_close(handle_test);
  return 0;
}
