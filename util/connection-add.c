#include <stdio.h>
#include <stdlib.h>

#include <flshm.h>

int main(int argc, char ** argv) {
	if (argc < 2) {
		printf("%s name version? sandbox?\n", argv[0]);
		return EXIT_FAILURE;
	}

	flshm_keys keys = flshm_get_keys(false);
	flshm_info * info = flshm_open(&keys);

	if (!info) {
		printf("FAILED: flshm_open\n");
		return EXIT_FAILURE;
	}

	flshm_connection connection;
	connection.name = argv[1];
	connection.version = argc > 2 ?
		argv[2][0] - (uint8_t)'0' :
		FLSHM_VERSION_1;
	connection.sandbox = argc > 3 ?
		argv[3][0] - (uint8_t)'0' :
		FLSHM_SECURITY_NONE;

	int ret = EXIT_SUCCESS;

	if (!flshm_connection_add(info, connection)) {
		ret = EXIT_FAILURE;
	}

	flshm_close(info);

	return ret;
}
