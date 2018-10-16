#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <flshm.h>

int main(int argc, char ** argv) {
	bool locking = argc < 2 ? false : argv[1][0] == '1';

	flshm_keys keys;
	flshm_keys_init(&keys, false);

	flshm_info info;
	if (!flshm_open(&info, &keys)) {
		printf("FAILED: flshm_open\n");
		return EXIT_FAILURE;
	}

	if (locking) {
		flshm_lock(&info);
	}

	printf("Press enter to close.\n");
	getchar();

	if (locking) {
		flshm_unlock(&info);
	}

	// Close info.
	flshm_close(&info);

	return EXIT_SUCCESS;
}
