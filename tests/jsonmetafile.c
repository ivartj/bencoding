#include "../bencoding.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

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
	unsigned char *benstring;
	size_t benstringlen;
	bencode_val *metainfo;
	char *json;
	size_t jsonlen;

	file = fopen("minix.torrent", "rb");
	if(file == NULL) {
		fprintf(stderr, "Failed to open minix.torrent.\n");
		exit(EXIT_FAILURE);
	}

	benstring = freadall(file, &benstringlen);
	fclose(file);

	metainfo = bencode_parse(benstring, benstringlen);
	free(benstring);

	if(metainfo == NULL) {
		fprintf(stderr, "Failed to parse bencode.\n");
		exit(EXIT_FAILURE);
	}

	json = bencode_val_json(metainfo, &jsonlen);
	bencode_free_recursive(metainfo);

	puts(json);
	free(json);

	printf("%d\n", jsonlen);

	exit(EXIT_SUCCESS);
}
