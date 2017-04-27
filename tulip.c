/***
  Copyright (C) irfan
  tulip main.
 **/

#include "tulip.h"
#include "tul_tests.h"
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
#ifdef SYSLOG_USE
  char syslog_ident[] = "TULIP";
  openlog(syslog_ident, LOG_PID|LOG_NDELAY, LOG_LOCAL1);
#endif

#if defined(__APPLE__) || defined(__linux__)
  if(daemon_mode)
  {
#ifdef SYSLOG_USE
    syslog(LOG_INFO, "%s", "starting in daemon mode");
#endif
    tul_make_daemon();
  }
#ifdef SYSLOG_USE
  else
    syslog(LOG_INFO, "%s", "starting in foreground");
#endif
    if(!daemon_mode)
      fprintf(stdout, "starting in foreground\n");

#else
  WSADATA wsaData;
  ZeroMemory(&wsaData,sizeof(wsaData));
  WSAStartup(MAKEWORD(2,2),&wsaData);
#endif

  /* initialize crypto provider */
  if(crypto_init())
  {
#ifdef SYSLOG_USE
    syslog(LOG_ERR, "%s", "Initializing crypto provider failed!");
#endif    
    if(!daemon_mode)
      fprintf(stderr, "Initializing crypto provider failed!\n");
    return -1;
  }
  else
  {
#ifdef SYSLOG_USE
    syslog(LOG_INFO, "%s", "crypto provider initialized");
#endif    
    if(!daemon_mode)
      fprintf(stdout, "crypto provider initialized\n");
  }

#ifdef SYSLOG_USE
  syslog(LOG_INFO, "%s", "starting listener");
#endif   
  if(!daemon_mode)
    fprintf(stdout, "starting listener\n");
  run_listener(port);

#ifdef SYSLOG_USE
  syslog(LOG_INFO, "%s", "loading module callbacks");
#endif   
  if(!daemon_mode)
    fprintf(stdout, "loading module callbacks\n");
 

  while(!TUL_SIGNAL_INT)
  {
    /* let processes run and sleep
     * until we are signaled to exit */
    usleep(1000000);
  }
#if !defined(__APPLE__) && !defined(__linux__)
  WSACleanup();
#endif
#ifdef SYSLOG_USE
  syslog(LOG_INFO, "%s", "exiting (sleeping 2 seconds)");
#else
  if(!daemon_mode)
    fprintf(stdout, "exiting (sleeping 2 seconds)");
#endif
  return 0;
}

void run_tests()
{
  _tcp_listen_test();
  _tcp_connect_test();
}


