/***
  Copyright (C) irfan

 **/

 #include "tul_globals.h"
 #include "tul_net_context.h"

static _tul_int_context_struct _glbl_struct_list;
pthread_mutex_t _glbl_struct_mtx;

 void tul_add_context(unsigned sock)
 {
   pthread_mutex_lock(&_glbl_struct_mtx);
   _tul_int_context_struct *new = NULL;
   _tul_int_context_struct *cur = NULL;

   /* find the end of the list */
   while(cur->next != NULL && cur->this->_sock != sock)
   {
     cur = cur->next;
   }

   /*
    * this should never happen.
    * we should never try to add a socket already in the list
    */
   if(cur->this->_sock == sock)
   {
     #ifdef SYSLOG_USE
     syslog(LOG_WARNING, "%s", "skipping adding socket; already in list");
     #else
     fprintf(stderr, "LOG_WARNING: %s\n", "skipping adding socket; \
     already in list");
     #endif
   }
   else /* we add the network context here */
   {
     new = (_tul_int_context_struct *)calloc(1, sizeof(_tul_int_context_struct));
     new->this=(tul_net_context*)calloc(1, sizeof(tul_net_context));
     new->this->_sock = sock;
     cur->next = new;
     new->back = cur;
   }
   pthread_mutex_unlock(&_glbl_struct_mtx);
 }

 void tul_rem_context(unsigned sock)
 {
   pthread_mutex_lock(&_glbl_struct_mtx);
    _tul_int_context_struct *cur = NULL;
   while(cur->next != NULL && cur->this->_sock != sock)
   {
     cur = cur->next;
   }

   if(cur->this->_sock == sock)
   {
     cur->back->next = cur->next;
     cur->next->back = cur->back;

     /* close the socket */
     close(cur->this->_sock);
     memset(cur->this, 0, sizeof(tul_net_context));
     free(cur->this);
     memset(cur, 0, sizeof(_tul_int_context_struct));
     free(cur);
   }
   pthread_mutex_unlock(&_glbl_struct_mtx);
 }

 void tul_init_context_list()
 {
   pthread_mutex_lock(&_glbl_struct_mtx);
   memset(&_glbl_struct_list, 0, sizeof(_tul_int_context_struct));
   #ifdef SYSLOG_USE
   syslog(LOG_INFO, "%s", "starting net context");
   #else
   fprintf(stderr, "LOG_INFO: %s\n", "starting net context");
   #endif
   pthread_mutex_unlock(&_glbl_struct_mtx);
 }


 void tul_dest_context_list()
 {
   pthread_mutex_lock(&_glbl_struct_mtx);

   _tul_int_context_struct *cur = NULL;
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
     close(cur->this->_sock);
     memset(cur->this, 0, sizeof(tul_net_context));
     free(cur->this);
     memset(cur, 0, sizeof(_tul_int_context_struct));
     free(cur);
     cur = tmp;
   }
   #ifdef SYSLOG_USE
   syslog(LOG_INFO, "%s", "stopping net context");
   #else
   fprintf(stderr, "LOG_INFO: %s\n", "stopping net context");
   #endif
   pthread_mutex_unlock(&_glbl_struct_mtx);
 }