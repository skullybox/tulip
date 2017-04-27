/***
  Copyright (C) irfan
  Generically handle (ignore) most signals
 **/

#include "tul_log.h"
#include "tul_globals.h"
#include "tul_signal_handler.h"

int TUL_SIGNAL_INT = 0;

void tul_global_signal_handle_init()
{

  LOG("adding signal handlers");

  for(int i = 1; i < 32; i++)
  {
    if(i == SIGINT)
    {
      signal(i, tul_signal_handler_interupt);
    }
    else
    {
      signal(i, tul_signal_handler_ignore);
    }
  }
}

void tul_signal_handler_ignore(int i)
{
  return;
}


void tul_signal_handler_interupt(int i)
{
  if(!TUL_SIGNAL_INT)
  {
    TUL_SIGNAL_INT = 1;
    LOG("signal to shutdown");
  }
}
