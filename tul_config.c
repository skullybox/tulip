/***
  Copyright (C) irfan
  Socket thread handler
 **/

#include "tul_log.h"
#include "tul_config.h"
#include <stdio.h>


/* forward declaration */
char tul_config_context[81920000] = {0};
void generate_behavior(char *c);

void load_config()
{
  char *fp_buff = NULL;
  FILE *fp = fopen("./config", "r");
  
  /* config failed to open */
  if(!fp)
  {
    tul_log("warning config file ./config.cfg failed to open");
    return;
  }

  /* load config context here */
  fread(tul_config_context, 1, 81920000, fp);
  fclose(fp);
  fp = NULL;
  tul_log("configuration loaded"); 

  generate_behavior(tul_config_context);

}


void generate_behavior(char *c)
{
  if(!c)
    return;
}

