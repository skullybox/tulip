/***
  Copyright (C) irfan
  tulip main.
 **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tul_usage.h"
#include "tul_testwrap.h"

extern int port;
extern int daemon_mode;
extern int tls;

void parseParams(int argc, char **argv)
{
  int ret = 0;

  if((ret = usage(argc)))
  {
    exit(ret);
  }

  /* check for tls enablement */
  for(int i = 1; i < argc; i++)
  {
    if(!strcmp("-t", argv[i]))
    {
      tls = 1;
      break;
    }
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
      !strcmp(argv[2],"-t"))
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
  else if(argc == 4 && (port = atoi(argv[1])) > 0 &&
      !strcmp(argv[2],"-d") && !strcmp(argv[3],"-t"))
  {
#if defined(__APPLE__) || defined(__linux__)
    daemon_mode = 1;
#endif
    return;
  }


  usage(1);
  exit(-1);
}

