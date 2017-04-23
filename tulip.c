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
int udp_flag = 0;
char GLOBAL_PATH[257] = {0};

int usage(int argc)
{
  if(argc == 1)
  {
    printf("usage: <tulip> <port> <path>\n\
        \tport: listening port\n\n");
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
  else if(argc == 3 && (port = atoi(argv[1])) > 0)
  {
    strcpy(GLOBAL_PATH, argv[2]);
    return;
  }

  usage(1);
  exit(-1);
}

int main(int argc, char **argv)
{

  parseParams(argc, argv);
#if defined(__APPLE__) || defined(__linux__)
  tul_make_daemon();
#else
  WSADATA wsaData;
  ZeroMemory(&wsaData,sizeof(wsaData));
  WSAStartup(MAKEWORD(2,2),&wsaData);
#endif
  tul_global_signal_handle_init();

  /* initialize crypto provider */
  if(crypto_init())
  {
#ifdef SYSLOG_USE
    syslog(LOG_ERR, "%s", "Initializing crypto provider failed!");
#else
    fprintf(stderr, "LOG_ERR: %s\n", "Initializing crypto provider failed!");
#endif    
    return -1;
  }
  else
  {
#ifdef SYSLOG_USE
    syslog(LOG_INFO, "%s", "crypto provider initialized");
#else
    fprintf(stdout, "LOG_INFO: %s\n", "crypto provider initialized");
#endif    
  }

  run_listener(port);

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
  fprintf(stdout, "LOG_INFO: %s\n", "exiting");
#endif
  return 0;
}

void run_tests()
{
  _tcp_listen_test();
  _tcp_connect_test();
}
