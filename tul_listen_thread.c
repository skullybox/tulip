/***
 Copyright (C) irfan

**/

#include "tul_listen_thread.h"
#include "tul_tcp_soc.h"
#include "tul_udp_soc.h"

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
  int sock;
  struct sockaddr_in6 addr;
  int udp_mode = 0;
  internal_list_s *d = (internal_list_s*)data;
  
  udp_mode = d->udp;
  if(udp_mode)
  {
    if(tul_udp_listen_init(d->port, &sock, &addr))
    {
      
      /* listen failed exit */
      free(d);
      TUL_SIGNAL_INT = 1;
      return NULL;
    }
  }
  else
  {
    if(tul_tcp_listen_init(d->port, &sock))
    {
      
      /* listen failed exit */
      free(d);
      TUL_SIGNAL_INT = 1;
      return NULL;
    }
  }
  free(d);
  
  /* main loop of thread */
  while(!TUL_SIGNAL_INT)
  {
    
  }
  
  return NULL;
}
