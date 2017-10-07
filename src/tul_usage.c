/***
  Copyright (C) irfan
  tulip main.
 **/

#include "tul_usage.h"
#include <stdio.h>


extern void print_version_info();

int usage(int argc)
{
  print_version_info();
  if(argc == 1)
  {
    printf(" usage: <tulip> <port> -d\n\
        \tport: listening port\n\
        \t  -d: run as daemon\n\n");
    return 1;
  }
  return 0;
}


