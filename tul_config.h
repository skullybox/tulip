/***
  Copyright (C) irfan
  Socket thread handler
 **/

#ifndef _tul_config
#define _tul_config

/* max size of config entry */
#define TUL_MAX_FIELD_SIZE 129

/* flags used in tul_flowdef */
#define TUL_AUTH_USER 19
#define TUL_AUTH_TOKEN 21
#define TUL_DEST_USER 39
#define TUL_DEST_DISK 41

#define TUL_MAX_CONFIG_FILE 81920

#define TUL_MAX_LINE_LENGTH 512

#define TUL_CONFIG_ERROR -2


/* structure to hold flow definitions */
typedef struct _flowdef {
  int  auth;
  int  dest;
  char name[TUL_MAX_FIELD_SIZE+1];
  char disk_path[TUL_MAX_LINE_LENGTH+1];
} tul_flowdef;

void load_config();
int parse_config(char *c);
int read_line(char *c, int *offset, char **ret_line);

#endif

