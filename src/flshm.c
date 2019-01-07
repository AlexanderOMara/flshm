#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#ifdef _WIN32
	#include <windows.h>
#else
	#include <unistd.h>
	#include <sys/types.h>
	#include <sys/shm.h>

	#ifdef __APPLE__
		#include <semaphore.h>
		#include <mach/mach_time.h>
	#else
		#include <sys/ipc.h>
		#include <sys/sem.h>
		#include <sys/time.h>
		#include <sys/sysinfo.h>
	#endif
#endif

#include "flshm.h"


uint32_t flshm_amf0_read_string(flshm_amf0_string * str, char * p, uint32_t max) {
	// Bounds check the header.
	if (max < 3) {
		return false;
	}

	// Check the type marker.
	if (*p != '\x02') {
		return false;
	}
	p++;

	// Read the string size, big endian.
	uint16_t size =
		((*((uint8_t *)p + 1)     ) & 0xFF  ) |
		((*((uint8_t *)p    ) << 8) & 0xFF00);
	p += 2;

	// Compute total data size.
	uint32_t encode_size = size + 3;

	// Bounds check the length.
	if (encode_size > max) {
		return false;
	}

	// Set the string size.
	str->size = size;

	// Copy string data, not null terminated.
	memcpy(str->data, p, size);

	// Return the amount of data read.
	return encode_size;
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

	// Read big endian double into native via union.
	union {
		double d;
		uint64_t i;
	} u;
	u.i = 0;
	u.i |= (((uint64_t)p[0]) & 0xFF) << 56;
	u.i |= (((uint64_t)p[1]) & 0xFF) << 48;
	u.i |= (((uint64_t)p[2]) & 0xFF) << 40;
	u.i |= (((uint64_t)p[3]) & 0xFF) << 32;
	u.i |= (((uint64_t)p[4]) & 0xFF) << 24;
	u.i |= (((uint64_t)p[5]) & 0xFF) << 16;
	u.i |= (((uint64_t)p[6]) & 0xFF) << 8;
	u.i |= (((uint64_t)p[7]) & 0xFF);

	// Set the double variable.
	*number = u.d;

	return 9;
}


uint32_t flshm_amf0_write_string(const flshm_amf0_string * str, char * p, uint32_t max) {
	// Get encode length and bounds check.
	uint16_t size = str->size;
	uint32_t encode_size = size + 3;
	if (encode_size > max) {
		return 0;
	}

	// Write the string marker.
	*p = '\x02';
	p++;

	// Write the string size, big endian.
	*((uint8_t *)p    ) = (size >> 8) & 0xFF;
	*((uint8_t *)p + 1) = (size     ) & 0xFF;
	p += 2;

	// Copy the string without null byte.
	memcpy(p, str->data, size);

	return encode_size;
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

	// Write big endian double from native via union.
	union {
		double d;
		uint64_t i;
	} u;
	u.d = number;
	p[0] = (char)(u.i >> 56);
	p[1] = (char)(u.i >> 48);
	p[2] = (char)(u.i >> 40);
	p[3] = (char)(u.i >> 32);
	p[4] = (char)(u.i >> 24);
	p[5] = (char)(u.i >> 16);
	p[6] = (char)(u.i >> 8);
	p[7] = (char)(u.i);

	return 9;
}


bool flshm_amf0_decode_string_cstr(const flshm_amf0_string * str, char * to, bool skip_null) {
	bool r = true;
	uint16_t size = str->size;
	uint16_t i = 0;
	for (; i < size; i++) {
		char c = str->data[i];
		if (c == '\x00') {
			if (skip_null) {
				r = false;
				continue;
			}
			break;
		}
		to[i] = c;
	}
	to[i] = '\x00';
	if (i != size) {
		r = false;
	}
	return r;
}


bool flshm_amf0_encode_string_cstr(flshm_amf0_string * str, const char * from) {
	size_t size = strlen(from);
	if (size > FLSHM_AMF0_STRING_DATA_MAX_SIZE) {
		return false;
	}
	memcpy(str->data, from, size);
	str->size = (uint16_t)size;
	return true;
}


char * flshm_write_connection(char * addr, flshm_connection * connection) {
	// Copy the name with the null byte into the list and advance past it.
	uint32_t name_size = (uint32_t)strlen(connection->name);

	strcpy(addr, connection->name);
	addr += name_size + 1;

	// Add the meta data if present, '0' and '1' indexed.
	if (connection->version != FLSHM_VERSION_1) {
		*(addr++) = ':';
		*(addr++) = ':';
		*(addr++) = (char)((uint8_t)'0' + connection->version);
		*(addr++) = '\x00';
		if (connection->sandbox != FLSHM_SECURITY_NONE) {
			*(addr++) = ':';
			*(addr++) = ':';
			*(addr++) = (char)((uint8_t)'1' + connection->sandbox);
			*(addr++) = '\x00';
		}
	}

	return addr;
}


uint32_t flshm_hash_uid(uint32_t uid) {
	// Hash the uid into something unique.
	uint32_t a = 9 * ((uid + ~(uid << 15)) ^ ((uid + ~(uid << 15)) >> 10));
	uint32_t b = 16389 * (a ^ (a >> 6)) ^ (16389 * (a ^ (a >> 6)) >> 16);

	// Return 1 if hash is 0.
	return b ? b : 1;
}


bool flshm_shm_inited(void * shmdata) {
	// Check for 2 uint32 flag in the head.
	return (
		*((uint32_t *)shmdata) == 1 &&
		*((uint32_t *)((char *)shmdata + 4)) == 1
	);
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


void flshm_keys_init(flshm_keys * keys, bool is_per_user) {
	#ifdef _WIN32
		// Not used.
		(void)is_per_user;

		// Copy keys.
		strcpy(keys->sem, "MacromediaMutexOmega");
		strcpy(keys->shm, "MacromediaFMOmega");
	#elif __APPLE__
		// Check if user-specific keys for isPerUser, else user-shared.
		if (is_per_user) {
			// The isPerUser generated keys.
			uint32_t shm = flshm_hash_uid(getuid());
			snprintf(keys->sem, 20, "%u", shm);
			keys->shm = (key_t)shm;
		}
		else {
			// The default user-shared keys.
			strcpy(keys->sem, "MacromediaSemaphoreDig");
			keys->shm = (key_t)0x53414E44; // SAND
		}
	#else
		// Not used.
		(void)is_per_user;

		// Generate key.
		key_t key = (key_t)flshm_hash_uid(getuid());
		keys->sem = key;
		keys->shm = key;
	#endif
}


bool flshm_open(flshm_info * info, flshm_keys * keys) {
	bool r;

	#ifdef _WIN32
		// Open semaphore.
		info->sem = OpenMutex(MUTEX_ALL_ACCESS, FALSE, keys->sem);

		// Open the shared memory.
		info->shm = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, keys->shm);

		// Attach to shared memory.
		info->shmaddr = info->shm ?
			MapViewOfFile(info->shm, FILE_MAP_ALL_ACCESS, 0, 0, FLSHM_SIZE) :
			NULL;
		info->data = info->shmaddr;

		// Check success, cleanup if any failed.
		r = info->sem &&
			info->shm &&
			info->shmaddr &&
			flshm_shm_inited(info->data);
		if (!r) {
			if (info->shmaddr) {
				UnmapViewOfFile(info->shmaddr);
			}
			if (info->shm) {
				CloseHandle(info->shm);
			}
			if (info->sem) {
				CloseHandle(info->sem);
			}
			return false;
		}
	#else
		bool semopen;
		void * badptr = (void *)-1;

		// Open semaphore.
		#if __APPLE__
			info->sem = sem_open(keys->sem, 0);
			semopen = info->sem != SEM_FAILED;
		#else
			info->sem = semget(keys->sem, 1, 0);
			semopen = info->sem > -1;
		#endif

		// Open the shared memory.
		info->shm = shmget(keys->shm, FLSHM_SIZE, 0);
		bool shmgot = info->shm > -1;

		// Attach to shared memory.
		info->shmaddr = shmgot ? shmat(info->shm, NULL, 0) : badptr;
		info->data = (void *)info->shmaddr;
		bool shmopen = info->shmaddr != badptr;

		// Cleanup if any failed.
		r = semopen &&
			shmgot &&
			shmopen &&
			flshm_shm_inited(info->data);
		if (!r) {
			if (shmopen) {
				shmdt(info->shmaddr);
			}
			if (shmgot) {
				close(info->shm);
			}
			if (semopen) {
				#if __APPLE__
					sem_close(info->sem);
				#else
					close(info->sem);
				#endif
			}
			return false;
		}
	#endif

	return r;
}


void flshm_close(flshm_info * info) {
	#ifdef _WIN32
		// Detach from shared memory.
		UnmapViewOfFile(info->shmaddr);

		// Close the shared memory.
		CloseHandle(info->shm);

		// Close semaphore.
		CloseHandle(info->sem);
	#else
		// Detach from shared memory.
		shmdt(info->shmaddr);

		// Try to lock to check if another process is using the memory.
		if (flshm_lock(info)) {
			// Check how many processes are connect to the shared memory.
			struct shmid_ds ds;
			shmctl(info->shm, IPC_STAT, &ds);

			// If no open connection, then remove shared memory.
			if (!ds.shm_nattch) {
				shmctl(info->shm, IPC_RMID, &ds);
			}

			flshm_unlock(info);
		}

		// Close the shared memory.
		close(info->shm);

		// Close semaphore.
		#if __APPLE__
			sem_close(info->sem);
		#else
			// Nothing to close here.
		#endif
	#endif
}


bool flshm_lock(flshm_info * info) {
	#ifdef _WIN32
		return WaitForSingleObject(info->sem, INFINITE) == WAIT_OBJECT_0;
	#elif __APPLE__
		return !sem_wait(info->sem);
	#else
		struct sembuf sb;
		sb.sem_num = 0;
		sb.sem_op = -1;
		sb.sem_flg = SEM_UNDO;
		return !semop(info->sem, &sb, 1);
	#endif
}


bool flshm_unlock(flshm_info * info) {
	#ifdef _WIN32
		return ReleaseMutex(info->sem) == TRUE;
	#elif __APPLE__
		return !sem_post(info->sem);
	#else
		struct sembuf sb;
		sb.sem_num = 0;
		sb.sem_op = 1;
		sb.sem_flg = SEM_UNDO;
		return !semop(info->sem, &sb, 1);
	#endif
}


bool flshm_connection_name_valid(const char * name, size_t size) {
	// Check if too large.
	if (size > FLSHM_CONNECTION_NAME_MAX_LENGTH) {
		return false;
	}

	// First character must exist and not be ':' or null.
	if (size == 0 || name[0] == ':' || name[0] == '\x00') {
		return false;
	}

	// Check how many colons are expected, and keep track of those seen.
	uint8_t colons_valid = name[0] == '_' ? 0 : 1;
	uint8_t colons = 0;
	for (uint32_t i = 0; i < size; i++) {
		char c = name[i];

		// Should not encounter a null byte within size.
		if (c == '\x00') {
			return false;
		}

		// If colon, make sure not more than expected.
		if (c == ':') {
			colons++;
			if (colons > colons_valid) {
				return false;
			}
		}
	}

	// If saw the expected colon count, return valid.
	return colons == colons_valid;
}


bool flshm_connection_name_valid_cstr(const char * name) {
	size_t len = strlen(name);
	return flshm_connection_name_valid(name, len);
}


bool flshm_connection_name_valid_amf0(const flshm_amf0_string * name) {
	return flshm_connection_name_valid(name->data, name->size);
}


void flshm_connection_list(flshm_info * info, flshm_connected * list) {
	list->count = 0;

	char * name = NULL;
	flshm_version version = FLSHM_VERSION_1;
	flshm_security sandbox = FLSHM_SECURITY_NONE;

	// Map out the memory, and loop over them.
	char * memory = ((char *)info->data) + FLSHM_CONNECTIONS_OFFSET;
	for (uint32_t i = 0; i < FLSHM_CONNECTIONS_SIZE; i++) {
		// Get pointer to memory and the character that appears there.
		char * p = memory + i;
		char pc = *p;

		// Unexpected null byte terminates the list.
		if (pc == '\x00') {
			break;
		}
		else if (pc == ':') {
			// Check if matches "::[^\x00]\x00" in the remaining space.
			if (
				i < FLSHM_CONNECTIONS_SIZE - 3 &&
				*(p + 1) == ':' &&
				*(p + 2) != '\x00' &&
				*(p + 3) == '\x00'
			) {
				// Check that we are still parsing a connection.
				if (name) {
					unsigned char c = *(p + 2);

					// Set version then sandbox, '0' then '1' indexed.
					if (version == FLSHM_VERSION_1) {
						version = c - (uint8_t)'0';
					}
					else if (sandbox == FLSHM_SECURITY_NONE) {
						sandbox = c - (uint8_t)'1';
					}
				}
				i += 3;
			}
			else {
				// Otherwise seek until the next null or end.
				for (; i < FLSHM_CONNECTIONS_SIZE; i++) {
					if (*(memory + i) == '\x00') {
						break;
					}
				}
			}
		}
		else {
			// If currently has an open connection, store it, and reset.
			if (name) {
				flshm_connection * con = &list->connections[list->count];
				strcpy(con->name, name);
				con->version = version;
				con->sandbox = sandbox;
				list->count++;

				name = NULL;
				version = FLSHM_VERSION_1;
				sandbox = FLSHM_SECURITY_NONE;

				// Stop if reached the maximum connections.
				if (list->count >= FLSHM_CONNECTIONS_MAX_COUNT) {
					break;
				}
			}

			// Seek out the null in the remaining memory.
			for (; i < FLSHM_CONNECTIONS_SIZE; i++) {
				if (*(memory + i) == '\x00') {
					// If nulled and valid, set name.
					if (flshm_connection_name_valid_cstr(p)) {
						name = p;
					}
					break;
				}
			}
		}
	}

	// Store the last connection if not yet stored.
	if (name) {
		flshm_connection * con = &list->connections[list->count];
		strcpy(con->name, name);
		con->version = version;
		con->sandbox = sandbox;
		list->count++;
	}
}


bool flshm_connection_add(flshm_info * info, flshm_connection * connection) {
	// Validate the connection name.
	if (!flshm_connection_name_valid_cstr(connection->name)) {
		return false;
	}

	// Get connection name size.
	uint32_t name_size = (uint32_t)strlen(connection->name);

	// Compute size requirements for serialized connection, includes null.
	uint32_t serialized_size = name_size + 1;
	if (connection->version != FLSHM_VERSION_1) {
		serialized_size += connection->sandbox != FLSHM_SECURITY_NONE ? 8 : 4;
	}

	// Get the current connections.
	flshm_connected connected;
	flshm_connection_list(info, &connected);

	// Fail if maxed out on connections.
	if (connected.count >= FLSHM_CONNECTIONS_MAX_COUNT) {
		return false;
	}

	// Loop over the connections, make sure the name is unique.
	for (uint32_t i = 0; i < connected.count; i++) {
		if (!strcmp(connection->name, connected.connections[i].name)) {
			return false;
		}
	}

	// Seek out end of connection list, if entries are present.
	char * memory = ((char *)info->data) + FLSHM_CONNECTIONS_OFFSET;
	uint32_t offset = 0;
	if (*memory != '\x00') {
		// Seek for double null, make default offset invalid.
		offset = FLSHM_CONNECTIONS_SIZE;
		for (uint32_t i = 0; i < FLSHM_CONNECTIONS_SIZE - 1; i++) {
			char * p = memory + i;
			if (*p == '\x00' && *(p + 1) == '\x00') {
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
	*(addr + 1) = '\x00';

	return true;
}


bool flshm_connection_remove(flshm_info * info, flshm_connection * connection) {
	// Get the current connections.
	flshm_connected connected;
	flshm_connection_list(info, &connected);

	// Get the offset of connection list.
	char * addr = ((char *)info->data) + FLSHM_CONNECTIONS_OFFSET;

	// Loop over them all, rewrite everything to ensure clean reflow.
	bool found = false;
	for (uint32_t i = 0; i < connected.count; i++) {
		flshm_connection c = connected.connections[i];

		// Check if this is the one to remove.
		if (
			!found &&
			c.version == connection->version &&
			c.sandbox == connection->sandbox &&
			!strcmp(c.name, connection->name)
		) {
			found = true;
			continue;
		}

		// Rewrite this connection entry.
		addr = flshm_write_connection(addr, &c);
	}

	// Add list terminating null.
	*(addr) = '\x00';

	return found;
}


uint32_t flshm_message_tick(flshm_info * info) {
	// Read the tick from the memory.
	return *((uint32_t *)((char *)info->data + FLSHM_MESSAGE_TICK_OFFSET));
}


bool flshm_message_read(flshm_info * info, flshm_message * message) {
	// All the properties to be set.
	uint32_t tick;
	uint32_t amfl;
	flshm_version version = FLSHM_VERSION_1;
	bool sandboxed = false;
	bool https = false;
	flshm_security sandbox = FLSHM_SECURITY_NONE;
	uint32_t swfv = 0;
	flshm_amf amfv = FLSHM_AMF0;
	uint32_t size = 0;

	double d2i;

	// Pointer to shared memory.
	char * shmdata = (char *)info->data;

	// Read the tick count and check if set (only valid if non-zero).
	tick = *((uint32_t *)(shmdata + FLSHM_MESSAGE_TICK_OFFSET));
	if (!tick) {
		return false;
	}

	// Read the message size if present and sanity check it.
	amfl = *((uint32_t *)(shmdata + FLSHM_MESSAGE_SIZE_OFFSET));
	if (!amfl || amfl > FLSHM_MESSAGE_MAX_SIZE) {
		return false;
	}

	// Keep track of position and bounds.
	uint32_t i = FLSHM_MESSAGE_BODY_OFFSET;
	uint32_t max = FLSHM_MESSAGE_BODY_OFFSET + amfl;
	uint32_t re;

	bool success = false;

	// Start a block that can be broken from, assume failure on an early break.
	for (;;) {
		// Read the connection name, or fail.
		re = flshm_amf0_read_string(
			&message->name,
			shmdata + i,
			max - i
		);
		if (!re) {
			break;
		}
		i += re;

		// Read the host name, or fail.
		re = flshm_amf0_read_string(
			&message->host,
			shmdata + i,
			max - i
		);
		if (!re) {
			break;
		}
		i += re;

		// Read the optional data, if present, based on first boolean.

		// Read version 2 data if present.
		re = flshm_amf0_read_boolean(
			&sandboxed,
			shmdata + i,
			max - i
		);
		if (re) {
			// Read sandboxed successfully.
			i += re;

			// We have version 2 at least, read data.
			version = FLSHM_VERSION_2;

			// Read HTTPS or fail.
			re = flshm_amf0_read_boolean(
				&https,
				shmdata + i,
				max - i
			);
			if (!re) {
				break;
			}
			i += re;

			// Read version 3 data if present, based on first double.
			re = flshm_amf0_read_double(
				&d2i,
				shmdata + i,
				max - i
			);
			if (re) {
				// Read sandbox successfully.
				sandbox = (int32_t)d2i;
				i += re;

				// We have version 3 at least, read data.
				version = FLSHM_VERSION_3;

				// Read version or fail.
				re = flshm_amf0_read_double(
					&d2i,
					shmdata + i,
					max - i
				);
				if (!re) {
					break;
				}
				swfv = (uint32_t)d2i;
				i += re;

				// If sandbox local-with-file, includes sender filepath.
				if (sandbox == FLSHM_SECURITY_LOCAL_WITH_FILE) {
					re = flshm_amf0_read_string(
						&message->filepath,
						shmdata + i,
						max - i
					);
					if (!re) {
						break;
					}
					i += re;
				}
				else {
					message->size = 0;
				}

				// Read AMF version if present, else ignore.
				re = flshm_amf0_read_double(
					&d2i,
					shmdata + i,
					max - i
				);
				if (re) {
					amfv = (uint32_t)d2i;
					i += re;

					// Version must be 4.
					version = FLSHM_VERSION_4;
				}
			}
		}

		// Read the method name or fail.
		re = flshm_amf0_read_string(
			&message->method,
			shmdata + i,
			max - i
		);
		if (!re) {
			break;
		}
		i += re;

		// Copy the message data.
		size = max - i;
		if (size) {
			memcpy(message->data, shmdata + i, size);
		}

		// Finish setting the members.
		message->tick = tick;
		message->amfl = amfl;
		message->version = version;
		message->sandboxed = sandboxed;
		message->https = https;
		message->sandbox = sandbox;
		message->swfv = swfv;
		message->amfv = amfv;
		message->size = size;

		success = true;
		break;
	}

	return success;
}


bool flshm_message_write(flshm_info * info, flshm_message * message) {
	// The buffer to encode message into.
	char buffer[FLSHM_MESSAGE_MAX_SIZE];

	// Start a block that can be broken from, assume failure on break.
	bool success = false;
	for (;;) {
		// Keep track of offset and bounds.
		uint32_t i = 0;
		uint32_t max = sizeof(buffer);
		uint32_t wr;

		// Write the connection name or fail.
		wr = flshm_amf0_write_string(
			&message->name,
			buffer + i,
			max - i
		);
		if (!wr) {
			break;
		}
		i += wr;

		// Write the connection host or fail.
		wr = flshm_amf0_write_string(
			&message->host,
			buffer + i,
			max - i
		);
		if (!wr) {
			break;
		}
		i += wr;

		// Add version 2 data if specified.
		if (message->version >= FLSHM_VERSION_2) {
			// Write sandboxed or fail.
			wr = flshm_amf0_write_boolean(
				message->sandboxed,
				buffer + i,
				max - i
			);
			if (!wr) {
				break;
			}
			i += wr;

			// Write HTTPS or fail.
			wr = flshm_amf0_write_boolean(
				message->https,
				buffer + i,
				max - i
			);
			if (!wr) {
				break;
			}
			i += wr;

			// Add version 3 data if specified.
			if (message->version >= FLSHM_VERSION_3) {
				// Write sandbox or fail.
				wr = flshm_amf0_write_double(
					(double)message->sandbox,
					buffer + i,
					max - i
				);
				if (!wr) {
					break;
				}
				i += wr;

				// Write version or fail.
				wr = flshm_amf0_write_double(
					(double)message->swfv,
					buffer + i,
					max - i
				);
				if (!wr) {
					break;
				}
				i += wr;

				// Write filepath if local-with-file or fail.
				if (
					message->sandboxed &&
					message->sandbox == FLSHM_SECURITY_LOCAL_WITH_FILE
				) {
					wr = flshm_amf0_write_string(
						&message->filepath,
						buffer + i,
						max - i
					);
					if (!wr) {
						break;
					}
					i += wr;
				}

				// Add version 4 data if specified.
				if (message->version >= FLSHM_VERSION_4) {
					// Write the AMF version or fail.
					wr = flshm_amf0_write_double(
						(double)message->amfv,
						buffer + i,
						max - i
					);
					if (!wr) {
						break;
					}
					i += wr;
				}
			}
		}

		// Write method or fail.
		wr = flshm_amf0_write_string(
			&message->method,
			buffer + i,
			max - i
		);
		if (!wr) {
			break;
		}
		i += wr;

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
		*((uint32_t *)(shmdata + FLSHM_MESSAGE_TICK_OFFSET)) = message->tick;

		success = true;
		break;
	}

	return success;
}


void flshm_message_clear(flshm_info * info) {
	// Pointer to shared memory.
	char * shmdata = (char *)info->data;

	// Write 0 to both the tick and size.
	*((uint32_t *)(shmdata + FLSHM_MESSAGE_SIZE_OFFSET)) = 0;
	*((uint32_t *)(shmdata + FLSHM_MESSAGE_TICK_OFFSET)) = 0;
}
