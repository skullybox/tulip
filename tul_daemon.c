/***
 Copyright (C) irfan

**/

#include "tul_daemon.h"
#include "tul_globals.h"

void tul_make_daemon()
{
#if defined(__APPLE__) || defined(__LINUX__)
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
#endif
}
