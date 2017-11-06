/***
  Copyright (C) irfan
  user functions
 **/


#include "tul_user.h"
#include "tul_mysql.h"
#include <stdio.h>
#include <string.h>

int daemon_mode = 0;
int RUN_TESTS = 0;

void usage();
int main(int argc, char **argv)
{

  char uid[30] = {0};
  char name[51] = {0};
  char email[51] = {0};
  char pass[17] = {0};
  int ret = 0;
  unsigned count = 0;
  char ch = '0';
  char buff[17] = {0};

  if(argc < 3)
  {
    usage();
    return -1;
  }

  strncpy(uid, argv[1], 30);
  strncpy(email, argv[2], 50);

  fprintf(stdout, "password: ");
  while(count < 17)
  {
    ch = getchar();
    if(ch == EOF || ch == '\n')
      break;

    buff[count] = ch;
    count++;

  }

  strncpy(pass, buff, 16);
  tul_dbinit();
  ret = create_user(uid, email, pass);

  if(!ret)
  {
    fprintf(stdout, "Created user: %s (%s)\n", uid, name);
  }
  else {
    fprintf(stderr, "Error creating user: %s (%s)\n", uid, name);
  }

  return ret;
}


void usage()
{
    fprintf(stdout, "t_adduser <username> <email>\n");
    fprintf(stdout, "returns: \n");
    fprintf(stdout, "\t %s\n", "1 - uid taken");
    fprintf(stdout, "\t %s\n", "2 - email taken");
    fprintf(stdout, "\t %s\n", "3 - uid length");
    fprintf(stdout, "\t %s\n", "4 - email length");
    fprintf(stdout, "\t %s\n", "5 - pass length");
    fprintf(stdout, "\t %s\n", "6 - db error");

}


