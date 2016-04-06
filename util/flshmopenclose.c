#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <flshm.h>

int main(int argc, char ** argv) {

	bool locking = argc < 2 ? false : argv[1][0] == '1';

	flshm_info * info = flshm_open(false);

	if (!info) {
		printf("FAILED: flshm_open\n");
		return EXIT_FAILURE;
	}

	if (locking) {
		flshm_lock(info);
	}

	printf("Press enter to close.\n");
	getchar();

	if (locking) {
		flshm_unlock(info);
	}

	flshm_close(info);

	return EXIT_SUCCESS;
}
