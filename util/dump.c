#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <flshm.h>

#include <dump.h>

int main(int argc, char ** argv) {
	// Optionally skip null rows in the hex dump.
	bool skip_null = argc > 1 ? (argv[1][0] == '1') : false;

	flshm_keys keys;
	flshm_keys_init(&keys, false);

	flshm_info info;
	if (!flshm_open(&info, &keys)) {
		printf("FAILED: flshm_open\n");
		return EXIT_FAILURE;
	}

	// Lock memory, to avoid race conditions.
	flshm_lock(&info);

	// Dump memory.
	dump_hex(info.shmaddr, FLSHM_SIZE, skip_null);

	// Unlock memory.
	flshm_unlock(&info);

	// Close info.
	flshm_close(&info);

	return EXIT_SUCCESS;
}
