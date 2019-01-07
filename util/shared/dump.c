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
	char amf0_cstr[FLSHM_AMF0_STRING_DECODE_MAX_SIZE];
	printf("Message:\n");
	printf("    tick: %u\n", message->tick);
	printf("    amfl: %u\n", message->amfl);
	flshm_amf0_decode_string_cstr(&message->name, amf0_cstr, false);
	printf("    name: %s\n", amf0_cstr);
	flshm_amf0_decode_string_cstr(&message->host, amf0_cstr, false);
	printf("    host: %s\n", amf0_cstr);
	printf("    version: %i\n", message->version);
	printf("    sandboxed: %i\n", message->sandboxed);
	printf("    https: %i\n", message->https);
	printf("    sandbox: %u\n", message->sandbox);
	printf("    swfv: %u\n", message->swfv);
	flshm_amf0_decode_string_cstr(&message->filepath, amf0_cstr, false);
	printf("    filepath: %s\n", amf0_cstr);
	printf("    amfv: %i\n", message->amfv);
	flshm_amf0_decode_string_cstr(&message->method, amf0_cstr, false);
	printf("    method: %s\n", amf0_cstr);
	printf("    size: %u\n", message->size);
	printf("    data:\n");
	dump_hex(message->data, message->size, false);
}

void dump_str(void * addr, int size) {
	unsigned char * p = (unsigned char *)addr;
	bool sized = size >= 0;
	for (int i = 0;; i++) {
		if (sized && i >= size) {
			break;
		}
		else if (!*p) {
			break;
		}
	}
}
