#include <stdio.h>
#include <stdlib.h>

#include <flshm.h>

#include <dump.h>

int main(int argc, char ** argv) {
	// Optionally skip null rows in the hex dump.
	int skipNull = argc > 1 ? (argv[1][0] == '1') : 0;

	// Open the shared memory.
	flshm_keys * keys = flshm_keys_create(false);
	flshm_info * info = flshm_open(keys);

	// Check if opened successfully.
	if (!info) {
		printf("FAILED: flshm_open\n");
		return EXIT_FAILURE;
	}

	// Lock memory, to avoid race conditions.
	flshm_lock(info);

	// Dump memory.
	dump_hex(info->shmaddr, FLSHM_SIZE, 16, skipNull);

	// Unlock memory.
	flshm_unlock(info);

	// Close info.
	flshm_close(info);

	// Cleanup memory.
	flshm_keys_destroy(keys);

	return EXIT_SUCCESS;
}
