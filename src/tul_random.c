/***
  Copyright (C) irfan
 **/

#include "tul_random.h"
#include <stdio.h>

void random(void *data, unsigned sz)
{
  if(sz == 0)
    return;

  FILE *fp = fopen("/dev/urandom", "r");
  if(fp)
    return;

  fread(data, 1, sz, fp);
  fclose(fp);
}

