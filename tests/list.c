#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "../bencoding.c"

int main(int argc, char *argv[])
{
	bencode_list *list;
	bencode_integer *integer;
	bencode_string *string;
	char *str = "5:cameli5ee";
	int n;
	int i;
	io_buf buf;
	io_reader r;

	memset(&buf, 0, sizeof(buf));
	memset(&r, 0, sizeof(r));

	buf.buf = str;
	buf.len = strlen(str);
	r.data = &buf;
	r.read = io_bufread;

	list = parselist('l', &r, &n);
	assert(list != NULL);
	assert(list->nvals == 2);

	assert(list->vals != NULL);

	assert(list->vals[0] != NULL);
	assert(list->vals[0]->type == BENCODE_STRING);

	assert(list->vals[1] != NULL);
	assert(list->vals[1]->type == BENCODE_INTEGER);

	assert(n == strlen(str) + 1);

	exit(EXIT_SUCCESS);
}
