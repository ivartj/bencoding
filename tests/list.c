#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "../bencoding.c"

int main(int argc, char *argv[])
{
	bencode_list *list;
	bencode_integer *integer;
	bencode_string *string;
	char *str = "l5:cameli5ee";
	int n;
	int i;

	list = parselist(str, strlen(str), &n);
	assert(list != NULL);
	assert(list->nvals == 2);

	assert(list->vals != NULL);

	assert(list->vals[0] != NULL);
	assert(list->vals[0]->type == BENCODE_STRING);

	assert(list->vals[1] != NULL);
	assert(list->vals[1]->type == BENCODE_INTEGER);

	assert(n == strlen(str));

	exit(EXIT_SUCCESS);
}
