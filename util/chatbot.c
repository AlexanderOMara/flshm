#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <signal.h>
#include <time.h>

#include <flshm.h>

#include <hexdump.h>
#include <sleep.h>

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

static int signals[] = {
	SIGABRT,
	SIGFPE,
	SIGILL,
	SIGINT,
	SIGSEGV,
	SIGTERM
};
static flshm_keys * keys = NULL;
static flshm_info * info = NULL;
static flshm_connection connection;
static bool locked = false;
static flshm_message * message = NULL;
static flshm_message * response = NULL;
static char filepath_append[] = ".chatbot.swf";

static void filepath_create(char * dest, char * src) {
	dest[0] = '\0';

	if (!src[0]) {
		return;
	}

	size_t len = strlen(src);
	if (len + (sizeof(filepath_append) - 1) > FLSHM_AMF0_STRING_MAX_LENGTH) {
		return;
	}

	strcpy(dest, src);
	strcat(dest, filepath_append);
}

static void cleanup(void) {
	printf("\nCleaning up...\n");
	if (info) {
		if (!locked) {
			flshm_lock(info);
			flshm_connection_remove(info, &connection);
		}
		flshm_unlock(info);
	}
	if (response) {
		flshm_message_destroy(response);
		response = NULL;
	}
	if (message) {
		flshm_message_destroy(message);
		message = NULL;
	}
	if (info) {
		flshm_close(info);
		info = NULL;
	}
	if (keys) {
		flshm_keys_destroy(keys);
		keys = NULL;
	}
}

static void onshutdown(int signo) {
	cleanup();
	printf("Shutting down...\n");
	exit(signo == SIGINT ? EXIT_SUCCESS : EXIT_FAILURE);
}

static void register_shutdown() {
	// Register a shutdown handler, to cleanup everything on Ctrl+C etc.
	for (size_t i = 0; i < (sizeof(signals) / sizeof(int)); i++) {
		if (signal(signals[i], onshutdown) == SIG_ERR) {
			printf("FAILED: signal: %i\n", signals[i]);
		}
	}
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

	strcpy(connection.name, connection_name_self);
	connection.version = FLSHM_VERSION_3;
	connection.sandbox = FLSHM_SECURITY_LOCAL_TRUSTED;

	register_shutdown();

	keys = flshm_keys_create(is_per_user);
	info = flshm_open(keys);
	if (!info) {
		printf("FAILED: flshm_open\n");
		return EXIT_FAILURE;
	}

	// Register the connection name or fail.
	flshm_lock(info);
	if (!flshm_connection_add(info, &connection)) {
		printf("FAILED: flshm_connection_add\n");
		flshm_unlock(info);
		flshm_close(info);
		return EXIT_FAILURE;
	}
	flshm_unlock(info);

	message = flshm_message_create();
	response = flshm_message_create();

	char msgstr[FLSHM_AMF0_STRING_MAX_SIZE];

	printf("Chatbot runnning...\n");

	// Run loop.
	bool running = true;
	while (running) {
		flshm_lock(info);
		locked = true;

		// Read message if present.
		if (flshm_message_read(info, message)) {
			// Check that this message is intended for this.
			if (!strcmp(connection_name_self, message->name)) {
				// Clear the message from the memory.
				flshm_message_clear(info);

				// Show debug info for the message.
				if (debug) {
					dump_message(message);
				}

				// Read the data as AMF0 string if possible.
				if (flshm_amf0_read_string(
					msgstr,
					message->data,
					message->size
				)) {
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
					uint32_t size = flshm_amf0_write_string(
						msgstr,
						response->data,
						FLSHM_MESSAGE_MAX_SIZE
					);
					if (size) {
						// Create the response data, mimick sender.
						response->tick = tick;
						strcpy(response->name, connection_name_peer);
						strcpy(response->host, message->host);
						response->version = message->version;
						response->sandboxed = message->sandboxed;
						response->https = message->https;
						response->sandbox = message->sandbox;
						response->swfv = message->swfv;
						filepath_create(response->filepath, message->filepath);
						response->amfv = FLSHM_AMF0;
						strcpy(response->method, message->method);
						response->size = size;

						// Write the message to shared memory.
						// In theory, should poll the tick to ensure is read.
						// If not read in set timout, then erase to free.
						if (!flshm_message_write(info, response)) {
							printf("FAILED: flshm_message_write\n");
						}

						// Show debug info for the response.
						if (debug) {
							dump_message(response);
						}

						// Print the response string.
						printf("Response: %s\n", msgstr);
					}
				}
			}
		}

		flshm_unlock(info);
		locked = false;

		sleep_ms(100);
	}

	cleanup();

	return EXIT_SUCCESS;
}
