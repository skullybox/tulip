/***
  Copyright (C) irfan
  Crypto functions referecing libgcrypt
 **/

#include "tul_crypto_ref.h"
#include "gcrypt.h"

int crypto_init()
{
  if( gcry_check_version(GCRYPT_VERSION) ||
      gcry_control(GCRYCTL_SUSPEND_SECMEM_WARN) ||
      gcry_control(GCRYCTL_INIT_SECMEM, 102400, 0) ||
      gcry_control(GCRYCTL_RESUME_SECMEM_WARN) ||
      gcry_control(GCRYCTL_INITIALIZATION_FINISHED, 0))
    return 1;
  return 0;
}
