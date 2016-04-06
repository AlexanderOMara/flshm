#include <stdio.h>
#include <stdlib.h>

#include <flshm.h>

int main(int argc, char ** argv) {

	// Read the current tick.
	uint32_t last_tick = 0;
	while (1) {
		uint32_t tick = flshm_tick();
		if (tick != last_tick) {
			last_tick = tick;
			printf("tick: %u\n", tick);
		}
	}

	return EXIT_SUCCESS;
}
