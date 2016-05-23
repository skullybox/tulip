/***
 Copyright (C) irfan

**/

#include "tul_listen_thread.h"

#include <pthread.h>
#include <stdlib.h>

extern int TUL_SIGNAL_INT;

/* internal forward declaration
 *  for running thread function
 */
typedef struct _internal_list_struct
{
  int port;
  int udp;
} internal_list_s;

void *_run_listener(void *data);

void run_listener(int port, int udp)
{
  pthread_attr_t _tref_attr;
  pthread_t _tref;
  
  internal_list_s *data = 
    (internal_list_s*) malloc(sizeof(internal_list_s));
  
  data->port = port;
  data->udp = udp;
  
  /* pthread detached attribute */
  pthread_attr_init(&_tref_attr);
  pthread_attr_setdetachstate(&_tref_attr, PTHREAD_CREATE_DETACHED);
  
  pthread_create(&_tref, &_tref_attr, _run_listener, data);
  pthread_attr_destroy(&_tref_attr);
}

void *_run_listener(void *data)
{
  
  return NULL;
}
