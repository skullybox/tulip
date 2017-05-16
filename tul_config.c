/***
  Copyright (C) irfan
  Socket thread handler
 **/

#include "tul_log.h"
#include "tul_config.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>



extern int TUL_SIGNAL_INT;

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
  char str_tmp[TUL_MAX_LINE_LENGTH+1] = {0};
  int expect_config_ident= 1;
  int ret = 0;
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
    if(expect_config_ident)
    {
      if(get_config_ident(
            ret_line,
            FLOW_LIST[FLOW_COUNT]) == 0)
      {
        expect_config_ident = 0;
      }
      else
      {
        /* error state for identity */
        snprintf(str_tmp, TUL_MAX_LINE_LENGTH, "CONFIG ERROR: %s", ret_line);
        tul_log(str_tmp);
        TUL_SIGNAL_INT = 1;
        return;
      }
    }
    else /* expect config line */
    {
      ret = get_config(ret_line, FLOW_LIST[FLOW_COUNT]);

      if(ret == 1)
      {
        expect_config_ident = 1;
        FLOW_COUNT++;
        goto NEXT_LINE;
      }
      else if(ret == 0)
      {
        goto NEXT_LINE;
      }

      snprintf(str_tmp, TUL_MAX_LINE_LENGTH, "CONFIG ERROR: %s", ret_line);
      tul_log(str_tmp);
      TUL_SIGNAL_INT = 1;
      return;

    }


NEXT_LINE:
    if(ret_line)
      free(ret_line);
    ret_line = NULL;
  }

  snprintf(str_tmp, TUL_MAX_LINE_LENGTH, "read %d flows", FLOW_COUNT);
  tul_log(str_tmp);
}


/* get_config gets the config values
 * after parsing the config identiy
 * return 0 on config parse
 * return 1 on end of config section
 * return -1 on config error
 */
int get_config(char *l, tul_flowdef *f)
{
  char k[TUL_MAX_FIELD_SIZE+1] = {0};
  char v[TUL_MAX_FIELD_SIZE+1] = {0};
  char *ptr = l;
  char *tail = NULL;
  int offset = 0;
  int len = 0;

  if(!ptr)
    return -1;

  len = strlen(l);

  /* get key */
  for(int i = 0; i < len; i++)
  {
    if(l[i] != ' ')
    {
      offset = i;
      ptr = &l[i];
      break;
    }
  }

  /* find offset for end of key */
  for(int i = offset; i < len; i++)
  {
    if(l[i] == ' ')
    {
      offset = i - offset;
      break;
    }
  }

  /* if offset is wrong */
  if(offset <= 0 && *ptr !='}')
    return -1;

  /* copy the key */
  strncpy(k, ptr, offset+1);

  if(*k == '}')
  {
    if(!strcmp("}", k))
      return 1;
    else
      return -1;
  }

  if(offset+1 >= strlen(&ptr[offset]) || ptr[offset] != ' ')
    return -1;

  ptr = &ptr[offset+1];

  if( ptr[strlen(ptr)-1] != ';')
    return -1;

  /* get value of key */
  strncpy(v, ptr, strlen(ptr)-1);

  /* auth user */
  if(!strcmp(k, "auth"))
  {
    if(!strcmp(v, "user"))
    {
      f->auth = TUL_AUTH_USER;      
    }
    else if(!strcmp(v, "token"))
    {
      f->auth = TUL_AUTH_TOKEN;
    }
  }

  /* dest */
  if(!strcmp(k, "dest"))
  {
    if(!strcmp(v, "user"))
    {
      f->auth = TUL_DEST_USER;
    }
    else if(!strncmp(v, "disk:", 5))
    {
      f->auth = TUL_DEST_DISK;
      strcpy(f->disk_path, &v[5]);
    }
  }

  /* IP filter */
  if(!strcmp(k, "ip"))
    strcpy(f->ip, v);



  return 0;
}

int get_config_ident(char *l, tul_flowdef *f)
{
  char *head = NULL;
  int offset = 0;
  int len = strlen(l);

  if(len == 0)
    return -1;

  for(int i = 0; i < len-1; i++)
  {
    if( l[i] != ' ' && head == NULL )
      head = &l[i];

    if( l[i] != ' ' && head != NULL && offset > 0 )
      goto IDENT_ERR_STATE;

    if(l[i] == ' ' && l[i+1] == '{' && (i+1)==(len-1))
    {
      offset = i;
      break;
    }
  }

  if(head == NULL || offset == 0 || offset > TUL_MAX_FIELD_SIZE)
  {
IDENT_ERR_STATE:
    return 1;
  }

  strncpy(f->name, head, offset); 
  return 0;
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

    if(*cur == '\t' || *cur == ' ' || *cur == '\n' || *cur == '\r')
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


