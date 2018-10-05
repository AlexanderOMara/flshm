#include <stdio.h>
#include <stdlib.h>

#include <flshm.h>

#include <hexdump.h>

int main() {
	flshm_keys * keys = flshm_keys_create();
	flshm_keys_init(keys, false);
	flshm_info * info = flshm_open(keys);

	if (!info) {
		printf("FAILED: flshm_open\n");
		return EXIT_FAILURE;
	}

	int ret = EXIT_SUCCESS;

	// Lock memory, to avoid race conditions.
	flshm_lock(info);

	// Read message.
	flshm_message * message = flshm_message_read(info);

	if (message) {
		printf(
			"Message:\n"
			"    tick: %u\n"
			"    amfl: %u\n"
			"    name: %s\n"
			"    host: %s\n"
			"    version: %i\n"
			"    sandboxed: %i\n"
			"    https: %i\n"
			"    sandbox: %u\n"
			"    swfv: %u\n"
			"    filepath: %s\n"
			"    amfv: %i\n"
			"    method: %s\n"
			"    size: %u\n"
			"    data:\n",
			message->tick,
			message->amfl,
			message->name,
			message->host,
			message->version,
			message->sandboxed,
			message->https,
			message->sandbox,
			message->swfv,
			message->filepath,
			message->amfv,
			message->method,
			message->size
		);
		hexdump(message->data, message->size, 16, 0);
		flshm_message_free(message);
	}
	else {
		printf("FAILED: flshm_message_read\n");
		ret = EXIT_FAILURE;
	}

	// Unlock memory.
	flshm_unlock(info);

	// Close info.
	flshm_close(info);

	// Cleanup memory.
	flshm_keys_destroy(keys);

	return ret;
}
