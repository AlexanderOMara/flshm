#include <string.h>
#include <stdio.h>

#include "hex2bin.h"

char * hex2bin(const char * hex, size_t * len) {
	size_t len_hex = strlen(hex);
	if (len_hex % 2) {
		len_hex--;
	}
	size_t bin_len = len_hex / 2;
	char * bin = malloc(bin_len);
	if (!bin) {
		return NULL;
	}
	for (size_t i = 0; i < bin_len; i++) {
		unsigned int c;
		sscanf(hex + (i * 2), "%02x", &c);
		bin[i] = (char)c;
	}
	*len = bin_len;
	return bin;
}
