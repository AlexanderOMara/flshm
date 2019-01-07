#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <signal.h>
#include <time.h>

#include <flshm.h>

#include <dump.h>
#include <sleep.h>
#include <strinv.h>

static flshm_keys keys;
static flshm_info info;
static bool opened = false;
static bool locked = false;
static flshm_connection connection;
static flshm_message message;
static flshm_message response;
static char filepath_append[] = ".chatbot.swf";

static void filepath_create(
	flshm_amf0_string * dest,
	const flshm_amf0_string * src
) {
	uint16_t size = src->size;
	size_t filepath_append_size = sizeof(filepath_append);

	// Find the new size after appending, limited to max size.
	size_t dest_size = size + filepath_append_size;
	if (dest_size > FLSHM_AMF0_STRING_DATA_MAX_SIZE) {
		dest_size = FLSHM_AMF0_STRING_DATA_MAX_SIZE;
	}

	// Add filepath append to the end, or as close to the end as possible.
	memcpy(dest->data, src->data, size);
	memcpy(
		&dest->data[dest_size - filepath_append_size],
		filepath_append,
		filepath_append_size
	);
	dest->size = (uint16_t)dest_size;
}

static uint32_t wait_next_tick(uint32_t current) {
	// Generate tick, loop if still same.
	uint32_t r;
	do {
		r = flshm_tick();
	}
	while (r == current);
	return r;
}

static void cleanup(void) {
	printf("\nCleaning up...\n");
	if (opened) {
		if (!locked) {
			flshm_lock(&info);
		}
		flshm_connection_remove(&info, &connection);
		flshm_unlock(&info);
		flshm_close(&info);
		opened = false;
	}
}

static void onshutdown(int signo) {
	cleanup();
	printf("Shutting down...\n");
	exit(signo == SIGINT ? EXIT_SUCCESS : EXIT_FAILURE);
}

static void register_shutdown() {
	int signals[] = {
		SIGABRT,
		SIGFPE,
		SIGILL,
		SIGINT,
		SIGSEGV,
		SIGTERM
	};

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

	if (!flshm_connection_name_valid_cstr(argv[1])) {
		printf("Invalid connection_name_self: %s\n", argv[1]);
		return EXIT_FAILURE;
	}
	if (!flshm_connection_name_valid_cstr(argv[2])) {
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

	flshm_keys_init(&keys, is_per_user);
	opened = flshm_open(&info, &keys);
	if (!opened) {
		printf("FAILED: flshm_open\n");
		cleanup();
		return EXIT_FAILURE;
	}

	// Register the connection name or fail.
	locked = flshm_lock(&info);
	if (!flshm_connection_add(&info, &connection)) {
		printf("FAILED: flshm_connection_add\n");
		cleanup();
		return EXIT_FAILURE;
	}
	locked = !flshm_unlock(&info);

	// char msgstr[FLSHM_AMF0_STRING_DECODE_MAX_SIZE];
	flshm_amf0_string msg_data_amf0;
	char msg_data_cstr[FLSHM_AMF0_STRING_DECODE_MAX_SIZE];
	char msg_name[FLSHM_AMF0_STRING_DECODE_MAX_SIZE];

	printf("Chatbot runnning...\n");

	// Run loop.
	bool running = true;
	while (running) {
		locked = flshm_lock(&info);

		// Start a block that can be broken from.
		for (;;) {
			// Read message if present.
			if (!flshm_message_read(&info, &message)) {
				break;
			}

			// Check that the message is intended for this.
			flshm_amf0_decode_string_cstr(&message.name, msg_name, false);
			if (strcmp(connection_name_self, msg_name)) {
				break;
			}

			// Clear the message from the memory.
			flshm_message_clear(&info);

			// Show debug info for the message.
			if (debug) {
				dump_msg(&message);
			}

			// Read the data as AMF0 string if possible.
			if (!flshm_amf0_read_string(
				&msg_data_amf0,
				message.data,
				message.size
			)) {
				break;
			}

			// Decode message to C-string.
			flshm_amf0_decode_string_cstr(
				&msg_data_amf0,
				msg_data_cstr,
				false
			);

			// Print the parsed string.
			printf("Received: %s\n", msg_data_cstr);

			// Invert the character cases.
			strinv(msg_data_cstr);

			// Encode response from C-string.
			flshm_amf0_encode_string_cstr(
				&msg_data_amf0,
				msg_data_cstr
			);

			// Create a buffer for the data, and write to it.
			uint32_t size = flshm_amf0_write_string(
				&msg_data_amf0,
				response.data,
				FLSHM_MESSAGE_MAX_SIZE
			);
			if (!size) {
				break;
			}
			// Generate tick, waiting for next one if necessary.
			uint32_t tick = wait_next_tick(message.tick);

			// Create the response data, mimick sender.
			response.tick = tick;
			flshm_amf0_encode_string_cstr(
				&response.name,
				connection_name_peer
			);
			response.host = message.host;
			response.version = message.version;
			response.sandboxed = message.sandboxed;
			response.https = message.https;
			response.sandbox = message.sandbox;
			response.swfv = message.swfv;
			filepath_create(&response.filepath, &message.filepath);
			response.amfv = FLSHM_AMF0;
			response.method = message.method;
			response.size = size;

			// Write the message to shared memory.
			// In theory, should poll the tick to ensure is read.
			// If not read in set timout, then erase to free.
			if (!flshm_message_write(&info, &response)) {
				printf("FAILED: flshm_message_write\n");
			}

			// Show debug info for the response.
			if (debug) {
				dump_msg(&response);
			}

			// Print the response string.
			printf("Response: %s\n", msg_data_cstr);
		}

		locked = !flshm_unlock(&info);

		sleep_ms(100);
	}

	cleanup();

	return EXIT_SUCCESS;
}
