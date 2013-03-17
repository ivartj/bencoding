#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "../bencoding.c"

int main(int argc, char *argv[])
{
	char *str = ":camel";
	int n;
	bencode_string *val;
	io_buf buf;
	io_reader r;

	memset(&buf, 0, sizeof(buf));
	memset(&r, 0, sizeof(r));
	buf.buf = str;
	buf.len = strlen(str);
	r.data = &buf;
	r.read = io_bufread;

	val = parsestring('5', &r, &n);

	assert(val != NULL);

	assert(strcmp(val->val, "camel") == 0);

	assert(n == strlen(str) + 1);

	exit(EXIT_SUCCESS);
}
