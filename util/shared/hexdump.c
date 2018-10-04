#include <stdio.h>
#include <stdlib.h>

void hexdump(void * addr, unsigned int size, unsigned int col, int skipnull) {
	unsigned char * ascii = malloc(col + 1);
	unsigned char * hex = malloc(col * 3);
	unsigned char * offset = malloc(5);
	unsigned char * bytes = (unsigned char *)addr;
	char * format = "%s  %s  %s\n";
	unsigned int i = 0;
	int nonnull = 0;
	int skippingnull = 0;
	for (; i < size; ++i) {
		unsigned int ci = i % col;
		unsigned int hci = ci ? (ci * 3 - 1) : 0;
		if (!ci) {
			if (i) {
				if (skipnull && !nonnull) {
					if (!skippingnull) {
						printf("....\n");
					}
					skippingnull = 1;
				}
				else {
					printf(format, offset, hex, ascii);
					skippingnull = 0;
				}
				nonnull = 0;
			}
			sprintf((char *)offset, "%04X", i);
		}
		// Add hex to the hex column.
		sprintf((char *)(hex + hci), hci ? " %02X" : "%02X", bytes[i]);
		// Add character to the ASCII column.
		unsigned char c = bytes[i];
		ascii[ci] = c > 31 && c < 127 ? c : '.';
		ascii[ci + 1] = '\0';
		// Mark the row as non-null, if the byte is non-null.
		nonnull = c ? 1 : nonnull;
	}
	// Fill the hex column with spaces if not already aligned.
	for (; i % col; ++i) {
		sprintf((char *)(hex + ((i % col) * 3 - 1)), "   ");
	}
	// Output the last line.
	printf(format, offset, hex, ascii);
	// Free memory.
	free(ascii);
	free(hex);
	free(offset);
}
