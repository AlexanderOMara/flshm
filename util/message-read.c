#include <stdio.h>
#include <stdlib.h>

#include <flshm.h>

#include <hexdump.h>
#include <msgdump.h>

int main() {
	flshm_keys * keys = flshm_keys_create(false);
	flshm_info * info = flshm_open(keys);

	if (!info) {
		printf("FAILED: flshm_open\n");
		return EXIT_FAILURE;
	}

	int ret = EXIT_SUCCESS;

	// Create message.
	flshm_message * message = flshm_message_create();

	// Lock memory, to avoid race conditions.
	flshm_lock(info);

	// Read message.
	if (flshm_message_read(info, message)) {
		msgdump(message);
	}
	else {
		printf("FAILED: flshm_message_read\n");
		ret = EXIT_FAILURE;
	}

	// Unlock memory.
	flshm_unlock(info);

	// Cleanup message.
	flshm_message_destroy(message);

	// Close info.
	flshm_close(info);

	// Cleanup memory.
	flshm_keys_destroy(keys);

	return ret;
}
