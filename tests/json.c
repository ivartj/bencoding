#include "../bencoding.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

int main(int argc, char *argv[])
{
	char *json;
	size_t jsonlen;
	char *bcode = "d5:cameli5e6:camelsi6ee";
	bencode_val *bval;

	bval = bencode_parse(bcode, strlen(bcode));
	if(bval == NULL) {
		fprintf(stderr, "Failed to parse bencode.\n");
		exit(EXIT_FAILURE);
	}

	json = bencode_val_json(bval, &jsonlen);

	exit(EXIT_SUCCESS);
}
