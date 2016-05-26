/***
 Copyright (C) irfan
**/

#include "tulip.h"
#include "tul_tests.h"
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

int usage(int argc)
{
  if(argc == 1)
  {
    printf("usage: <tulip> <port> <udp [0|1]>\n\n\
    \tport: listening port\n\
    \t udp: a flag to enable UDP\n");
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
  
  if(argc == 2)
  {
    if(!strcmp(argv[1], "-D"))
    {
      run_tests();
      exit(0);
    }
    usage(1);
    exit(-1);
  }
  
  if(argc == 3)
  {
    port = atoi(argv[1]);
    udp_flag = atoi(argv[2]);
    
    if(port <= 0 || (strcmp(argv[2], "0") && strcmp(argv[2], "1")))
    {
      usage(1);
      exit(-1);
    }
  }
}

int main(int argc, char **argv)
{
 
  parseParams(argc, argv);
  //tul_make_daemon();
  
  tul_global_signal_handle_init();
  run_listener(port, udp_flag);
  
  while(!TUL_SIGNAL_INT)
  {
    /* let processes run and sleep
     * until we are signaled to exit */
    usleep(1000000);
  }
  
  return 0;
}

void run_tests()
{
  _tcp_listen_test();
  _tcp_connect_test();
  _udp_listen_test();
  _udp_connect_test();

}
