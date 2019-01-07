#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <flshm.h>

int main(int argc, const char ** argv) {
	if (argc < 4) {
		printf("%s name version sandbox\n", argv[0]);
		return EXIT_FAILURE;
	}

	flshm_keys keys;
	flshm_keys_init(&keys, false);

	flshm_info info;
	if (!flshm_open(&info, &keys)) {
		printf("FAILED: flshm_open\n");
		return EXIT_FAILURE;
	}

	flshm_connection connection;
	strcpy(connection.name, argv[1]);
	connection.version = argv[2][0] - '0';
	connection.sandbox = argv[3][0] - '0';

	int ret = EXIT_SUCCESS;

	if (!flshm_connection_remove(&info, &connection)) {
		ret = EXIT_FAILURE;
	}

	// Close info.
	flshm_close(&info);

	return ret;
}
