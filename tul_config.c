/***
  Copyright (C) irfan
  Socket thread handler
 **/

#include "tul_log.h"
#include "tul_config.h"
#include <stdio.h>
#include <string.h>


/* forward declaration */
char tul_config_context[819201] = {0};
void generate_behavior(char *c);

void load_config()
{
  size_t bytes_read = 0;
  char *fp_buff = NULL;
  FILE *fp = fopen("./config.cfg", "r");
  
  /* config failed to open */
  if(!fp)
  {
    tul_log("warning config file ./config.cfg failed to open");
    return;
  }

  /* load config context here */
  bytes_read = fread(tul_config_context, 1, 819201, fp);
  fclose(fp);
  fp = NULL;

  if(bytes_read > 819200)
  {
    tul_log("config file too large. unloading.");
    memset(tul_config_context, 0, 819201);
  }
  else
  {
    tul_log("configuration loaded"); 
    generate_behavior(tul_config_context);
  }

}


void generate_behavior(char *c)
{
  if(!c)
    return;
}

