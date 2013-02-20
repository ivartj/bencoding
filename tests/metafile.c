#include "../bencoding.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

unsigned char *freadall(FILE *in, size_t *rlen)
{
	unsigned char *out;
	size_t inc, len, cap;

	len = 0;
	cap = 256;
	out = calloc(1, cap);

	while((inc = fread(out + len, 1, cap - len, in)) > 0) {
		len += inc;
		if(len == cap) {
			cap *= 2;
			out = realloc(out, cap);
		}
	}

	if(rlen != NULL)
		*rlen = len;

	return out;
}

int main(int argc, char *argv[])
{
	FILE *file;
	unsigned char *string;
	size_t stringlen;
	bencode_val *benval;
	unsigned char *benvalstring;
	size_t benvalstringlen;

	file = fopen("minix.torrent", "r");
	if(file == NULL) {
		fprintf(stderr, "Failed to open minix.torrent.\n");
		exit(EXIT_FAILURE);
	}

	string = freadall(file, &stringlen);

	benval = bencode_parse(string, stringlen);
	if(benval == NULL) {
		fprintf(stderr, "Failed to parse minix.torrent.\n");
		exit(EXIT_FAILURE);
	}

	benvalstring = bencode_val_string(benval, &benvalstringlen);

	printf("benvalstringlen\t= %zd.\n", benvalstringlen);
	printf("stringlen\t= %zd.\n", stringlen);

	assert(memcmp(benvalstring, string, benvalstringlen) == 0);

	exit(EXIT_SUCCESS);
}
