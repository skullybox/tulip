/***
  Copyright (C) irfan
  Socket thread handler
 **/

#ifndef _tul_config
#define _tul_config

/* max size of config entry */
static const int tul_max_field_size = 129;

/* flags used in tul_flowdef */
static const int tul_auth_user = 19;

static const int tul_auth_token = 21;

static const int tul_dest_user = 39;

static const int tul_dest_disk = 41;


/* structure to hold flow definitions */
typedef struct _flowdef {
  char *name;
  int  auth;
  int  dest;
  char disk_path[512];
} tul_flowdef;

void load_config();
int parse_config(char *c);
int read_line(char *c, int *offset);

#endif

