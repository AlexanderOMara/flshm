#include <stdio.h>
#include <stdlib.h>

#include <flshm.h>

int main() {
	flshm_keys * keys = flshm_keys_create(false);
	flshm_info * info = flshm_open(keys);

	if (!info) {
		printf("FAILED: flshm_open\n");
		return EXIT_FAILURE;
	}

	int ret = EXIT_SUCCESS;

	// Lock memory, to avoid race conditions.
	flshm_lock(info);

	// Read the current tick.
	uint32_t tick = flshm_message_tick(info);
	printf("tick: %u\n", tick);

	// Unlock memory.
	flshm_unlock(info);

	// Close info.
	flshm_close(info);

	// Cleanup memory.
	flshm_keys_destroy(keys);

	return ret;
}
