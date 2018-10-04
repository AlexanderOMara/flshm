#include <stdio.h>
#include <stdlib.h>

#include <flshm.h>

#include <hexdump.h>

int main(int argc, char ** argv) {

	// Optionally skip null rows in the hex dump.
	int skipNull = argc > 1 ? 1 : 0;

	// Open the shared memory.
	flshm_info * info = flshm_open(false);

	// Check if opened successfully.
	if (!info) {
		printf("FAILED: flshm_open\n");
		return EXIT_FAILURE;
	}

	// Lock memory, to avoid race conditions.
	flshm_lock(info);

	// Dump memory.
	hexdump(info->shmaddr, FLSHM_SIZE, 16, skipNull);

	// Unlock memory.
	flshm_unlock(info);

	// Close info.
	flshm_close(info);

	return EXIT_SUCCESS;
}
