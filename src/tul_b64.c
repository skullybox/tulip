/***
  Copyright (C) irfan

 **/

#include "tul_b64.h"
#include <stdio.h>

static char B[64] = {
  'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
  'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd',
  'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's',
  't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7',
  '8', '9', '+', '/'
};

int getIndex(char c)
{
  for(int i = 0; i < 64; i++)
  {
    if(c == B[i])
      return i;
  }
  return -1;
}

unsigned char * base64_enc(unsigned char *d, size_t sz)
{
  unsigned char *n = NULL;
  size_t n_sz = 4 * (sz / 3);
  int b_count = 0;

  if(sz % 3)
    n = calloc(1, n_sz+4+1);
  else
    n = calloc(1, n_sz+1);

  for(int i = 0; i < sz; i += 3)
  {
    n[b_count + 0] = B[d[i]>>2]; 
    n[b_count + 1] = B[ ((d[i]&0x03)<<4) | ((d[i+1]&0xf0)>>4)];
    n[b_count + 2] = B[ ((d[i+1]&0x0f)<<2) | ((d[i+2]&0xc0)>>6)];
    n[b_count + 3] = B[(d[i+2]&0x3f)];
    b_count +=4;
  }

  b_count = n_sz + (sz%3)+1;
  if( (sz % 3) == 1)
  {
    n[b_count + 0] = '=';
    n[b_count + 1] = '=';
 
  }
  else if( (sz % 3) == 2)
  {
    n[b_count] = '=';
  }
  
  return n;
}

unsigned char * base64_dec(unsigned char *d, size_t sz)
{
  unsigned char *n = NULL;
  size_t n_sz = 0;
  int b_count = 0;
  int a,b,c,e;
  int offset = 0;

  /* check for padding --- */
  if( d[sz-1] == '=' && d[sz-2] == '=' )
    offset = 2;
  else  if(d[sz-1] == '=' )
    offset = 1;

  n_sz = ((sz)/4)*3 - offset;

  n = calloc(1, n_sz+1);

  for(int i = 0; i < sz; i+=4)
  {

    if( i + 4 < sz )
    {
      a = getIndex(d[i]);
      b = getIndex(d[i+1]);
      c = getIndex(d[i+2]);
      e = getIndex(d[i+3]);

      n[b_count+0] =  ((a<<2)&0xfc)|((b>>4)&0x03);
      n[b_count+1] =  ((b<<4)&0xf0)|((c>>2)&0x0f);
      n[b_count+2] =  ((c<<6)&0xc0)|((e)&0x03f);
    }
    else 
    {
      a = getIndex(d[i]);
      b = getIndex(d[i+1]);
      c = getIndex(d[i+2]);
      e = getIndex(d[i+3]);

      if ( offset == 2)
      {
        n[b_count+0] =  ((a<<2)&0xfc)|((b>>4)&0x03);
        c = 0;
        d = 0;
        e = 0;
      }
      else if ( offset == 1 )
      {
        n[b_count+0] =  ((a<<2)&0xfc)|((b>>4)&0x03);
        n[b_count+1] =  ((b<<4)&0xf0)|((c>>2)&0x0f);
        d = 0;
        e = 0;
      }
      else {
        n[b_count+0] =  ((a<<2)&0xfc)|((b>>4)&0x03);
        n[b_count+1] =  ((b<<4)&0xf0)|((c>>2)&0x0f);
        n[b_count+2] =  ((c<<6)&0xc0)|((e)&0x03f);
      }


    }
        
    b_count+=3;
  }

  return n;
}


