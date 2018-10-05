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

	// Lock memory, to avoid race conditions.
	flshm_lock(info);

	flshm_connected connected = flshm_connection_list(info);
	printf("Connections: %i\n", connected.count);
	for (uint32_t i = 0; i < connected.count; i++) {
		flshm_connection c = connected.connections[i];
		printf(
			"    %i:  name:%s  version:%i  sandbox:%i\n",
			i,
			c.name,
			c.version,
			c.sandbox
		);
	}

	// Unlock memory.
	flshm_unlock(info);

	// Close info.
	flshm_close(info);

	// Cleanup memory.
	flshm_keys_destroy(keys);

	return EXIT_SUCCESS;
}
