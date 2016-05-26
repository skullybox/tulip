/***
 Copyright (C) irfan

**/

#include "tul_daemon.h"

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>

void tul_make_daemon()
{
  pid_t ret_p = 0;
  
  ret_p = fork();
  if(ret_p < 0)
  {
    fprintf(stderr, "fork error! ..exiting.\n");
    exit(-1);
  }
  else if(ret_p > 0)
  {
    fprintf(stdout, "launching deamon ..done.\n");
    exit(0);
  }
  
  umask(0);
  chdir("/tmp/");
  //close(STDIN_FILENO);
  close(STDOUT_FILENO);
  close(STDERR_FILENO);
}

