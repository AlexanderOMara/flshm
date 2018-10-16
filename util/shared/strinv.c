#include <stdlib.h>

#include "strinv.h"

void strinv(char * s) {
	for (size_t i = 0; s[i] != '\0'; i++) {
		char c = s[i];
		if (c >= 'a' && c <= 'z') {
			s[i] = s[i] - ' ';
		}
		else if (c >= 'A' && c <= 'Z') {
			s[i] = s[i] + ' ';
		}
	}
}
