#include <stdio.h>
#include <stdlib.h>

#include <flshm.h>

#include <dump.h>

int main() {
	flshm_keys keys;
	flshm_keys_init(&keys, false);

	flshm_info info;
	if (!flshm_open(&info, &keys)) {
		printf("FAILED: flshm_open\n");
		return EXIT_FAILURE;
	}

	int ret = EXIT_SUCCESS;

	flshm_message message;

	// Lock memory, to avoid race conditions.
	flshm_lock(&info);

	// Read message.
	if (flshm_message_read(&info, &message)) {
		dump_msg(&message);
	}
	else {
		printf("FAILED: flshm_message_read\n");
		ret = EXIT_FAILURE;
	}

	// Unlock memory.
	flshm_unlock(&info);

	// Close info.
	flshm_close(&info);

	return ret;
}
