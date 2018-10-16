#ifndef _MSGDUMP_H
#define _MSGDUMP_H

#include <stdbool.h>

#include <flshm.h>

void dump_hex(void * addr, unsigned int size, bool skip_null);

void dump_msg(flshm_message * message);

#endif
