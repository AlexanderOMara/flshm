#include <stdio.h>
#include <stdlib.h>

#include <flshm.h>

int main(int argc, char ** argv) {

	flshm_info * info = flshm_open(false);

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

	flshm_close(info);

	return ret;
}
