/***
  Copyright (C) irfan
  tulip main.
 **/

#include "tul_usage.h"
#include <stdio.h>

int usage(int argc)
{
  if(argc == 1)
  {
    printf("usage: <tulip> <port> -d, -t\n\
        \tport: listening port\n\
        \t  -d: run as daemon\n\
        \t  -t: enable tls\n\n");
    return 1;
  }
  return 0;
}


