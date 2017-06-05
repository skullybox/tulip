/***
  Copyright (C) irfan
  tulip main.
 **/

#include "tulip.h"
#include "tul_log.h"
#include "tul_module.h"
#include "tul_config.h"
#include "tul_daemon.h"
#include "tul_globals.h"
#include "tul_parseparams.h"
#include "tul_listen_thread.h"
#include "tul_signal_handler.h"

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int tls = 1;
int port = 0;
int daemon_mode = 0;

int main(int argc, char **argv)
{

  parseParams(argc, argv);

  tul_global_signal_handle_init();

  /* open syslog */
#ifdef SYStul_log_USE
  char syslog_ident[] = "TULIP";
  openlog(syslog_ident, tul_log_PID|tul_log_NDELAY, tul_log_LOCAL1);
#endif

#if defined(__APPLE__) || defined(__linux__)
  if(daemon_mode)
  {
    tul_log("starting in daemon mode");
    tul_make_daemon();
  }
  else
    tul_log("starting in foreground");

#else
  WSADATA wsaData;
  ZeroMemory(&wsaData,sizeof(wsaData));
  WSAStartup(MAKEWORD(2,2),&wsaData);
#endif

  /* load configuration */
  load_config();

  tul_log("starting listener");
  run_listener(port, tls);

  tul_log("loading module callbacks");
  configure_module();


  while(!TUL_SIGNAL_INT)
  {
    /* let processes run and sleep
     * until we are signaled to exit */
    usleep(1000000);
  }
#if !defined(__APPLE__) && !defined(__linux__)
  WSACleanup();
#endif
  tul_log("exiting (sleeping 2s)");
  usleep(2000000);
  return 0;
}



