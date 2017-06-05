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
    printf("usage: <tulip> <port> -d, -n\n\
        \tport: listening port\n\
        \t  -d: run as daemon\n\
        \t  -n: disable tls\n\n");
    return 1;
  }
  return 0;
}


