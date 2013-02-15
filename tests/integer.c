#include <stdio.h>
#include <stdlib.h>
#include "../bencoding.c"
#include <assert.h>

int main(int argc, char *argv[])
{
	char *str;
	int n;
	bencode_integer *val;

	str = "i500e";

	val = parseinteger(str, strlen(str), &n);
	assert(val != NULL);

	assert(val->val == 500);

	assert(n == strlen(str));

	exit(EXIT_SUCCESS);
}
