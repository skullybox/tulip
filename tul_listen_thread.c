/***
  Copyright (C) irfan
  Socket thread handler
 **/

#include "tul_listen_thread.h"
#include "tul_service.h"
#include "tul_tcp_soc.h"
#include "tul_globals.h"
#include "tul_net_context.h"

extern int TUL_SIGNAL_INT;

void *_run_listener(void *data);
void _run_core(int fd);
void do_read(int i);
void do_write(int i);

static fd_set read_fd_set;
static fd_set write_fd_set;

void run_listener(int port)
{
  pthread_attr_t _tref_attr;
  pthread_t _tref;

  int *data = (int*) malloc(sizeof(int));
  *data = port;

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
  int *d = (int*)data;

  if(tul_tcp_listen_init(*d, &sock))
  {
#ifdef SYSLOG_USE
    syslog(LOG_ERR, "%s", "network listener failed");
#else
    fprintf(stderr, "LOG_ERR: %s\n", "network listener failed");
#endif

    /* listen failed exit */
    free(d);
    TUL_SIGNAL_INT = 1;
    return NULL;
  }

  free(d);
#ifdef SYSLOG_USE
  syslog(LOG_INFO, "%s", "starting core");
#else
  fprintf(stdout, "LOG_INFO: %s\n", "starting core");
#endif
  _run_core(sock);
  return NULL;
}

void _run_core(int fd)
{
  int fd_new;
  socklen_t size;
  struct sockaddr_in client;

  size = sizeof(client);

  FD_ZERO(&read_fd_set);
  FD_ZERO(&write_fd_set);
  FD_SET(fd, &read_fd_set);

  tul_init_context_list();

  /* main loop of thread */
  while(!TUL_SIGNAL_INT)
  {
    if (select (FD_SETSIZE, &read_fd_set, &write_fd_set, NULL, NULL) < 0)
    {
      TUL_SIGNAL_INT = 1;
      return;
    }

    for(int i = 0; i < FD_SETSIZE; i++)
    {
      if(FD_ISSET(i, &read_fd_set))
      {
        if(i == fd)
        {
          /* new socket */
          fd_new = accept(fd, (struct sockaddr *)&client, &size);
          if(fd_new < 0)
          {
            TUL_SIGNAL_INT=1;
            return;
          }
          FD_SET(fd_new, &read_fd_set);
          tul_add_context(fd_new);
        }
        else
        {
          /* do read on connected socket */
          do_read(i);

        }
      }
      else if(FD_ISSET(i, &write_fd_set))
      {
        /* do write on connected socket if data on buffer */ 
        do_write(i);
          
      }
    }
  }
  tul_dest_context_list();
}

void do_read(int i)
{
  int bread = 0;
  tul_net_context *ctx;

  ctx = tul_find_context(i);

  if(ctx->_trecv < CTX_BLOCK)  
  {
    bread = read(ctx->_sock, 
        &(ctx->payload_in[ctx->payload_in_cnt]),
        CTX_BLOCK-ctx->payload_in_cnt);           

    /* socket closed */
    if(bread <= 0)
    {
      FD_CLR(i, &read_fd_set);
      FD_CLR(i, &write_fd_set);
      tul_rem_context(i);
    } 
    else
    {
      /* update bytes read */
      ctx->_trecv += bread;

      /* send to context processor */
      tul_service(ctx);
    }
  }
}

void do_write(int i)
{
  int bwrite = 0;
  tul_net_context *ctx;

  ctx = tul_find_context(i);

  if(ctx->payload_out_cnt > 0 && ctx->payload_out_cnt < CTX_BLOCK + 1 )  
  {
    bwrite = write(ctx->_sock, 
        &(ctx->payload_out[ctx->_tsend]),
        ctx->payload_out_cnt-ctx->_tsend);
    if(bwrite <= 0)
    {
      FD_CLR(i, &read_fd_set);
      FD_CLR(i, &write_fd_set);
      tul_rem_context(i);
    }
    else
    {
      /* update bytes read */
      ctx->_tsend += bwrite;

      /* send to context processor */
      tul_service(ctx);

    }
  }
}


