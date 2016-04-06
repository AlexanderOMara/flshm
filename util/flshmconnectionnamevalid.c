#include <stdio.h>
#include <stdlib.h>

#include <flshm.h>

int main(int argc, char ** argv) {

	if (argc < 2) {
		printf("%s name\n", argv[0]);
		return EXIT_FAILURE;
	}

	if (!flshm_connection_name_valid(argv[1])) {
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
