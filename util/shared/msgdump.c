#include <stdio.h>

#include <hexdump.h>

#include "msgdump.h"

void msgdump(flshm_message * message) {
	printf("Message:\n");
	printf("    tick: %u\n", message->tick);
	printf("    amfl: %u\n", message->amfl);
	printf("    name: %s\n", message->name);
	printf("    host: %s\n", message->host);
	printf("    version: %i\n", message->version);
	printf("    sandboxed: %i\n", message->sandboxed);
	printf("    https: %i\n", message->https);
	printf("    sandbox: %u\n", message->sandbox);
	printf("    swfv: %u\n", message->swfv);
	printf("    filepath: %s\n", message->filepath);
	printf("    amfv: %i\n", message->amfv);
	printf("    method: %s\n", message->method);
	printf("    size: %u\n", message->size);
	printf("    data:\n");
	hexdump(message->data, message->size, 16, 0);
}
