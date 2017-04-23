/***
  Copyright (C) irfan

  Generically handle (ignore) most signals
 **/

#ifndef _tul_signal_handler
#define _tul_signal_handler

void tul_global_signal_handle_init();
void tul_signal_handler_ignore(int i);
void tul_signal_handler_interupt(int i);

#endif
