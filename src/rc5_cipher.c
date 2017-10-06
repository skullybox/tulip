/* RC5 cipher
 * Modified from RC5REF file
 */

#include "rc5_cipher.h"
#include <stdio.h>

const WORD P = 0xb7e15163, Q = 0x9e3779b9;  /* magic constants */

void rc5_encrypt(WORD *pt, WORD *ct, RC5_ctx *ctx, short len)
{
  for(int i = 0; i < len/2; i+=2)
  {
    RC5_ENCRYPT(&pt[i], &ct[i], ctx);
  }
}

void rc5_decrypt(WORD *ct, WORD *pt, RC5_ctx *ctx, short len)
{
  for(int i = 0; i < len/2; i+=2)
  {
    RC5_DECRYPT(&ct[i], &pt[i], ctx);
  }
}

void RC5_ENCRYPT(WORD *pt, WORD *ct, RC5_ctx *ctx)
{
  WORD i, A=pt[0]+ctx->S[0], B=pt[1]+ctx->S[1];
  for(i=1; i<= RC5_r; i++)
  {
    A = ROTL(A^B,B)+ctx->S[2*i];
    B = ROTL(B^A,A)+ctx->S[2*i+1];
  }
  ct[0] = A; ct[1] = B;
}

void RC5_DECRYPT(WORD *ct, WORD *pt, RC5_ctx *ctx)
{
  WORD i, B=ct[1], A=ct[0];
  for(i=RC5_r; i>0; i--)
  {
    B = ROTR(B-ctx->S[2*i+1],A)^A;
    A = ROTR(A-ctx->S[2*i],B)^B;
  }
  pt[1] = B-ctx->S[1]; pt[0] = A-ctx->S[0];
}

void RC5_SETUP(unsigned char *K, RC5_ctx *ctx)
{
  WORD i, j, k, u=RC5_W/8, A, B, L[RC5_c];
  for (i=RC5_b-1, L[RC5_c-1]= 0; i!=-1; i--)
    L[i/u] = (L[i/u]<<8)+K[i];
  for(ctx->S[0]=P,i=1; i<RC5_t; i++)
    ctx->S[i] = ctx->S[i-1]+Q;
  for(A=B=i=j=k=0; k<3*RC5_t; k++, i=(i+1)%RC5_t, j=(j+1)%RC5_c)
  {
    A=ctx->S[i]=ROTL(ctx->S[i]+(A+B),3);
    B=L[j]=ROTL(L[j]+(A+B),(A+B));
  }
}

void salt_password(unsigned char *p, unsigned char *salt, short sz)
{
  return;
    for(int i = 0; i < sz; i++)
    {
      p[i] = p[i] ^ salt[i];
    }
}

