/***
  Copyright (C) irfan
  common module functions
 **/

#ifndef _tul_module_common
#define _tul_module_common

#include "tul_user.h"
#include "tul_module.h"
#include "tul_net_context.h"


#define PAYLOAD_CHECK_SZ if(p.data_sz > MAX_REQ_PAYLOAD - sizeof(comm_payload)) return 1;

int prep_transmission(char *uid, char *pass, 
    comm_req *r, comm_payload *p, tul_net_context *conn);

int verify_payload(tul_net_context *c, comm_req *r, comm_payload *p);

#endif
