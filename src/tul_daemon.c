/***
  Copyright (C) irfan

 **/

#include "tul_log.h"
#include "tul_daemon.h"
#include "tul_globals.h"

void tul_make_daemon()
{
#if defined(__APPLE__) || defined(__linux__)
  pid_t ret_p = 0;

  ret_p = fork();
  if(ret_p < 0)
  {
    tul_log(" tulip_boot >>> fork error! ..exiting.");
    exit(-1);
  }
  else if(ret_p > 0)
  {
    tul_log(" tulip_boot >>> launching deamon ..done.");
    exit(0);
  }

  umask(0);
  int ret = chdir("/tmp/");
  //close(STDIN_FILENO);
  close(STDOUT_FILENO);
  close(STDERR_FILENO);
#endif
}
