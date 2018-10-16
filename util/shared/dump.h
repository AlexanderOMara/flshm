#ifndef _MSGDUMP_H
#define _MSGDUMP_H

#include <flshm.h>

void dump_hex(void * addr, unsigned int size, unsigned int col, int skipnull);

void dump_msg(flshm_message * message);

#endif
