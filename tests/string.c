#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "../bencoding.c"

int main(int argc, char *argv[])
{
	char *str = "5:camel";
	int n;
	bencode_string *val;

	val = parsestring(str, strlen(str), &n);

	assert(val != NULL);

	assert(strcmp(val->val, "camel") == 0);

	assert(n == strlen(str));

	exit(EXIT_SUCCESS);
}
