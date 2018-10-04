#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <signal.h>
#include <time.h>

#include <flshm.h>

#include <hexdump.h>

uint32_t amf0_read_string(char ** str, char * p, uint32_t max) {

	// Bounds check the header.
	if (max < 3) {
		return false;
	}

	// Check the type marker.
	if (*p != '\x02') {
		return false;
	}
	p++;

	// Read the string length, big endian.
	uint16_t sl =
		((*((uint8_t *)p + 1)     ) & 0xFF  ) |
		((*((uint8_t *)p    ) << 8) & 0xFF00);
	p += 2;

	// Compute total data size.
	uint32_t size = sl + 3;

	// Bounds check the length.
	if (size > max) {
		return false;
	}

	// Copy string to memory, null terminate.
	*str = malloc(sl + 1);
	memcpy(*str, p, sl);
	(*str)[sl] = '\0';

	// Return the amout of data read.
	return size;
}

uint32_t amf0_write_string(char * str, char * p, uint32_t max) {

	// Get string length and bounds check.
	size_t l = strlen(str);
	uint16_t sl = l;
	uint32_t size = l + 3;
	if (l > 0xFFFF || size > max) {
		return 0;
	}

	// Write the string marker.
	*p = '\x02';
	p++;

	// Write the string size, big engian.
	*((uint8_t *)p    ) = (sl >> 8) & 0xFF;
	*((uint8_t *)p + 1) = (sl     ) & 0xFF;
	p += 2;

	// Copy the string without null byte.
	memcpy(p, str, sl);

	return size;
}

void dump_message(flshm_message * message) {
	printf(
		"Message:\n"
		"    tick: %u\n"
		"    amfl: %u\n"
		"    name: %s\n"
		"    host: %s\n"
		"    version: %i\n"
		"    sandboxed: %i\n"
		"    https: %i\n"
		"    sandbox: %u\n"
		"    swfv: %u\n"
		"    filepath: %s\n"
		"    amfv: %i\n"
		"    method: %s\n"
		"    size: %u\n"
		"    data:\n",
		message->tick,
		message->amfl,
		message->name,
		message->host,
		message->version,
		message->sandboxed,
		message->https,
		message->sandbox,
		message->swfv,
		message->filepath,
		message->amfv,
		message->method,
		message->size
	);
	hexdump(message->data, message->size, 16, 0);
}

void strinv(char * s) {
	for (uint32_t i = 0; s[i] != '\0'; i++) {
		char c = s[i];
		if (c >= 'a' && c <= 'z') {
			s[i] = s[i] - 32;
		}
		else if (c >= 'A' && c <= 'Z') {
			s[i] = s[i] + 32;
		}
	}
}

static flshm_info * info = NULL;
static flshm_connection connection = { NULL, 0, 0 };
static bool locked = false;

static void onshutdown(int signo) {
	printf("\nCleaning up...\n");
	if (info) {
		if (!locked && connection.name) {
			flshm_lock(info);
			flshm_connection_remove(info, connection);
		}
		flshm_unlock(info);
		flshm_close(info);
	}
	printf("Shutting down...\n");
	exit(signo == SIGINT ? EXIT_SUCCESS : EXIT_FAILURE);
}

int main(int argc, char ** argv) {

	if (argc < 3) {
		printf(
			"%s "
			"connection_name_self "
			"connection_name_peer "
			"debug? "
			"is_per_user?\n",
			argv[0]
		);
		return EXIT_FAILURE;
	}

	if (!flshm_connection_name_valid(argv[1])) {
		printf("Invalid connection_name_self: %s\n", argv[1]);
		return EXIT_FAILURE;
	}
	if (!flshm_connection_name_valid(argv[2])) {
		printf("Invalid connection_name_peer: %s\n", argv[2]);
		return EXIT_FAILURE;
	}

	char * connection_name_self = argv[1];
	char * connection_name_peer = argv[2];
	bool debug = argc < 4 ? false : argv[3][0] == '1';
	bool is_per_user = argc < 5 ? false : argv[4][0] == '1';

	// Register a shutdown handler, to cleanup everything on Ctrl+C etc.
	int signals[] = {
		SIGABRT,
		SIGFPE,
		SIGILL,
		SIGINT,
		SIGSEGV,
		SIGTERM
	};
	for (size_t i = 0; i < (sizeof(signals) / sizeof(int)); i++) {
		if (signal(signals[i], onshutdown) == SIG_ERR) {
			printf("FAILED: signal: %i\n", signals[i]);
		}
	}

	info = flshm_open(is_per_user);
	if (!info) {
		printf("FAILED: flshm_open\n");
		return EXIT_FAILURE;
	}

	// Register the connection name or fail.
	flshm_lock(info);
	connection.name = connection_name_self;
	connection.version = FLSHM_VERSION_3;
	connection.sandbox = FLSHM_SECURITY_LOCAL_TRUSTED;
	if (!flshm_connection_add(info, connection)) {
		printf("FAILED: flshm_connection_add\n");
		flshm_unlock(info);
		flshm_close(info);
		return EXIT_FAILURE;
	}
	flshm_unlock(info);

	printf("Chatbot runnning...\n");

	// Run loop.
	while (true) {
		flshm_lock(info);
		locked = true;

		// Read message if present.
		flshm_message * message = flshm_message_read(info);
		if (message) {

			// Check that this message is intended for this.
			if (!strcmp(connection_name_self, message->name)) {

				// Clear the message from the memory.
				flshm_message_clear(info);

				// Show debug info for the message.
				if (debug) {
					dump_message(message);
				}

				// Read the data as AMF0 string if possible.
				char * msgstr = NULL;
				if (amf0_read_string(&msgstr, message->data, message->size)) {

					// Print the parsed string.
					printf("Received: %s\n", msgstr);

					// Invert the character cases.
					strinv(msgstr);

					// Generate tick, loop if still same.
					uint32_t tick;
					do {
						tick = flshm_tick();
					}
					while (tick == message->tick);

					// Create a buffer for the data, and write to it.
					uint32_t max = 3 + strlen(msgstr);
					char * data = malloc(max);
					uint32_t size;
					if ((size = amf0_write_string(msgstr, data, max))) {

						// Create a new filepath from the existing one.
						char * filepath = NULL;
						if (message->filepath) {
							uint32_t fpl = strlen(message->filepath);
							char append[] = ".chatbot.swf";
							filepath = malloc(fpl + sizeof(append));
							memcpy(filepath, message->filepath, fpl);
							memcpy(filepath + fpl, append, sizeof(append));
						}

						// Create the response data, mimick sender.
						flshm_message response;
						response.tick = tick;
						response.name = connection_name_peer;
						response.host = message->host;
						response.version = message->version;
						response.sandboxed = message->sandboxed;
						response.https = message->https;
						response.sandbox = message->sandbox;
						response.swfv = message->swfv;
						response.filepath = filepath;
						response.amfv = FLSHM_AMF0;
						response.method = message->method;
						response.data = data;
						response.size = size;

						// Write the message to shared memory.
						// In theory, should poll the tick to ensure is read.
						// If not read in set timout, then erase to free.
						if (!flshm_message_write(info, &response)) {
							printf("FAILED: flshm_message_write\n");
						}

						// Show debug info for the response.
						if (debug) {
							dump_message(&response);
						}

						// Print the response string.
						printf("Response: %s\n", msgstr);

						// Free filepath if allocated.
						if (filepath) {
							free(filepath);
						}
					}

					// Free the data buffer.
					free(data);

					// Free the parsed string.
					free(msgstr);
				}
			}

			// Free the memory from the message.
			flshm_message_free(message);
		}

		flshm_unlock(info);
		locked = false;

		struct timespec tim;
		struct timespec tim2;
		tim.tv_sec = 0;
		tim.tv_nsec = 10000000L;
		nanosleep(&tim, &tim2);
	}

	flshm_close(info);

	return EXIT_SUCCESS;
}
