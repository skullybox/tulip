/***
  Copyright (C) irfan
  Socket thread handler
 **/

#include "tul_log.h"
#include "tul_module.h"
#include "tul_tls_common.h"
#include "tul_tls_server.h"
#include "tul_net_context.h"
#include "tul_tcp_soc.h"
#include "tul_globals.h"
#include "tul_listen_thread.h"

extern int TUL_SIGNAL_INT;
extern int daemon_mode;


void *_run_listener(void *data);
void _run_core(int fd, int tls);
void do_read(int i, int tls);
void do_write(int i, int tls);

static fd_set read_fd_set;
static fd_set write_fd_set;
static fd_set active_set;
tul_tls_ctx tls_serv;

void run_listener(int port, int tls)
{
  pthread_attr_t _tref_attr;
  pthread_t _tref;

  int *data = (int*) malloc(sizeof(int)*2);
  *data = port;
  data[1] = tls;

  /* pthread detached attribute */
  pthread_attr_init(&_tref_attr);
  pthread_attr_setdetachstate(&_tref_attr, PTHREAD_CREATE_DETACHED);

  pthread_create(&_tref, &_tref_attr, _run_listener, data);
  pthread_attr_destroy(&_tref_attr);
}

void *_run_listener(void *data)
{
  int ret = 0;
  int sock = 0;
  struct sockaddr_in6 addr;
  int *lport = (int*)data;
  int tls = ((int*)data)[1];

  if(!tls)
  {
    if ( tul_tcp_listen_init(*lport, &sock) )
    {
      tul_log(" tulip_boot >>>> ERROR: network listener failed");

      /* listen failed exit */
      free(lport);
      TUL_SIGNAL_INT = 1;
      return NULL;
    }
  }
  else
  {
    tul_log(" tulip_boot >>>> enabling tls");
    ret = tls_server_init(&tls_serv, *lport);
    sock = tls_serv.server_fd.fd;

    if(ret)
    {
      tul_log(" tulip_boot >>>> tls server init failed!");
      free(lport);
      TUL_SIGNAL_INT = 1;
      return NULL;
    }
  }

  free(lport);
  tul_log(" tulip_boot >>>> starting core");
  _run_core(sock, tls);
  return NULL;
}

void _run_core(int fd, int tls)
{
  int ret = 0;
  int ref_sock;
  int fd_new;
  socklen_t size;
  struct sockaddr_in client;
  struct timeval tm;

  size = sizeof(client);

  tul_init_context_list();

  if(tls && fd != tls_serv.server_fd.fd)
    tls_serv.server_fd.fd = fd;

  FD_ZERO(&read_fd_set);
  FD_ZERO(&write_fd_set);
  FD_SET(fd, &active_set);

  /* main loop of thread */
  while(!TUL_SIGNAL_INT)
  {
    read_fd_set = active_set;
    write_fd_set = active_set;
    tm.tv_sec = 15;
    tm.tv_usec = 0;
    if (select (FD_SETSIZE, &read_fd_set, &write_fd_set, NULL, &tm) < 0)
    {
      TUL_SIGNAL_INT = 1;
      return;
    }

    for(int i = 0; i < FD_SETSIZE; ++i)
    {
      if( i == 0 )
      {
        ref_sock = fd;
      }
      else
      {
        ref_sock = tul_get_sock(i);
        if(ref_sock == -1)
          continue;
      }

      if(FD_ISSET(ref_sock, &read_fd_set) && ref_sock == fd )
      {
        /* new socket */
        fd_new = accept(fd, (struct sockaddr *)&client, &size);

        if(fd_new < 0)
        {
          TUL_SIGNAL_INT=1;
          return;
        }
        FD_SET(fd_new, &active_set);
        ret = tul_add_context(fd_new, tls);
      }
      else if(FD_ISSET(ref_sock, &read_fd_set))
      {
        do_read(ref_sock, tls);
      }
      else if(FD_ISSET(ref_sock, &write_fd_set))
      {
        do_write(ref_sock, tls);
      }
    }
  }
  tul_dest_context_list();
}

void do_read(int i, int tls)
{
  int bread = 0;
  tul_net_context *ctx;
  tul_tls_ctx *tls_ctx;

  ctx = tul_find_context(i);
  tls_ctx = &(ctx->tls);

  if(ctx != NULL )
  {
    if(ctx->_trecv >= DEF_SOCK_BUFF_SIZE
        && !ctx->_teardown)
      goto DO_READ_SERVICE;

    if(ctx->_teardown)
      goto DO_READ_CLOSE;

    if(!tls)
    {
      if(ctx->_trecv < DEF_SOCK_BUFF_SIZE)
            bread = read(ctx->_sock,
              &(ctx->payload_in[ctx->_trecv]),
              DEF_SOCK_BUFF_SIZE-ctx->_trecv);
    }
    else
    {
      if(!(tls_ctx->handshake_done))
      {
        int ret = mbedtls_ssl_handshake(&(tls_ctx->ssl));
        if(ret ==  MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_READ)
        {

          return;
        }
        else if (!ret){
          tls_ctx->handshake_done = 1;
        }
        else {
          ctx->_teardown = 1; 
          goto DO_READ_CLOSE;
        }

      }
      if(ctx->_trecv < DEF_SOCK_BUFF_SIZE)
        bread = tls_read(tls_ctx,
            &(ctx->payload_in[ctx->_trecv]),
            DEF_SOCK_BUFF_SIZE-ctx->_trecv);
    }


    /* socket closed */
    if(bread == -1)
    {
DO_READ_CLOSE:
      close(i);
      FD_CLR(i, &active_set);
      tul_rem_context(i);
    }
    else if(bread > 0)
    {
      ctx->timestamp = time(NULL);

      /* update bytes read */
      ctx->_trecv += bread;

DO_READ_SERVICE:
      /* send to context processor */
      tul_service(ctx, 0);
    }
  }
}

void do_write(int i, int tls)
{
  int bwrite = 0;
  tul_net_context *ctx;
  tul_tls_ctx *tls_ctx;

  ctx = tul_find_context(i);
  tls_ctx = &(ctx->tls);

  if(ctx != NULL )
  {

    if(ctx->_teardown)
      goto DO_WRITE_CLOSE;

    if( ctx->_ttsend && ctx->_tsend < ctx->_ttsend )
    {
      if(!tls)
      {
        bwrite = write(ctx->_sock,
            &(ctx->payload_out[ctx->_tsend]),
            ctx->_ttsend-ctx->_tsend);
      }
      else if(tls_ctx->handshake_done)
      {
        bwrite = tls_write(tls_ctx,
            &(ctx->payload_out[ctx->_tsend]),
            ctx->_ttsend-ctx->_tsend);
      }

      if(bwrite == -1)
      {
        FD_CLR(i, &active_set);
        tul_rem_context(i);
      }
      else if(bwrite > 0)
      {
        ctx->timestamp = time(NULL);

        /* update bytes read */
        ctx->_tsend += bwrite;

        /* send to context processor */
        tul_service(ctx, 1);

        if(ctx->_teardown)
          goto DO_WRITE_CLOSE;

      }
    }
  }
  if(ctx->_teardown)
  {
DO_WRITE_CLOSE:
    close(i);
    FD_CLR(i, &active_set);
    tul_rem_context(i);
  }
}


