/***
  Copyright (C) irfan
  C object emmiter/parser
 **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

short count = 0;
char *objectDefs[500] = {0};
char *readbuf = NULL;
ssize_t bytes_read = 0;


void cleanup();
void emitObjects();

int main(int argc, char **argv)
{
  FILE *fp = NULL;
  if(argc <= 1)
  {
    printf("usage: obj_emit [object file]\n");
    return 1;
  }

  fp = fopen(argv[1], "r");
  if(!fp)
  {
    fprintf(stderr, "File IO Error!\n");
  }

  /* load object definitions into memory */
  while( ( bytes_read = getline(&readbuf, &bytes_read, fp) ) != -1 )
  {
    /* skip lines beginning with \n or \r */
    if( bytes_read > 0 && (readbuf[0] == '\n' || readbuf[0] == '\r' ))
    {
      free(readbuf);
      readbuf = NULL;
      continue;

    }

    /* validate the object name def. */
    if( bytes_read > 0 && readbuf[0] == '-' && 
        (readbuf[1] < 'A' || readbuf[1] > 'z'))
    {
      free(readbuf);
      readbuf = NULL;
      continue;
    }

    /* skip lines beginning with spaces */
    if( bytes_read > 0 && ( readbuf[0] == ' ' && (readbuf[0] == '\n'
            || readbuf[0] == '\r' || readbuf[0] == '-') ) )
    {
      free(readbuf);
      readbuf = NULL;
      continue;
    }

    /* skip comment lines */
    if( bytes_read > 0 && readbuf[0] == '#' )
    {
      free(readbuf);
      readbuf = NULL;
      continue;
    }

    /* defining new object */
    if( bytes_read > 0 )
    {
      objectDefs[count] = readbuf;
      objectDefs[count][strlen(objectDefs[count])-1] = '\0';
      readbuf = NULL;
      count++;
    }
  }

  /* print parsed contents */
  for( int i = 0; i < count; i++)
  {
    if( objectDefs[i][0] == '-' )
      printf("\n");
    printf("%s\n", objectDefs[i]);  
  }

  fclose(fp);
  fp = NULL;

  emitObjects();

  cleanup();
  return 0;
}

void cleanup()
{
  for( int i = 0; i < count; i++)
  {
    free(objectDefs[i]);
    objectDefs[i] = NULL;
  }
}

void emitObjects()
{

}

