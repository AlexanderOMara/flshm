#ifndef _FLSHM_H
#define _FLSHM_H

#include <stdbool.h>
#include <stdint.h>

#ifdef _WIN32
	#include <windows.h>
#elif __APPLE__
	#include <sys/types.h>
	#include <sys/shm.h>
	#include <semaphore.h>
#else
	#include <sys/types.h>
	#include <sys/shm.h>
#endif


/**
 * The size of the shared memory.
 */
#define FLSHM_SIZE 64528


/**
 * The offset of the tick for which a message is sent.
 */
#define FLSHM_MESSAGE_TICK_OFFSET 8


/**
 * The offset at which a message size is written.
 */
#define FLSHM_MESSAGE_SIZE_OFFSET 12


/**
 * The offset at which the message body is written.
 */
#define FLSHM_MESSAGE_BODY_OFFSET 16


/**
 * The maximum size an encoded message can be, includes all but the header.
 */
#define FLSHM_MESSAGE_MAX_SIZE 40960


/**
 * The offset of the list of connection names.
 */
#define FLSHM_CONNECTIONS_OFFSET 40976


/**
 * The size of the list of connection names.
 */
#define FLSHM_CONNECTIONS_SIZE 23552


/**
 * The maximum number of connection that are allowed.
 */
#define FLSHM_CONNECTIONS_MAX_COUNT 8


/**
 * Maximum length of an AMF0 string.
 */
#define FLSHM_AMF0_STRING_MAX_LENGTH 0xFFFF


/**
 * Maximum size of an AMF0 string.
 */
#define FLSHM_AMF0_STRING_MAX_SIZE (FLSHM_AMF0_STRING_MAX_LENGTH + 1)


/**
 * Maximum length of a keys string if used.
 */
#define FLSHM_KEYS_STRING_MAX_LENGTH 23


/**
 * Maximum size of a keys string if used.
 */
#define FLSHM_KEYS_STRING_MAX_SIZE (FLSHM_KEYS_STRING_MAX_LENGTH + 1)




/**
 * The ASVM version used by a connection.
 * 1 = FP6
 * 2 = FP7
 * 3 = FP8+ or AS2
 * 4 = FP9+ and AS3
 */
typedef enum flshm_version {
	FLSHM_VERSION_1 = 1, // Default.
	FLSHM_VERSION_2 = 2, // '2'
	FLSHM_VERSION_3 = 3, // '3'
	FLSHM_VERSION_4 = 4  // '4'
} flshm_version;


/**
 * The security sandbox used by a message.
 * These are encoded in a double for AMF.
 * Connection strings are 1-indexed in numeric characters.
 * The APPLICATION type is not used in the connection list.
 * If the gap number exists, it is unknown.
 */
typedef enum flshm_security {
	FLSHM_SECURITY_NONE               = -1, // Default.
	FLSHM_SECURITY_REMOTE             = 0,  // '1'
	FLSHM_SECURITY_LOCAL_WITH_FILE    = 1,  // '2'
	FLSHM_SECURITY_LOCAL_WITH_NETWORK = 2,  // '3'
	FLSHM_SECURITY_LOCAL_TRUSTED      = 3,  // '4'
	FLSHM_SECURITY_APPLICATION        = 5   // '6'
} flshm_security;


/**
 * AMF version number constants.
 */
typedef enum flshm_amf {
	FLSHM_AMF0 = 0,
	FLSHM_AMF3 = 3
} flshm_amf;




/**
 * The keys used to open the semaphore and shared memory.
 * Keys are platform specific.
 */
typedef struct flshm_keys {
	#ifdef _WIN32
		char sem[FLSHM_KEYS_STRING_MAX_SIZE];
		char shm[FLSHM_KEYS_STRING_MAX_SIZE];
	#elif __APPLE__
		char sem[FLSHM_KEYS_STRING_MAX_SIZE];
		key_t shm;
	#else
		key_t sem;
		key_t shm;
	#endif
} flshm_keys;


/**
 * The info for the semaphore and shared memory.
 * Everything but the data member is platform specific.
 */
typedef struct flshm_info {
	/**
	 * The address of the shared memory.
	 */
	void * data;

	#ifdef _WIN32
		HANDLE sem;
		HANDLE shm;
		LPVOID shmaddr;
	#elif __APPLE__
		sem_t * semdesc;
		int shmid;
		void * shmaddr;
	#else
		int semid;
		int shmid;
		void * shmaddr;
	#endif
} flshm_info;


/**
 * The connection name, ASVM, and sandbox.
 */
typedef struct flshm_connection {
	/**
	 * Connection name.
	 */
	const char * name;

	/**
	 * Version (FP7+).
	 */
	flshm_version version;

	/**
	 * Sandbox (FP9+).
	 */
	flshm_security sandbox;
} flshm_connection;


/**
 * The list of connections as a fixed-size array, with the registered count.
 */
typedef struct flshm_connected {
	/**
	 * The array of connection.
	 */
	flshm_connection connections[FLSHM_CONNECTIONS_MAX_COUNT];

	/**
	 * The number of connections listed in the array.
	 */
	uint32_t count;
} flshm_connected;


/**
 * Message structure containing all the data for an active message.
 */
typedef struct flshm_message {
	/**
	 * The tick timestamp for the message.
	 */
	uint32_t tick;

	/**
	 * The length of all of the AMF data.
	 */
	uint32_t amfl;

	/**
	 * The sending connection name.
	 */
	char name[FLSHM_AMF0_STRING_MAX_SIZE];

	/**
	 * The sending conneciton host.
	 */
	char host[FLSHM_AMF0_STRING_MAX_SIZE];

	/**
	 * What version the message format is.
	 * Defines what properties are set, as properties vary by version.
	 */
	flshm_version version;

	/**
	 * A flag for if sandboxed (SWF7 or higher, not SWf6).
	 * FLSHM_VERSION_2+
	 * FP7+
	 */
	bool sandboxed;

	/**
	 * A flag for if sending origin is using HTTPS.
	 * FLSHM_VERSION_2+
	 * FP7+
	 */
	bool https;

	/**
	 * The sender security sandbox.
	 * FLSHM_VERSION_3+
	 * FP8+
	 */
	flshm_security sandbox;

	/**
	 * The sender SWF version.
	 * FLSHM_VERSION_3+
	 * FP8+
	 */
	uint32_t swfv;

	/**
	 * The filepath of the sender for local-with-file sandbox.
	 * FLSHM_VERSION_3+
	 * FP8+ and sandbox == FLSHM_SECURITY_LOCAL_WITH_FILE
	 */
	char filepath[FLSHM_AMF0_STRING_MAX_SIZE];

	/**
	 * The AMF version the message data is encoded with.
	 * FLSHM_VERSION_4+
	 * FP9+ and AS3
	 * FLSHM_AMF0 = AMF0 (Arguments are encoded in reverse order.)
	 * FLSHM_AMF3 = AMF3 (Arguments are encoded in order.)
	 */
	flshm_amf amfv;

	/**
	 * The method name to be called in by the reciever.
	 */
	char method[FLSHM_AMF0_STRING_MAX_SIZE];

	/**
	 * The size of the message arguments data.
	 */
	uint32_t size;

	/**
	 * The message data for the arguments, encoded in AMF format in amfv.
	 */
	char data[FLSHM_MESSAGE_MAX_SIZE];
} flshm_message;




uint32_t flshm_amf0_read_string(char ** str, char * p, uint32_t max);

uint32_t flshm_amf0_read_boolean(bool * flag, char * p, uint32_t max);

uint32_t flshm_amf0_read_double(double * number, char * p, uint32_t max);

uint32_t flshm_amf0_write_string(char * str, char * p, uint32_t max);

uint32_t flshm_amf0_write_boolean(bool flag, char * p, uint32_t max);

uint32_t flshm_amf0_write_double(double number, char * p, uint32_t max);

char * flshm_write_connection(char * addr, flshm_connection connection);

uint32_t flshm_hash_uid(uint32_t uid);

bool flshm_shm_inited(void * shmdata);

/**
 * Generate a message tick.
 * Based on current time, but can return 0 in theory.
 */
uint32_t flshm_tick();


/**
 * Create the keys to open a connection with.
 * is_per_user has same functionality of the ASVM isPerUser.
 * Should be set to match the ASVM targeted.
 */
flshm_keys * flshm_keys_create(bool is_per_user);


/**
 * Destroy keys.
 * @param keys [description]
 */
void flshm_keys_destroy(flshm_keys * keys);


/**
 * Open the semaphores and shared memory.
 */
flshm_info * flshm_open(flshm_keys * keys);


/**
 * Close the semaphores and shared memory, freeing memory.
 */
void flshm_close(flshm_info * info);


/**
 * Lock the semaphore for the shared memory.
 * Useful for obtaining exclusive access, to avoid any race conditions.
 */
bool flshm_lock(flshm_info * info);


/**
 * Unlock the semaphore for the shared memory.
 */
bool flshm_unlock(flshm_info * info);


/**
 * Check if connection name is valid.
 */
bool flshm_connection_name_valid(const char * name);


/**
 * List all registered connecitons.
 * Listed connection names point directly to the string in the shared memory.
 * These strings can change anytime by another instance once unlocked.
 */
flshm_connected flshm_connection_list(flshm_info * info);


/**
 * Add a connection to the list of registered connections.
 */
bool flshm_connection_add(flshm_info * info, flshm_connection connection);


/**
 * Remove a connection from the list of registerd connections.
 */
bool flshm_connection_remove(flshm_info * info, flshm_connection connection);


/**
 * Read the current message tick.
 * Returns 0 if tick is not set.
 */
uint32_t flshm_message_tick(flshm_info * info);


/**
 * Create a message object.
 */
flshm_message * flshm_message_create();


/**
 * Free the memory returned from flshm_message_read.
 */
void flshm_message_destroy(flshm_message * message);


/**
 * Read a message from shared memory.
 */
flshm_message * flshm_message_read(flshm_info * info);


/**
 * Write a message to shared memory.
 */
bool flshm_message_write(flshm_info * info, flshm_message * message);


/**
 * Clear the message by erasing tick and size.
 */
void flshm_message_clear(flshm_info * info);

#endif
