/***
  Copyright (C) irfan

 **/

#include <string.h>
#include <stdlib.h>

#ifndef _tul_b64
#define _tul_b64

unsigned char * base64_enc(unsigned char *d, size_t sz);
unsigned char * base64_dec(unsigned char *d, size_t sz);
unsigned char * base64_dec_gt_sz(unsigned char *d, size_t sz, unsigned *nsz);

#endif

