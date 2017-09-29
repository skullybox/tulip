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

#define DEF_SOCK_BUFF_SIZE 4096
#define DEF_CTX_LIST_SZ 220

static char TUL_TULIP[15][50] = {
"                ,",
"            /\\^/`\\",
"           | \\/   |",
"           | |    |",
"           \\ \\    /",
"            '\\\\//'",
"              ||",
"              ||",
"              ||",
"              ||  ,",
"          |\\  ||  |\\",
"          | | ||  | |",
"          | | || / /",
"           \\ \\||/ /",
"            `\\\\//`"
};
static char TUL_SERVERNAME[] = "Tulip Server";
static char TUL_VERSION[] = "0.01";

extern int TUL_SIGNAL_INT;

#endif
