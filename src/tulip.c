/***
  Copyright (C) irfan
  tulip main.
 **/

#include "tulip.h"
#include "tul_db.h"
#include "tul_log.h"
#include "tul_module.h"
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


void print_version_info();

int main(int argc, char **argv)
{

  char tmp[201] = {0};

  /* open syslog */
#ifdef SYSLOG_USE
  char syslog_ident[] = "TULIP";
  openlog(syslog_ident, LOG_PID|LOG_NDELAY|LOG_CONS, LOG_LOCAL0);
#endif

  parseParams(argc, argv);

  print_version_info();
  tul_global_signal_handle_init();

#if defined(__APPLE__) || defined(__linux__)
  if(daemon_mode)
  {
    tul_log(" tulip_boot >>>> starting in daemon mode");
    tul_make_daemon();
  }
  else
    tul_log(" tulip_boot >>>> starting in foreground");

#else
  WSADATA wsaData;
  ZeroMemory(&wsaData,sizeof(wsaData));
  WSAStartup(MAKEWORD(2,2),&wsaData);
#endif

  tul_dbinit();

  tul_log(" tulip_boot >>>> starting listener");
  run_listener(port, tls);

  tul_log(" tulip_boot >>>> loading module callbacks");
  configure_module();

  /* print max payload info */
  sprintf(tmp, " tulip_boot >>>> max request payload: %d", MAX_REQ_PAYLOAD);
  tul_log(tmp);
  memset(tmp, 0, 201);
  sprintf(tmp, " tulip_boot >>>> max response payload: %d", MAX_RES_PAYLOAD);
  tul_log(tmp);

  while(!TUL_SIGNAL_INT)
  {
    /* let processes run and sleep
     * until we are signaled to exit */
    usleep(1000000);
  }

  tul_dbclean();

#if !defined(__APPLE__) && !defined(__linux__)
  WSACleanup();
#endif
  tul_log("exiting (sleeping 2s)");
  usleep(2000000);
  return 0;
}


void print_version_info()
{
  for(int i = 0; i < 15; i++)
  {
    printf("%s\n",&TUL_TULIP[i][0]);
  }

  printf(" ------------------------\n %s -- %s\n ------------------------\n",
      TUL_SERVERNAME, TUL_VERSION );

}


