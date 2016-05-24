/***
 Copyright (C) irfan

**/

#include "tul_daemon.h"

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

void tul_make_daemon()
{
  pid_t ret_p = 0;
  
  ret_p = fork();
  if(ret_p == -1)
  {
    fprintf(stderr, "fork error! ..exiting.\n");
  }
  
  if(ret_p > 0)
  {
    fprintf(stdout, "launching deamon ..Done.\n");
    exit(0);
  }
  
  fclose(stdout);
  fclose(stderr);
  fclose(stdin);
}

