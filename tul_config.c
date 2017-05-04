/***
  Copyright (C) irfan
  Socket thread handler
 **/

#include "tul_log.h"
#include "tul_config.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


/* forward declaration */
char tul_config_context[TUL_MAX_CONFIG_FILE+1] = {0};
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
  bytes_read = fread(tul_config_context, 1, TUL_MAX_CONFIG_FILE+1, fp);
  fclose(fp);
  fp = NULL;

  if(bytes_read > TUL_MAX_FIELD_SIZE)
  {
    tul_log("config file too large. unloading.");
    memset(tul_config_context, 0, TUL_MAX_CONFIG_FILE+1);
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

int parse_config(char *c)
{
  if(!c)
  {
    tul_log("config parse error!");
    return 1;
  }

  return 0;
}

/* read a line from the buffer stop when
 * \n|\r|\0is read
 *
 * return 0 when line read
 * return 1 when end of file
 * return -1 when error
 */
int read_line(char *c, int *offset, char **ret_line)
{
  int cpos = 0;
  char *c_point_h = NULL;
  char *c_point = NULL;
  char *line = NULL;

  /* set pointer to offset length */
  c_point = c;
  for(int i = 0; i < TUL_MAX_LINE_LENGTH+1; i++)
  {
    if(i == *offset)
      break;
    c_point++;
  }

  while(1)
  {
    /* max line length exceeded */
    if(cpos > TUL_MAX_LINE_LENGTH)
      return TUL_CONFIG_ERROR;

    /* line didn't end before end off file */
    if(*offset + cpos >= TUL_MAX_CONFIG_FILE+1)
      return TUL_CONFIG_ERROR;

    /* end of the line */
    if(*c_point == '\n' || *c_point == '\r' || *c_point == '\0')
    {
      line = calloc(1, cpos);
      memcpy(line, c_point_h, cpos);
      *ret_line = line;
      *offset += cpos;
      return cpos;
    }
    else
    {
      c_point++;
      cpos++;
    }

  }

}


