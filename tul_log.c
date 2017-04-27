/***
  Copyright (C) irfan
  logging definition 
 **/

#include <stdio.h>
extern int daemon_mode;

void LOG(char *x)
{
#ifdef SYSLOG_USE
  syslog(LOG_INFO, "%s", x);
#endif  
  if(!daemon_mode)
    fprintf(stdout, "%s\n", x);
}


