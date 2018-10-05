#include <stdlib.h>

#ifdef _WIN32
	#include <windows.h>
#elif _POSIX_C_SOURCE >= 199309L
	#include <time.h>
#else
	#include <sys/select.h>
#endif

void sleep_ms(int ms) {
#ifdef _WIN32

	Sleep(ms);

#elif _POSIX_C_SOURCE >= 199309L

	struct timespec ts;
	ts.tv_sec = ms / 1000;
	ts.tv_nsec = (ms % 1000) * 1000000;
	nanosleep(&ts, NULL);

#else

	struct timeval tv;
	tv.tv_sec = ms / 1000;
	tv.tv_usec = (ms % 1000) * 1000000;
	select(0, NULL, NULL, NULL, &tv);

#endif
}
