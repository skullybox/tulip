/***
  Copyright (C) irfan
  network context cache
 **/

#include "tul_log.h"
#include "tul_tls_common.h"
#include "tul_globals.h"
#include "tul_net_context.h"

static _tul_int_context_struct _glbl_struct_list;
static pthread_mutex_t _glbl_struct_mtx;
extern tul_tls_ctx tls_serv;

int tul_add_context(unsigned sock, int tls)
{
  int ret = 1;
  _tul_int_context_struct *new = NULL;
  _tul_int_context_struct *cur = &_glbl_struct_list;

  /* find the end of the list */
  while(cur->next != NULL && cur->_this != NULL && cur->_this->_sock != sock)
  {
    cur = cur->next;
  }

  /*
   * _this should never happen.
   * we should never try to add a socket already in the list
   */
  if(cur->_this != NULL && cur->_this->_sock == sock)
  {
    tul_log(" skipping adding socket; already in list");
  }
  else /* we add the network context here */
  {
    if(cur->_this == NULL)
    {
      cur->_this = (tul_net_context*)calloc(1, sizeof(tul_net_context));
      cur->_this->_sock = sock;
      new = cur;
    }
    else
    {
      new = (_tul_int_context_struct *)calloc(1, sizeof(_tul_int_context_struct));
      new->_this = (tul_net_context*)calloc(1, sizeof(tul_net_context));
      new->_this->_sock = sock;
      cur->next = new;
    }

    ret = 0;
    /* do tls setup */
    if(tls)
    {
      new->_this->_use_tls = 1;

      mbedtls_net_init(&(new->_this->tls.server_fd));
      mbedtls_ssl_init(&(new->_this->tls.ssl));
      new->_this->tls.server_fd.fd = sock;

      mbedtls_net_set_nonblock(&(new->_this->tls.server_fd));
      mbedtls_ssl_setup(&(new->_this->tls.ssl), &(tls_serv.conf));
      mbedtls_ssl_set_bio( 
          &(new->_this->tls.ssl), &(new->_this->tls.server_fd), 
          mbedtls_net_send, mbedtls_net_recv, NULL );

      while( (ret = mbedtls_ssl_handshake( &(tls_serv.ssl))) != 0)
      {
        if(ret != MBEDTLS_ERR_SSL_WANT_READ && 
            ret != MBEDTLS_ERR_SSL_WANT_WRITE)
        {
          ret = 1;
          break;
        }
      }

      /* TLS handshake failure */
      if(ret)
      {
        mbedtls_ssl_session_reset(&(tls_serv.ssl));
        new->_this->tls.handshake_done = 0;
      }
      else {
        new->_this->tls.handshake_done = 1;
        mbedtls_net_set_block(&(new->_this->tls.server_fd));
      }
    }
  }
  return ret;
}

int tul_get_sock(unsigned pos)
{
  int count = 1;
  int ret = -1;
  _tul_int_context_struct *cur = &_glbl_struct_list;

  for( int i = 0; i < pos; i++)
  {
    if(cur != NULL && cur->next != NULL)
    {
      cur = cur->next;
      count++;
    }
    else
      break;
  }

  if(count == pos && cur->_this != NULL)
    ret = cur->_this->_sock;

  return ret;
}

void tul_rem_context(unsigned sock)
{
  _tul_int_context_struct *tmp = NULL;
  _tul_int_context_struct *cur = &_glbl_struct_list;

  while(cur->next != NULL && cur->_this != NULL && cur->_this->_sock != sock)
  {
    tmp = cur;
    cur = cur->next;
  }

  if(cur->_this != NULL && cur->_this->_sock == sock)
  {
    /* close the socket */
    close(cur->_this->_sock);
    memset(cur->_this, 0, sizeof(tul_net_context));
    free(cur->_this);
    cur->_this = NULL;

    if(tmp != NULL)
    {
      if(cur->next != NULL)
        tmp->next = cur->next;
      else
        tmp->next = NULL;
    }
      
    memset(cur, 0, sizeof(_tul_int_context_struct));
  }
}

void tul_init_context_list()
{
  pthread_mutex_lock(&_glbl_struct_mtx);
  memset(&_glbl_struct_list, 0, sizeof(_tul_int_context_struct));
  tul_log(" tulip_boot >>>> starting net context");
  pthread_mutex_unlock(&_glbl_struct_mtx);
}


void tul_dest_context_list()
{
  pthread_mutex_lock(&_glbl_struct_mtx);

  _tul_int_context_struct *cur = &_glbl_struct_list;
  _tul_int_context_struct *tmp = NULL;
  cur = &_glbl_struct_list;

  /* find the end of the list */
CHECK_LIST:
  while(cur != NULL && cur->next != NULL)
  {
    tmp = cur;
    cur = cur->next;
  }

  /* free from the end to the tip */
  while(_glbl_struct_list.next != NULL)
  {
    /* clear data & close socket */
    if(cur && cur->_this && cur->_this->_sock)
      close(cur->_this->_sock);
    if(cur && cur->_this)
    {
      memset(cur->_this, 0, sizeof(tul_net_context));
      free(cur->_this);
      cur->_this = NULL;
      memset(cur, 0, sizeof(_tul_int_context_struct));
      free(cur);
      cur = NULL;
    }
    if(tmp)
      tmp->next = NULL;
    goto CHECK_LIST;
  }
  pthread_mutex_unlock(&_glbl_struct_mtx);
}

tul_net_context* tul_find_context(unsigned sock)
{
  _tul_int_context_struct* node = &_glbl_struct_list;
  tul_net_context* net_ctx = node->_this;

  while(net_ctx != NULL && net_ctx->_sock != sock)
  {
    if(node->next != NULL)
    {
      node = node->next;
      net_ctx = node->_this;
    }
    else
      return NULL;
  }

  return net_ctx;
}


