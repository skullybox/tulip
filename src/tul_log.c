/***
  Copyright (C) irfan
  logging definition 
 **/

#include <stdio.h>
#include <syslog.h>
extern int daemon_mode;
extern int RUN_TESTS;

void tul_log(char *x)
{
#ifdef SYSLOG_USE
  syslog(LOG_INFO, "%s", x);
#endif  
  if(!daemon_mode || !RUN_TESTS)
    fprintf(stdout, "%s\n", x);
}


