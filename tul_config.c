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
tul_flowdef *FLOW_LIST[TUL_MAX_FLOW_LIST] = {0};
int FLOW_COUNT = 0;
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

  if(bytes_read > TUL_MAX_CONFIG_FILE)
  {
    tul_log("config file too large. unloading.");
    memset(tul_config_context, 0, TUL_MAX_CONFIG_FILE+1);
    return;
  }
  else
    tul_log("configuration loaded"); 

  generate_behavior(tul_config_context);
}


void generate_behavior(char *c)
{
  int offset = 0;
  char *ret_line = NULL;


  if(!c)
    return;

  while(!read_line(c, &offset, &ret_line))
  {

    /* config line check */
    if(strlen(ret_line) > TUL_MAX_FIELD_SIZE)
    {
      tul_log("config line length exceeded!");

      if(ret_line)
        free(ret_line);
      ret_line = NULL;
    }

    /* process line */
    if(!FLOW_LIST[FLOW_COUNT])
      FLOW_LIST[FLOW_COUNT] = malloc(sizeof(tul_flowdef));

    /* check if comment line */
    if(comment_line(ret_line))
      goto NEXT_LINE;

    /* remove extra spaces from line */
    remove_extra_spaces(ret_line);
    if(!strlen(ret_line))
      goto NEXT_LINE;

    /* process instruction line */


NEXT_LINE:
    if(ret_line)
      free(ret_line);
    ret_line = NULL;
  }
}

void remove_extra_spaces(char *l)
{
  char *space_pos = NULL;
  int space_found = 0;
  char *tmp = NULL;
  char *cur = l;
  char *cur_new = NULL;
  int len = 0;

  if(!cur)
    return;

  len = strlen(l);

  tmp = calloc(1, len);
  cur_new = tmp;

  for(int i = 0; i < len; i++)
  {
    if(*cur == ';')
    {
      *cur_new = ';';
      cur_new++;
      break;
    }

    if(*cur == '\t' || *cur == ' ' || *cur == '\t' || *cur == '\r')
    {
      if(!space_found)
      {
        *cur_new = ' ';
        cur_new++;
      }
      space_found = 1;
      cur++;
    }
    else
    {
      space_found = 0;
      *cur_new = *cur;
      cur_new++;
      cur++;
    }

  }
  if(strlen(tmp))
    strcpy(l, tmp);
  free(tmp);
  tmp = NULL;
}


int comment_line(char *l)
{
  int char_flag = 0;
  int len = strlen(l);

  for(int i = 0; i < len; i++)
  {
    if(l[i] == ';' && !char_flag)
      return 1;

    if(l[i] != ' ' && l[i] != ';' && 
        l[i] !='\t' && l[i] != '\r' && l[i] != '\n')
      return 0;
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

  /* make sure ret_line is null */
  if(*ret_line)
    return TUL_CONFIG_ERROR;

  /* set pointer to offset length */
  c_point = c;
  for(int i = 0; i < TUL_MAX_CONFIG_FILE  +1; i++)
  {
    if(i == *offset)
      break;
    c_point++;
  }

  c_point_h = c_point;
  while(1)
  {
    /* max line length exceeded */
    if(cpos > TUL_MAX_CONFIG_FILE)
      return TUL_CONFIG_ERROR;

    /* line didn't end before end off file */
    if(*offset + cpos > TUL_MAX_CONFIG_FILE)
      return TUL_CONFIG_ERROR;

    /* end of the line */
    if(*c_point == '\n' || *c_point == '\r' || *c_point == '\0')
    {
      line = calloc(1, cpos);
      memcpy(line, c_point_h, cpos);
      *ret_line = line;

      cpos++;
      *offset += cpos;

      if( *c_point == '\n' || *c_point == '\r')
        return TUL_CONFIG_LINE;

      if( *c_point == '\0' )
        return TUL_CONFIG_EOF;
    }
    else
    {
      c_point++;
      cpos++;
    }

  }
}


