/***
 Copyright (C) irfan

**/

#include "tul_listen_thread.h"

#include <pthread.h>
#include <stdlib.h>

// internal forward declaration
//  for running thread function
typedef struct _internal_list_struct
{
  int port;
  int udp;
} internal_list_s;

void *_run_listener(void *data);

void run_listener(int port, int udp)
{
  pthread_t _tref;
  internal_list_s *data = 
    (internal_list_s*) malloc(sizeof(internal_list_s));
  
  internal_list_s->port = port;
  internal_list_s->udp = udp;
  
  pthread_create(&_tref, NULL, _run_listener, data);
  
}