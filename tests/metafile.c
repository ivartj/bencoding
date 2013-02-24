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
	unsigned char *iter;
	size_t minlen;
	int i, j; 
	char *json;
	size_t jsonlen;
	bencode_dict *info;
	bencode_integer *private;

	file = fopen("minix.torrent", "rb");
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

	if(benvalstring == NULL) {
		fprintf(stderr, "Failed to parse benvalstring.\n");
		exit(EXIT_FAILURE);
	}

	assert(memcmp(benvalstring, string, benvalstringlen) == 0);

	free(benvalstring);
	free(string);

	info = (bencode_dict *)bencode_dict_get((bencode_dict *)benval, "info");
	if(info == NULL || info->type != BENCODE_DICT) {
		fprintf(stderr, "No info section found in bencode!\n");
		exit(EXIT_FAILURE);
	}

	private = (bencode_integer *)bencode_dict_get(info, "private");
	if(private == NULL || private->type != BENCODE_INTEGER) {
		fprintf(stderr, "No private member of info section in bencode!\n");
		exit(EXIT_FAILURE);
	}

	printf("private = %i\n", private->val);

	bencode_free_recursive(benval);

	exit(EXIT_SUCCESS);
}
