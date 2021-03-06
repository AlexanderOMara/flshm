#include <stdio.h>
#include <string.h>

#include <flshm.h>

#include <hex2bin.h>

int main(int argc, const char ** argv) {
	if (argc < 13) {
		printf("%s "
			"tick "
			"name "
			"host "
			"version "
			"sandboxed "
			"https "
			"sandbox "
			"swfv "
			"filepath "
			"amfv "
			"method "
			"size "
			"data\n",
			argv[0]
		);
		return EXIT_FAILURE;
	}

	uint32_t tick = 0;
	const char * name = NULL;
	const char * host = NULL;
	flshm_version version = FLSHM_VERSION_1;
	bool sandboxed = false;
	bool https = false;
	uint32_t sandbox = 0;
	uint32_t swfv = 0;
	const char * filepath = NULL;
	flshm_amf amfv = FLSHM_AMF0;
	const char * method = NULL;
	size_t size = 0;
	void * data = NULL;

	if (!sscanf(argv[1], "%u", &tick) || !tick) {
		printf("ERROR: tick: %s\n", argv[1]);
		return EXIT_FAILURE;
	}
	name = argv[2];
	host = argv[3];
	version = argv[4][0] - (uint8_t)'0';
	sandboxed = argv[5][0] != '0';
	https = argv[6][0] != '0';
	if (!sscanf(argv[7], "%u", &sandbox)) {
		printf("ERROR: sandbox: %s\n", argv[7]);
		return EXIT_FAILURE;
	}
	if (!sscanf(argv[8], "%u", &swfv)) {
		printf("ERROR: swfv: %s\n", argv[8]);
		return EXIT_FAILURE;
	}
	filepath = argv[9];
	if (!sscanf(argv[10], "%u", &amfv)) {
		printf("ERROR: amfv: %s\n", argv[10]);
		return EXIT_FAILURE;
	}
	method = argv[11];

	data = hex2bin(argv[12], &size);
	if (size > FLSHM_MESSAGE_MAX_SIZE) {
		printf("FAILED: too much data\n");
		return EXIT_FAILURE;
	}

	// Create message.
	flshm_message message;
	message.tick = tick;
	flshm_amf0_encode_string_cstr(
		&message.name,
		name
	);
	flshm_amf0_encode_string_cstr(
		&message.host,
		host
	);
	message.version = version;
	message.sandboxed = sandboxed;
	message.https = https;
	message.sandbox = sandbox;
	message.swfv = swfv;
	flshm_amf0_encode_string_cstr(
		&message.filepath,
		filepath
	);
	message.amfv = amfv;
	flshm_amf0_encode_string_cstr(
		&message.method,
		method
	);
	message.size = (uint32_t)size;
	memcpy(message.data, data, size);

	flshm_keys keys;
	flshm_keys_init(&keys, false);

	flshm_info info;
	if (!flshm_open(&info, &keys)) {
		printf("FAILED: flshm_open\n");
		return EXIT_FAILURE;
	}

	int ret = EXIT_SUCCESS;

	// Lock memory, to avoid race conditions.
	flshm_lock(&info);

	if (!flshm_message_write(&info, &message)) {
		printf("FAILED: flshm_message_write\n");
		ret = EXIT_FAILURE;
	}

	// Unlock memory.
	flshm_unlock(&info);

	// Close info.
	flshm_close(&info);

	return ret;
}
