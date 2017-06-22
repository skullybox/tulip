/* RC5 cipher
 * Modified from RC5REF file
 */

#ifndef _tul_rc5
#define _tul_rc5

typedef unsigned     WORD; /* Should be 32-bit = 4 bytes        */
#define RC5_W        32 /* RC5_Word size in bits                 */
#define RC5_r        27 /* number of rounds                  */  
#define RC5_b        16 /* number of bytes in key            */
#define RC5_c         4 /* number  RC5_Words in key = ceil(8*b/RC5_W)*/
#define RC5_t        56 /* size of table S = 2*(r+1) RC5_Words   */
/* Rotation operators. x must be unsigned, to get logical right shift*/
#define ROTL(x,y) (((x)<<(y&(RC5_W-1))) | ((x)>>(RC5_W-(y&(RC5_W-1)))))
#define ROTR(x,y) (((x)>>(y&(RC5_W-1))) | ((x)<<(RC5_W-(y&(RC5_W-1)))))

typedef struct _RC5_ctx
{
  WORD S[RC5_t];         /* expanded key table */
  unsigned char K[RC5_b];
} RC5_ctx;

void RC5_ENCRYPT(WORD *pt, WORD *ct, RC5_ctx *ctx);
void RC5_DECRYPT(WORD *ct, WORD *pt, RC5_ctx *ctx);
void RC5_SETUP(unsigned char *K, RC5_ctx *ctx);


#endif

