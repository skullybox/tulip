/***
  Copyright (C) irfan

 **/

#include "tul_b64.h"

static char B[64] = {
  'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
  'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd',
  'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's',
  't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7',
  '8', '9', '+', '/'
};

char * base64_enc(char *d, size_t sz)
{
  char *n = NULL;
  size_t n_sz = 4 * (sz / 3);
  int b_count = 0;

  if(sz % 3)
    n = calloc(1, n_sz+4);
  else
    n = calloc(1, n_sz);

  for(int i = 0; i < sz; i += 3)
  {
    n[b_count + 0] = B[d[i]>>2]; 
    n[b_count + 1] = B[ ((d[i]&0x03)<<4) | ((d[i+1]&0xf0)>>4)];
    n[b_count + 2] = B[ ((d[i+1]&0x0f)<<2) | ((d[i+2]&0xc0)>>6)];
    n[b_count + 3] = B[(d[i+2]&0x3f)];
    b_count +=4;
  }

  if( (sz % 3) == 1)
  {
    n[b_count + 0] = B[d[sz]>>2]; 
    n[b_count + 1] = B[ ((d[sz]&0x03)<<4)]; 
    n[b_count + 2] = '=';
    n[b_count + 3] = '=';
 
  }
  else if( (sz % 3) == 2)
  {
    n[b_count + 0] = B[d[sz]>>2]; 
    n[b_count + 1] = B[ ((d[sz]&0x03)<<4) | ((d[sz+1]&0xf0)>>4)];
    n[b_count + 2] = B[ ((d[sz]&0x0f)<<2)];
    n[b_count + 3] = '=';
  }
  
  return n;
}

char * base64_dec(char *d, size_t sz)
{
  char *n = NULL;
  size_t n_sz = 3* (sz / 4);
  int b_count = 0;

  return NULL;
}


