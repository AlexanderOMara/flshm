#ifndef _MSGDUMP_H
#define _MSGDUMP_H

#include <stdbool.h>

#include <flshm.h>

void dump_hex(const void * addr, unsigned int size, bool skip_null);

void dump_msg(const flshm_message * message);

void dump_str(const void * addr, int size);

#endif
