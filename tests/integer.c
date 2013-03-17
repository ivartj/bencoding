#include <stdio.h>
#include <stdlib.h>
#include "../bencoding.c"
#include <assert.h>

int main(int argc, char *argv[])
{
	char *str;
	int n;
	bencode_integer *val;
	io_buf buf;
	io_reader r;

	str = "500e";
	buf.buf = str;
	buf.len = strlen(str);
	r.data = &buf;
	r.read = io_bufread;

	val = parseinteger('i', &r, &n);
	assert(val != NULL);

	assert(val->val == 500);

	assert(n == strlen(str) + 1);

	exit(EXIT_SUCCESS);
}
