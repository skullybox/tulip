/***
  Copyright (C) irfan

  place globals here
 **/

#if defined(__APPLE__) || defined(__linux__)
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <sys/select.h>
#include <syslog.h>
#define SYSLOG_USE
#else
#define UNICODE 1
#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x0501
#include <ws2tcpip.h>
#include <winsock2.h>
#include <fcntl.h>
#endif

#include <time.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>


#ifndef _tul_globals
#define _tul_globals

#define DEF_SOCK_BUFF_SIZE 2048
#define DEF_CTX_LIST_SZ 120

typedef struct __app_context
{
  unsigned int sock;
  struct sockaddr_in addr;
  short udp_mode;
  char dat_in[DEF_SOCK_BUFF_SIZE];
  char dat_out[DEF_SOCK_BUFF_SIZE];
  unsigned int s;
  unsigned int r;
} tul_app_ctx;

tul_app_ctx tul_context_list[DEF_CTX_LIST_SZ];

extern int TUL_SIGNAL_INT;

#endif
