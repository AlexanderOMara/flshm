#include <stdio.h>
#include <stdlib.h>

#include "dump.h"

void dump_hex(void * addr, unsigned int size, bool skip_null) {
	char ascii[16 + 1] = {0};
	char hex[(sizeof(ascii) - 1) * 3] = {0};
	int col = sizeof(ascii) - 1;
	char offset[5] = {0};

	unsigned char * bytes = (unsigned char *)addr;
	char * format = "%s  %s  %s\n";
	unsigned int i = 0;
	bool nonnull = 0;
	bool skippingnull = 0;

	for (; i < size; ++i) {
		unsigned int ci = i % col;
		unsigned int hci = ci ? (ci * 3 - 1) : 0;
		if (!ci) {
			if (i) {
				if (skip_null && !nonnull) {
					if (!skippingnull) {
						printf("....\n");
					}
					skippingnull = 1;
				}
				else {
					printf(format, offset, hex, ascii);
					skippingnull = 0;
				}
				nonnull = 0;
			}
			sprintf(offset, "%04X", i);
		}

		// Add hex to the hex column.
		sprintf((char *)(hex + hci), hci ? " %02X" : "%02X", bytes[i]);

		// Add character to the ASCII column.
		unsigned char c = bytes[i];
		ascii[ci] = c > 31 && c < 127 ? c : '.';
		ascii[ci + 1] = '\0';

		// Mark the row as non-null, if the byte is non-null.
		nonnull = c ? 1 : nonnull;
	}

	// Fill the hex column with spaces if not already aligned.
	for (; i % col; ++i) {
		sprintf((char *)(hex + ((i % col) * 3 - 1)), "   ");
	}

	// Output the last line.
	printf(format, offset, hex, ascii);
}

void dump_msg(flshm_message * message) {
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
	dump_hex(message->data, message->size, false);
}
