#include "../bencoding.c"
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
	bencode_val *val;
	unsigned len;
	const char *bencoding = "d5:cameli5e6:camelsi6ee";
	char *written;

	val = bencode_parse(bencoding, strlen(bencoding));
	assert(val != NULL);

	assert(valstrlen(val) == strlen(bencoding));

	written = bencode_val_string(val, &len);

	assert(strlen(bencoding) == len);
	assert(strcmp(written, bencoding) == 0);

	exit(EXIT_SUCCESS);
}
