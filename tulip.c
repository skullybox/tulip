/***
  Copyright (C) irfan
  tulip main.
 **/

#include "tulip.h"
#include "tul_log.h"
#include "tul_tests.h"
#include "tul_module.h"
#include "tul_crypto_ref.h"
#include "tul_daemon.h"
#include "tul_globals.h"
#include "tul_listen_thread.h"
#include "tul_signal_handler.h"

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int port = 0;
int daemon_mode = 0;

int usage(int argc)
{
  if(argc == 1)
  {
    printf("usage: <tulip> <port> [-d]\n\
        \tport: listening port\n\
        \t  -d: run as daemon\n\n");
    return 1;
  }
  return 0;
}

void parseParams(int argc, char **argv)
{
  int ret = 0;

  if((ret = usage(argc)))
  {
    exit(ret);
  }

  if(argc == 2 && !strcmp(argv[1], "-D"))
  {
#if !defined(__APPLE__) && !defined(__linux__)
    WSADATA wsaData;
    ZeroMemory(&wsaData,sizeof(wsaData));
    WSAStartup(MAKEWORD(2,2),&wsaData);
#endif
    run_tests();
#if !defined(__APPLE__) && !defined(__linux__)
    WSACleanup();
#endif
    exit(0);
  }
  else if(argc == 2 && (port = atoi(argv[1])) > 0)
  {
    return;
  }
  else if(argc == 3 && (port = atoi(argv[1])) > 0 &&
      !strcmp(argv[2],"-d"))
  {
#if defined(__APPLE__) || defined(__linux__)
    daemon_mode = 1;
#endif
    return;
  }

  usage(1);
  exit(-1);
}

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

  /* initialize crypto provider */
  if(crypto_init())
  {
    tul_log("Initializing crypto provider failed!");
    return -1;
  }
  else
    tul_log("crypto provider initialized");

  tul_log("starting listener");
  run_listener(port);

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
  tul_log("exiting (sleeping 2seconds)");
  return 0;
}

void run_tests()
{
  _tcp_listen_test();
  _tcp_connect_test();
}


