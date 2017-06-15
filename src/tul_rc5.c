/* RC5 cipher
 * Modified from RC5REF file
 */

#include "tul_rc5.h"

void RC5_ENCRYPT(WORD *pt, WORD *ct)
{
  WORD i, A=pt[0]+S[0], B=pt[1]+S[1];
  for(i=1; i<= RC5_r; i++)
  {
    A = ROTL(A^B,B)+S[2*i];
    B = ROTL(B^A,A)+S[2*i+1];
  }
  ct[0] = A; ct[1] = B;
}

void RC5_DECRYPT(WORD *ct, WORD *pt)
{
  WORD i, B=ct[1], A=ct[0];
  for(i=RC5_r; i>0; i--)
  {
    B = ROTR(B-S[2*i+1],A)^A;
    A = ROTR(A-S[2*i],B)^B;
  }
  pt[1] = B-S[1]; pt[0] = A-S[0];
}

void RC5_SETUP(unsigned char *K)
{
  WORD i, j, k, u=RC5_W/8, A, B, L[RC5_c];
  for (i=RC5_b-1, L[RC5_c-1]= 0; i!=-1; i--)
    L[i/u] = (L[i/u]<<8)+K[i];
  for(S[0]=P,i=1; i<RC5_t; i++)
    S[i] = S[i-1]+Q;
  for(A=B=i=j=k=0; k<3*RC5_t; k++, i=(i+1)%RC5_t, j=(j+1)%RC5_c)
  {
    A=S[i]=ROTL(S[i]+(A+B),3);
    B=L[j]=ROTL(L[j]+(A+B),(A+B));
  }
}



