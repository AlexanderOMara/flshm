#include <stdio.h>
#include <stdlib.h>

#include <flshm.h>

int main() {

	// Read the current tick.
	uint32_t last_tick = 0;
	for (;;) {
		uint32_t tick = flshm_tick();
		if (tick != last_tick) {
			last_tick = tick;
			printf("tick: %u\n", tick);
		}
	}

	return EXIT_SUCCESS;
}
