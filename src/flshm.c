#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#ifdef _WIN32
	#include <windows.h>
#elif __APPLE__
	#include <unistd.h>
	#include <sys/types.h>
	#include <sys/shm.h>
	#include <semaphore.h>
	#include <mach/mach_time.h>
#else
	#include <unistd.h>
	#include <sys/types.h>
	#include <sys/shm.h>
	#include <sys/ipc.h>
	#include <sys/sem.h>
	#include <sys/time.h>
	#include <sys/sysinfo.h>
#endif

#include "flshm.h"


// Private functions to read the subset of AMF0 used in the header.
uint32_t flshm_amf0_read_string(char ** str, char * p, uint32_t max) {

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


uint32_t flshm_amf0_read_boolean(bool * flag, char * p, uint32_t max) {

	// Bounds check.
	if (max < 2) {
		return 0;
	}

	// Check the type marker.
	if (*p != '\x01') {
		return 0;
	}
	p++;

	// Read flag, anything but null is considered true.
	*flag = *p != '\x00';

	return 2;
}


uint32_t flshm_amf0_read_double(double * number, char * p, uint32_t max) {

	// Bounds check.
	if (max < 9) {
		return 0;
	}

	// Check the type marker.
	if (*p != '\x00') {
		return 0;
	}
	p++;

	// Detect the host endianness via union.
	union {
		uint16_t i;
		uint8_t c[2];
	} which;
	which.i = 1;
	bool le = which.c[0] ? true : false;

	// Convert type by union.
	union {
		double d;
		char c[8];
	} data;
	data.d = 0;

	// Read backwards if host is little-endian, else read it forward.
	if (le) {
		char * p2 = p + 8;
		for (uint8_t i = 0; i < 8; i++) {
			p2--;
			data.c[i] = *(p2);
		}
	}
	else {
		char * p2 = p;
		for (uint8_t i = 0; i < 8; i++) {
			data.c[i] = *(p2);
			p2++;
		}
	}

	// Set the double variable.
	*number = data.d;

	return 9;
}


// Private functions to write the subset of AMF0 used in the header.
uint32_t flshm_amf0_write_string(char * str, char * p, uint32_t max) {

	// Get string length and bounds check.
	size_t l = strlen(str);
	uint32_t size = (uint32_t)(l + 3);
	if (l > 0xFFFF || size > max) {
		return 0;
	}
	uint16_t sl = (uint16_t)l;

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


uint32_t flshm_amf0_write_boolean(bool flag, char * p, uint32_t max) {

	// Bounds check.
	if (max < 2) {
		return 0;
	}

	// Write the type marker.
	*p = '\x01';
	p++;

	// Write flag.
	*p = flag ? '\x01' : '\x00';

	return 2;
}


uint32_t flshm_amf0_write_double(double number, char * p, uint32_t max) {

	// Bounds check.
	if (max < 9) {
		return 0;
	}

	// Write the type marker.
	*p = '\x00';
	p++;

	// Detect the host endianness via union.
	union {
		uint16_t i;
		uint8_t c[2];
	} which;
	which.i = 1;
	bool le = which.c[0] ? true : false;

	// Convert type by union.
	union {
		double d;
		char c[8];
	} data;

	// Set double.
	data.d = number;

	// Write backwards if host is little-endian, else write it forward.
	if (le) {
		char * p2 = p + 8;
		for (uint8_t i = 0; i < 8; i++) {
			p2--;
			*(p2) = data.c[i];
		}
	}
	else {
		char * p2 = p;
		for (uint8_t i = 0; i < 8; i++) {
			*(p2) = data.c[i];
			p2++;
		}
	}

	return 9;
}


// Private function to write serialized connection to memory.
char * flshm_write_connection(char * addr, flshm_connection connection) {

	// Copy the name with the null byte into the list and advance past it.
	uint32_t name_size = (uint32_t)strlen(connection.name);
	// Only copy name if not pointing to itself.
	if (addr != connection.name) {
		strcpy(addr, connection.name);
	}
	addr += name_size + 1;

	// Add the meta data if present, '0' and '1' indexed.
	if (connection.version != FLSHM_VERSION_1) {
		*(addr++) = ':';
		*(addr++) = ':';
		*(addr++) = (char)((uint8_t)'0' + connection.version);
		*(addr++) = '\0';
		if (connection.sandbox != FLSHM_SECURITY_NONE) {
			*(addr++) = ':';
			*(addr++) = ':';
			*(addr++) = (char)((uint8_t)'1' + connection.sandbox);
			*(addr++) = '\0';
		}
	}

	return addr;
}


// Private function to hash a uid into a shm key.
uint32_t flshm_hash_uid(uint32_t uid) {

	// Hash the uid into something unique.
	uint32_t a = 9 * ((uid + ~(uid << 15)) ^ ((uid + ~(uid << 15)) >> 10));
	uint32_t b = 16389 * (a ^ (a >> 6)) ^ (16389 * (a ^ (a >> 6)) >> 16);

	// Return 1 if hash is 0.
	return b ? b : 1;
}


// Private function to check if sharded memory has been initialized.
bool flshm_shm_inited(void * shmdata) {

	// Check for 2 uint32 flag in the head.
	return *((uint32_t *)shmdata) == 1 &&
		*((uint32_t *)((char *)shmdata + 4)) == 1;
}


uint32_t flshm_tick() {

	uint32_t ret;

#ifdef _WIN32

	ret = GetTickCount();

#elif __APPLE__

	// Generated by TickCount, but TickCount is deprecated, reimplement.
	uint64_t mat = mach_absolute_time();
	uint32_t mul = 0x80D9594E;
	ret = ((((0xFFFFFFFF & mat) * mul) >> 32) + (mat >> 32) * mul) >> 23;

#else

	// Variables to cache across multiple calls.
	static bool inited = false;
	static struct timeval tvbase;
	static uint32_t base = 0;

	if (inited) {

		// Get the current time.
		struct timeval tv;
		gettimeofday(&tv, 0);

		// Calculate offset from base.
		tv.tv_sec -= tvbase.tv_sec;
		tv.tv_usec -= tvbase.tv_usec;

		// Calculate the tick from the base.
		ret = base + 1000 * tv.tv_sec + tv.tv_usec / 1000;
	}
	else {

		// Get the system uptime, first time.
		struct sysinfo si;
		sysinfo(&si);

		// Get the current time as base.
		gettimeofday(&tvbase, 0);

		// Mark initialized.
		inited = true;

		// Calculate the time base, and use it as first tick.
		base = 1000 * si.uptime + tvbase.tv_usec / 1000 % 1000;
		ret = base;
	}

#endif

	return ret;
}


flshm_keys flshm_get_keys(bool is_per_user) {

	flshm_keys keys;

#ifdef _WIN32

	(void)is_per_user;
	strcpy(keys.sem, "MacromediaMutexOmega");
	strcpy(keys.shm, "MacromediaFMOmega");

#elif __APPLE__

	// Check if user-specific keys for isPerUser, else user-shared.
	if (is_per_user) {
		// The isPerUser generated keys.
		uint32_t shm = flshm_hash_uid(getuid());
		snprintf(keys.sem, 20, "%u", shm);
		keys.shm = (key_t)shm;
	}
	else {
		// The default user-shared keys.
		strcpy(keys.sem, "MacromediaSemaphoreDig");
		keys.shm = (key_t)0x53414E44; // SAND
	}

#else

	(void)is_per_user;

	// Generate key.
	key_t key = (key_t)flshm_hash_uid(getuid());
	keys.sem = key;
	keys.shm = key;

#endif

	return keys;
}


flshm_info * flshm_open(bool is_per_user) {

	// First get the keys for the semaphore and shared memory.
	flshm_keys keys = flshm_get_keys(is_per_user);

	flshm_info * info = NULL;

#ifdef _WIN32

	// First try to open the semaphore.
	HANDLE sem = OpenMutex(MUTEX_ALL_ACCESS, FALSE, keys.sem);
	if (sem == NULL) {
		return NULL;
	}

	// Next try to open the shared memory.
	HANDLE shm = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, keys.shm);
	if (shm == NULL) {
		CloseHandle(sem);
		return NULL;
	}

	// Finally try to attach to the shared memory.
	LPVOID shmaddr = MapViewOfFile(shm, FILE_MAP_ALL_ACCESS, 0, 0, FLSHM_SIZE);
	if (shmaddr == NULL) {
		CloseHandle(shm);
		CloseHandle(sem);
		return NULL;
	}

	// Check that the memory has already been initialized.
	if (!flshm_shm_inited(shmaddr)) {
		UnmapViewOfFile(shmaddr);
		CloseHandle(shm);
		CloseHandle(sem);
		return NULL;
	}

	// Everything we need, create info data.
	info = malloc(sizeof(flshm_info));
	info->data = (void *)shmaddr;
	info->sem = sem;
	info->shm = shm;
	info->shmaddr = shmaddr;

#elif __APPLE__

	// First try to open the semaphore.
	sem_t * semdesc = sem_open(keys.sem, 0);
	if (semdesc == SEM_FAILED) {
		return NULL;
	}

	// Next try to open the shared memory.
	int shmid = shmget(keys.shm, FLSHM_SIZE, 0);
	if (shmid == -1) {
		sem_close(semdesc);
		return NULL;
	}

	// Finally try to attach to the shared memory.
	void * shmaddr = shmat(shmid, NULL, 0);
	if (shmaddr == (void *)-1) {
		close(shmid);
		sem_close(semdesc);
		return NULL;
	}

	// Check that the memory has already been initialized.
	if (!flshm_shm_inited(shmaddr)) {
		shmdt(shmaddr);
		close(shmid);
		sem_close(semdesc);
		return NULL;
	}

	// Everything we need, create info data.
	info = malloc(sizeof(flshm_info));
	info->data = (void *)shmaddr;
	info->semdesc = semdesc;
	info->shmid = shmid;
	info->shmaddr = shmaddr;

#else

	// First try to open the semaphore.
	int semid = semget(keys.sem, 1, 0);
	if (semid == -1) {
		return NULL;
	}

	// Next try to open the shared memory.
	int shmid = shmget(keys.shm, FLSHM_SIZE, 0);
	if (shmid == -1) {
		return NULL;
	}

	// Finally try to attach to the shared memory.
	void * shmaddr = shmat(shmid, NULL, 0);
	if (shmaddr == (void *)-1) {
		close(shmid);
		return NULL;
	}

	// Check that the memory has already been initialized.
	if (!flshm_shm_inited(shmaddr)) {
		shmdt(shmaddr);
		close(shmid);
		return NULL;
	}

	// Everything we need, create info data.
	info = malloc(sizeof(flshm_info));
	info->data = (void *)shmaddr;
	info->semid = semid;
	info->shmid = shmid;
	info->shmaddr = shmaddr;

#endif

	return info;
}


void flshm_close(flshm_info * info) {

	// Cleat the data pointer.
	info->data = NULL;

#ifdef _WIN32

	// Detach from shared memory, and clear the address.
	UnmapViewOfFile(info->shmaddr);
	info->shmaddr =  NULL;

	// Close the shared memory, and clear the id.
	// No need to delete, does not persist once everything detaches.
	CloseHandle(info->shm);
	info->shm = NULL;

	// Close the semaphore and null it.
	CloseHandle(info->sem);
	info->sem = NULL;

#elif __APPLE__

	// Detach from shared memory, and clear the address.
	shmdt(info->shmaddr);
	info->shmaddr = NULL;

	// Try to lock to check if another process is using the memory.
	if (flshm_lock(info)) {

		// Check how many processes are connect to the shared memory.
		struct shmid_ds ds;
		shmctl(info->shmid, IPC_STAT, &ds);

		// If no open connection, then remove shared memory.
		if (!ds.shm_nattch) {
			// Delete the shared memory.
			shmctl(info->shmid, IPC_RMID, &ds);
		}

		// Unlock and continue closing.
		flshm_unlock(info);
	}

	// Close the shared memory, and clear the id.
	close(info->shmid);
	info->shmid = 0;

	// Close the semaphore and null it.
	sem_close(info->semdesc);
	info->semdesc = NULL;

#else

	// Detach from shared memory, and clear the address.
	shmdt(info->shmaddr);
	info->shmaddr = NULL;

	// Try to lock to check if another process is using the memory.
	if (flshm_lock(info)) {

		// Check how many processes are connect to the shared memory.
		struct shmid_ds ds;
		shmctl(info->shmid, IPC_STAT, &ds);

		// If no open connection, then remove shared memory.
		if (!ds.shm_nattch) {
			// Delete the shared memory.
			shmctl(info->shmid, IPC_RMID, &ds);
		}

		// Unlock and continue closing.
		flshm_unlock(info);
	}

	// Close the shared memory, and clear the id.
	close(info->shmid);
	info->shmid = 0;

	// Clear the semaphore id.
	info->semid = 0;

#endif

	// Free the memory for the info.
	free(info);
}


bool flshm_lock(flshm_info * info) {

#ifdef _WIN32

	return WaitForSingleObject(info->sem, INFINITE) == WAIT_OBJECT_0;

#elif __APPLE__

	return !sem_wait(info->semdesc);

#else

	struct sembuf sb;
	sb.sem_num = 0;
	sb.sem_op = -1;
	sb.sem_flg = SEM_UNDO;
	return !semop(info->semid, &sb, 1);

#endif

}


bool flshm_unlock(flshm_info * info) {

#ifdef _WIN32

	return ReleaseMutex(info->sem) == TRUE;

#elif __APPLE__

	return !sem_post(info->semdesc);

#else

	struct sembuf sb;
	sb.sem_num = 0;
	sb.sem_op = 1;
	sb.sem_flg = SEM_UNDO;
	return !semop(info->semid, &sb, 1);

#endif

}


bool flshm_connection_name_valid(const char * name) {

	// First character must not be ':'.
	if (name[0] == ':') {
		return false;
	}

	// Check how many colons are expected, and keep track of those seen.
	uint8_t colons_valid = name[0] == '_' ? 0 : 1;
	uint8_t colons = 0;
	for (uint32_t i = 0; true; i++) {

		// Check if too large.
		if (i >= 0xFFFF) {
			return false;
		}

		char c = name[i];

		// If null, end of the string.
		if (c == '\0') {
			// Must have length and not end in ':'.
			if (i == 0 || name[i - 1] == ':') {
				return false;
			}
			break;
		}

		// If colon, make sure not more than expected.
		if (c == ':') {
			if (colons >= colons_valid) {
				return false;
			}
			colons++;
		}
	}

	// If saw the expected colon count, return valid.
	return colons == colons_valid;
}


flshm_connected flshm_connection_list(flshm_info * info) {

	// Initialize the connected object.
	flshm_connected connected;
	connected.count = 0;

	// Initialize a connection struct to all empty values.
	flshm_connection connection;
	connection.name = NULL;
	connection.version = FLSHM_VERSION_1;
	connection.sandbox = FLSHM_SECURITY_NONE;

	// Map out the memory, and loop over them.
	char * memory = ((char *)info->data) + FLSHM_CONNECTIONS_OFFSET;
	for (uint32_t i = 0; i < FLSHM_CONNECTIONS_SIZE; i++) {

		// Get pointer to memory and the character that appears there.
		char * p = memory + i;
		char pc = *p;

		// Unexpected null byte terminates the list.
		if (pc == '\0') {
			break;
		}
		else if (pc == ':') {

			// Check if matches "::[^\x00]\x00" in the remaining space.
			if (
				i < FLSHM_CONNECTIONS_SIZE - 3 &&
				*(p + 1) == ':' &&
				*(p + 2) != '\0' &&
				*(p + 3) == '\0'
			) {
				// Check that we are still parsing a connection.
				if (connection.name) {
					unsigned char c = *(p + 2);

					// Set version then sandbox, '0' then '1' indexed.
					if (connection.version == FLSHM_VERSION_1) {
						connection.version = c - (uint8_t)'0';
					}
					else if (connection.sandbox == FLSHM_SECURITY_NONE) {
						connection.sandbox = c - (uint8_t)'1';
					}
				}
				i += 3;
			}
			else {

				// Otherwise seek until the next null or end.
				for (; i < FLSHM_CONNECTIONS_SIZE; i++) {
					if (*(memory + i) == '\0') {
						break;
					}
				}
			}
		}
		else {

			// If currently has an open connection, store it, and reset.
			if (connection.name) {
				connected.connections[connected.count++] = connection;
				connection.name = NULL;
				connection.version = FLSHM_VERSION_1;
				connection.sandbox = FLSHM_SECURITY_NONE;
				// Stop if reached the maximum connections.
				if (connected.count >= FLSHM_CONNECTIONS_MAX_COUNT) {
					break;
				}
			}

			// Seek out the null in the remaining memory.
			for (; i < FLSHM_CONNECTIONS_SIZE; i++) {
				if (*(memory + i) == '\0') {
					// If nulled and valid, set name.
					if (flshm_connection_name_valid(p)) {
						connection.name = p;
					}
					break;
				}
			}
		}
	}

	// Store the last connection if not yet stored.
	if (connection.name) {
		connected.connections[connected.count++] = connection;
	}

	return connected;
}


bool flshm_connection_add(flshm_info * info, flshm_connection connection) {

	// Sanity check the name.
	if (!connection.name) {
		return false;
	}

	// Validate the connection name.
	if (!flshm_connection_name_valid(connection.name)) {
		return false;
	}

	// Get connection name size.
	uint32_t name_size = (uint32_t)strlen(connection.name);

	// Compute size requirements for serialized connection, includes null.
	uint32_t serialized_size = name_size + 1;
	if (connection.version != FLSHM_VERSION_1) {
		serialized_size += connection.sandbox != FLSHM_SECURITY_NONE ? 8 : 4;
	}

	// Get the current connections.
	flshm_connected connected = flshm_connection_list(info);

	// Fail if maxed out on connections.
	if (connected.count >= FLSHM_CONNECTIONS_MAX_COUNT) {
		return false;
	}

	// Loop over the connections, make sure the name is unique.
	for (uint32_t i = 0; i < connected.count; i++) {
		if (!strcmp(connection.name, connected.connections[i].name)) {
			return false;
		}
	}

	// Seek out end of connection list, if entries are present.
	char * memory = ((char *)info->data) + FLSHM_CONNECTIONS_OFFSET;
	uint32_t offset = 0;
	if (*memory != '\0') {
		// Seek for double null, make default offset invalid.
		offset = 0xFFFFFFFF;
		for (uint32_t i = 0; i < FLSHM_CONNECTIONS_SIZE - 1; i++) {
			char * p = memory + i;
			if (*p == '\0' && *(p + 1) == '\0') {
				offset = i + 1;
				break;
			}
		}
	}

	// Fail if the serialized data would not fit in the remaining memory.
	if (
		offset >= FLSHM_CONNECTIONS_SIZE ||
		offset + serialized_size + 1 >= FLSHM_CONNECTIONS_SIZE
	) {
		return false;
	}

	// Write the connection to the list.
	char * addr = flshm_write_connection(memory + offset, connection);

	// Add list terminating null.
	*(addr + 1) = '\0';

	return true;
}


bool flshm_connection_remove(flshm_info * info, flshm_connection connection) {

	// Get the current connections.
	flshm_connected connected = flshm_connection_list(info);

	// Get the offset of connection list.
	char * addr = ((char *)info->data) + FLSHM_CONNECTIONS_OFFSET;

	// Loop over them all, rewrite everything to ensure clean reflow.
	// No overwrite risk, copies will always be written at or before self.
	bool found = false;
	for (uint32_t i = 0; i < connected.count; i++) {
		flshm_connection c = connected.connections[i];

		// Check if this is the one to remove.
		if (
			!found &&
			c.version == connection.version &&
			c.sandbox == connection.sandbox &&
			!strcmp(c.name, connection.name)
		) {
			found = true;
			continue;
		}

		// Rewrite this connection wherever earlier it may fall.
		addr = flshm_write_connection(addr, c);
	}

	// Add list terminating null.
	*(addr) = '\0';

	return found;
}


uint32_t flshm_message_tick(flshm_info * info) {

	// Read the tick from the memory.
	return *((uint32_t *)((char *)info->data + FLSHM_MESSAGE_TICK_OFFSET));
}


void flshm_message_free(flshm_message * message) {

	// Free any and all memory associated with the message structure.
	if (message->name) {
		free(message->name);
	}
	if (message->host) {
		free(message->host);
	}
	if (message->filepath) {
		free(message->filepath);
	}
	if (message->method) {
		free(message->method);
	}
	if (message->data) {
		free(message->data);
	}

	// Then free the message structure itself.
	free(message);
}


flshm_message * flshm_message_read(flshm_info * info) {

	// All the properties to be set.
	uint32_t tick;
	uint32_t amfl;
	char * name = NULL;
	char * host = NULL;
	flshm_version version = FLSHM_VERSION_1;
	bool sandboxed = false;
	bool https = false;
	flshm_security sandbox = 0;
	uint32_t swfv = 0;
	char * filepath = NULL;
	flshm_amf amfv = FLSHM_AMF0;
	char * method = NULL;
	uint32_t size = 0;
	void * data = NULL;

	double d2i;

	// Default to returning null.
	flshm_message * message = NULL;

	// Pointer to shared memory.
	char * shmdata = (char *)info->data;

	// Read the tick count and check if set (only valid if non-zero).
	tick = *((uint32_t *)(shmdata + FLSHM_MESSAGE_TICK_OFFSET));
	if (!tick) {
		return NULL;
	}

	// Read the message size if present and sanity check it.
	amfl = *((uint32_t *)(shmdata + FLSHM_MESSAGE_SIZE_OFFSET));
	if (!amfl || amfl > FLSHM_MESSAGE_MAX_SIZE) {
		return NULL;
	}

	// Keep track of position and bounds.
	uint32_t i = FLSHM_MESSAGE_BODY_OFFSET;
	uint32_t max = FLSHM_MESSAGE_BODY_OFFSET + amfl;
	uint32_t read;

	// Start a block that can be broken from, assume failure on break.
	do {
		// Read the connection name, or fail.
		read = flshm_amf0_read_string(
			&name,
			shmdata + i,
			max - i
		);
		if (!read) {
			break;
		}
		i += read;

		// Read the host name, or fail.
		read = flshm_amf0_read_string(
			&host,
			shmdata + i,
			max - i
		);
		if (!read) {
			break;
		}
		i += read;

		// Read the optional data, if present, based on first boolean.

		// Read version 2 data if present.
		read = flshm_amf0_read_boolean(
			&sandboxed,
			shmdata + i,
			max - i
		);
		if (read) {
			// Read sandboxed successfully.
			i += read;

			// We have version 2 at least, read data.
			version = FLSHM_VERSION_2;

			// Read HTTPS or fail.
			read = flshm_amf0_read_boolean(
				&https,
				shmdata + i,
				max - i
			);
			if (!read) {
				break;
			}
			i += read;

			// Read version 3 data if present, based on first double.
			read = flshm_amf0_read_double(
				&d2i,
				shmdata + i,
				max - i
			);
			if (read) {
				// Read sandbox successfully.
				sandbox = (int32_t)d2i;
				i += read;

				// We have version 3 at least, read data.
				version = FLSHM_VERSION_3;

				// Read version or fail.
				read = flshm_amf0_read_double(
					&d2i,
					shmdata + i,
					max - i
				);
				if (!read) {
					break;
				}
				swfv = (uint32_t)d2i;
				i += read;

				// If sandbox local-with-file, includes sender filepath.
				if (sandbox == FLSHM_SECURITY_LOCAL_WITH_FILE) {
					read = flshm_amf0_read_string(
						&filepath,
						shmdata + i,
						max - i
					);
					if (!read) {
						break;
					}
					i += read;
				}

				// Read AMF version if present, else ignore.
				read = flshm_amf0_read_double(
					&d2i,
					shmdata + i,
					max - i
				);
				if (read) {
					amfv = (uint32_t)d2i;
					i += read;

					// Version must be 4.
					version = FLSHM_VERSION_4;
				}
			}
		}

		// Read the method name or fail.
		read = flshm_amf0_read_string(
			&method,
			shmdata + i,
			max - i
		);
		if (!read) {
			break;
		}
		i += read;

		// Calculate the size of the message, and copy to memory.
		size = max - i;
		if (size) {
			data = malloc(size);
			memcpy(data, shmdata + i, size);
		}

		// Everything needed, allocate and return struct.
		message = malloc(sizeof(flshm_message));

		message->tick = tick;
		message->amfl = amfl;
		message->name = name;
		message->host = host;
		message->version = version;
		message->sandboxed = sandboxed;
		message->https = https;
		message->sandbox = sandbox;
		message->swfv = swfv;
		message->filepath = filepath;
		message->amfv = amfv;
		message->method = method;
		message->size = size;
		message->data = data;
	} while (false);

	// If message not initialized, cleanup any memory allocated.
	if (!message) {
		if (filepath) {
			free(filepath);
		}
		if (host) {
			free(host);
		}
		if (name) {
			free(name);
		}
	}

	return message;
}


bool flshm_message_write(flshm_info * info, flshm_message * message) {

	// Validate tick is non-0.
	if (!message->tick) {
		return false;
	}
	// Validate connection is set and valid.
	if (!message->name || !flshm_connection_name_valid(message->name)) {
		return false;
	}
	// Validate host is set and valid.
	if (!message->host || strlen(message->host) > 0xFFFF) {
		return false;
	}
	// If local-with-file sandbox, ensure filepath is set and valid.
	if (
		message->version >= FLSHM_VERSION_3 &&
		message->sandbox == FLSHM_SECURITY_LOCAL_WITH_FILE &&
		(!message->filepath || strlen(message->filepath) > 0xFFFF)
	) {
		return false;
	}
	// Validate method is set and valid.
	if (!message->method || strlen(message->method) > 0xFFFF) {
		return false;
	}

	// Allocate memory to encode message into.
	char * buffer = malloc(FLSHM_MESSAGE_MAX_SIZE);

	// Start a block that can be broken from, assume failure on break.
	bool success = false;
	do {
		// Keep track of offset and bounds.
		uint32_t i = 0;
		uint32_t max = FLSHM_MESSAGE_MAX_SIZE;
		uint32_t wrote;

		// Write the connection name or fail.
		wrote = flshm_amf0_write_string(
			message->name,
			buffer + i,
			max - i
		);
		if (!wrote) {
			break;
		}
		i += wrote;

		// Write the connection host or fail.
		wrote = flshm_amf0_write_string(
			message->host,
			buffer + i,
			max - i
		);
		if (!wrote) {
			break;
		}
		i += wrote;

		// Add version 2 data if specified.
		if (message->version >= FLSHM_VERSION_2) {

			// Write sandboxed or fail.
			wrote = flshm_amf0_write_boolean(
				message->sandboxed,
				buffer + i,
				max - i
			);
			if (!wrote) {
				break;
			}
			i += wrote;

			// Write HTTPS or fail.
			wrote = flshm_amf0_write_boolean(
				message->https,
				buffer + i,
				max - i
			);
			if (!wrote) {
				break;
			}
			i += wrote;

			// Add version 3 data if specified.
			if (message->version >= FLSHM_VERSION_3) {

				// Write sandbox or fail.
				wrote = flshm_amf0_write_double(
					(double)message->sandbox,
					buffer + i,
					max - i
				);
				if (!wrote) {
					break;
				}
				i += wrote;

				// Write version or fail.
				wrote = flshm_amf0_write_double(
					(double)message->swfv,
					buffer + i,
					max - i
				);
				if (!wrote) {
					break;
				}
				i += wrote;

				// Write filepath if local-with-file or fail.
				if (
					message->sandboxed &&
					message->sandbox == FLSHM_SECURITY_LOCAL_WITH_FILE
				) {
					wrote = flshm_amf0_write_string(
						message->filepath,
						buffer + i,
						max - i
					);
					if (!wrote) {
						break;
					}
					i += wrote;
				}

				// Add version 4 data if specified.
				if (message->version >= FLSHM_VERSION_4) {
					// Write the AMF version or fail.
					wrote = flshm_amf0_write_double(
						(double)message->amfv,
						buffer + i,
						max - i
					);
					if (!wrote) {
						break;
					}
					i += wrote;
				}
			}
		}

		// Write method or fail.
		wrote = flshm_amf0_write_string(
			message->method,
			buffer + i,
			max - i
		);
		if (!wrote) {
			break;
		}
		i += wrote;

		// Check that message data will fit.
		if (message->size > max - i) {
			break;
		}

		// Write message data to the buffer.
		memcpy(buffer + i, message->data, message->size);
		i += message->size;

		// Set the total AMF size on the struct.
		message->amfl = i;

		// Pointer to shared memory.
		char * shmdata = (char *)info->data;

		// Copy buffer to the shared memory.
		memcpy(shmdata + FLSHM_MESSAGE_BODY_OFFSET, buffer, i);

		// Set the size of the message.
		*((uint32_t *)(shmdata + FLSHM_MESSAGE_SIZE_OFFSET)) = i;

		// Set the message tick.
		*((uint32_t *)(shmdata + FLSHM_MESSAGE_TICK_OFFSET)) =
			message->tick;

		// If finished the block, then successful.
		success = true;
	} while (false);

	// Free the memory and return successful or not.
	free(buffer);
	return success;
}

void flshm_message_clear(flshm_info * info) {

	// Pointer to shared memory.
	char * shmdata = (char *)info->data;

	// Write 0 to both the tick and size.
	*((uint32_t *)(shmdata + FLSHM_MESSAGE_SIZE_OFFSET)) = 0;
	*((uint32_t *)(shmdata + FLSHM_MESSAGE_TICK_OFFSET)) = 0;
}
