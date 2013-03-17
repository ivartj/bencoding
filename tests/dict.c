#include <stdio.h>
#include <stdlib.h>
#include "../bencoding.c"
#include <assert.h>

int main(int argc, char *argv[])
{
	bencode_dict *dict;
	bencode_string *key;
	bencode_integer *val;
	int i;
	io_buf buf;
	io_reader r;

	char *str = "5:cameli5e6:camelsi6ee";
	int n;

	memset(&buf, 0, sizeof(buf));
	memset(&r, 0, sizeof(r));
	buf.buf = str;
	buf.len = strlen(str);
	r.data = &buf;
	r.read = io_bufread;

	dict = parsedict('d', &r, &n);
	assert(dict != NULL);

	assert(dict->nvals == 2);
	assert(dict->vals != NULL);
	assert(dict->keys != NULL);

	for(i = 0; i < 2; i++) {
		assert(dict->keys[i] != NULL);
		assert(dict->keys[i]->type == BENCODE_STRING);
		assert(dict->vals[i] != NULL);
		assert(dict->vals[i]->type == BENCODE_INTEGER);
	}

	exit(EXIT_SUCCESS);
}
