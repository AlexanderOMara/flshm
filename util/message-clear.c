#include <stdio.h>
#include <stdlib.h>

#include <flshm.h>

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

	// Clear message.
	flshm_message_clear(info);

	// Unlock memory.
	flshm_unlock(info);

	// Close info.
	flshm_close(info);

	// Cleanup memory.
	flshm_keys_destroy(keys);

	return ret;
}
