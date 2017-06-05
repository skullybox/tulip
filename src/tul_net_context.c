/***
  Copyright (C) irfan

 **/

#include "tul_log.h"
#include "tul_globals.h"
#include "tul_net_context.h"

static _tul_int_context_struct _glbl_struct_list;
static pthread_mutex_t _glbl_struct_mtx;

int tul_add_context(unsigned sock, int tls)
{
  int ret = 1;
  _tul_int_context_struct *new = NULL;
  _tul_int_context_struct *cur = &_glbl_struct_list;

  /* find the end of the list */
  while(cur->next != NULL && cur->this != NULL && cur->this->_sock != sock)
  {
    cur = cur->next;
  }

  /*
   * this should never happen.
   * we should never try to add a socket already in the list
   */
  if(cur->this != NULL && cur->this->_sock == sock)
  {
    tul_log("skipping adding socket; already in list");
  }
  else /* we add the network context here */
  {
    if(cur->this == NULL)
    {
      cur->this = (tul_net_context*)calloc(1, sizeof(tul_net_context));
      cur->this->_sock = sock;

      new = (_tul_int_context_struct *)calloc(1, sizeof(_tul_int_context_struct));
      new->this = (tul_net_context*)calloc(1, sizeof(tul_net_context));
      new->this->_sock = sock;
      cur->next = new;

      ret = 0;
      /* do tls setup */
      printf("TLS IT: %d\n", tls);
      if(tls)
      {
        new->this->_use_tls = 1;

        mbedtls_ssl_set_bio( 
            &(new->this->ssl), &(new->this->net_c), 
            mbedtls_net_send, mbedtls_net_recv, NULL );

        mbedtls_debug_set_threshold(4);

        while( (ret = mbedtls_ssl_handshake( &(new->this->ssl))) != 0)
        {
          if(ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE)
          {
            ret = 1;
            break;
          }
        }
        printf("TLS ret: %d\n", ret);

        /* TLS handshake failure */
        if(ret)
        {
          mbedtls_net_free( &(new->this->net_c));
          mbedtls_ssl_session_reset( &(new->this->ssl) );

          free(new);
          cur->next = NULL;
        }
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
    if(cur->next != NULL)
    {
      cur = cur->next;
      count++;
    }
    else
      break;
  }

  if(count == pos && cur->this != NULL)
    ret = cur->this->_sock;

  return ret;
}

void tul_rem_context(unsigned sock)
{
  _tul_int_context_struct *cur = &_glbl_struct_list;
  while(cur->next != NULL && cur->this->_sock != sock)
  {
    cur = cur->next;
  }

  if(cur->this->_sock == sock)
  {
    cur->back = cur->next;
    cur->next = cur->back;

    /* close the socket */
    close(cur->this->_sock);
    memset(cur->this, 0, sizeof(tul_net_context));
    free(cur->this);
    memset(cur, 0, sizeof(_tul_int_context_struct));
  }
}

void tul_init_context_list()
{
  pthread_mutex_lock(&_glbl_struct_mtx);
  memset(&_glbl_struct_list, 0, sizeof(_tul_int_context_struct));
  tul_log("starting net context");
  pthread_mutex_unlock(&_glbl_struct_mtx);
}


void tul_dest_context_list()
{
  pthread_mutex_lock(&_glbl_struct_mtx);

  _tul_int_context_struct *cur = &_glbl_struct_list;
  _tul_int_context_struct *tmp = NULL;
  cur = &_glbl_struct_list;

  /* find the end of the list */
  while(cur->next != NULL)
  {
    cur = cur->next;
  }

  /* free from the end to the tip */
  while(cur != NULL)
  {
    tmp = cur->back;

    /* clear data & close socket */
    if(cur && cur->this && cur->this->_sock)
      close(cur->this->_sock);
    if(cur && cur->this)
    {
      memset(cur->this, 0, sizeof(tul_net_context));
      free(cur->this);
      memset(cur, 0, sizeof(_tul_int_context_struct));
      free(cur);
    }
    cur = tmp;
  }
  pthread_mutex_unlock(&_glbl_struct_mtx);
}

tul_net_context* tul_find_context(unsigned sock)
{
  _tul_int_context_struct* node = &_glbl_struct_list;
  tul_net_context* net_ctx = node->this;

  while(net_ctx != NULL && net_ctx->_sock != sock)
  {
    if(node->next != NULL)
    {
      node = node->next;
      net_ctx = node->this;
    }
    else
      return NULL;
  }

  return net_ctx;
}


