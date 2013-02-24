#include "bencoding.h"
#include <stdlib.h>
#include <assert.h>

static size_t writechar(unsigned char c, char *str);
static size_t writeval(bencode_val *val, char *str, unsigned ind);
static size_t writestring(bencode_val *val, char *str, unsigned ind);
static size_t writeinteger(bencode_val *val, char *str, unsigned ind);
static size_t writelist(bencode_val *val, char *str, unsigned ind);
static size_t writedict(bencode_val *val, char *str, unsigned ind);

char *bencode_val_json(bencode_val *val, size_t *rlen)
{
	size_t len;
	char *str;

	len = 100000;
	str = malloc(len + 1);
	len = writeval(val, str, 0);
	str[len + 1] = '\0';
	str = realloc(str, len + 1);

	if(rlen != NULL)
		*rlen = len;

	return str;
}

size_t writeval(bencode_val *val, char *str, unsigned ind)
{
	switch(val->type) {
	case BENCODE_STRING:
		return writestring(val, str, ind);
	case BENCODE_INTEGER:
		return writeinteger(val, str, ind);
	case BENCODE_LIST:
		return writelist(val, str, ind);
	case BENCODE_DICT:
		return writedict(val, str, ind);
	}
}

size_t writestring(bencode_val *val, char *str, unsigned ind)
{

	bencode_string *string;
	size_t n;
	int i;

	n = 0;

	string = &(val->string);

	str[0] = '\"';
	n++;

	for(i = 0; i < string->len; i++) {
		n += writechar(string->val[i], str + n);
	}

	str[n] = '\"';
	n++;

	return n;
}

size_t writechar(unsigned char c, char *str)
{
	size_t n;

	if(c == '\\') {
		return sprintf(str, "\\\\");
	}

	if(isascii(c) && (isspace(c) || isgraph(c))) {
		str[0] = c;
		return 1;
	}

	n = sprintf(str, "\\x%.2hhX", c);

	return n;
}

size_t writeinteger(bencode_val *val, char *str, unsigned ind)
{
	bencode_integer *integer;

	integer = &(val->integer);

	return sprintf(str, "%d", integer->val);
}

size_t indent(char *str, unsigned ind)
{
	int i;

	for(i = 0; i < ind; i++)
		str[i] = '\t';

	return ind;
}

size_t writelist(bencode_val *val, char *str, unsigned ind)
{
	size_t n;
	int i;
	bencode_list *list;

	list = &(val->list);

	if(list->nvals == 0)
		return sprintf(str, "[]");

	n = 0;
	n += sprintf(str + n, "[\n");

	for(i = 0; i < list->nvals - 1; i++) {
		n += indent(str + n, ind + 1);
		n += writeval(list->vals[i], str + n, ind + 1);
		n += sprintf(str + n, ",\n");
	}

	n += indent(str + n, ind + 1);
	n += writeval(list->vals[i], str + n, ind + 1);
	n += sprintf(str + n, "\n");

	n += indent(str + n, ind);
	n += sprintf(str + n, "]");

	return n;
}

size_t writedict(bencode_val *val, char *str, unsigned ind)
{
	size_t n;
	int i;
	bencode_dict *dict;

	dict = &(val->dict);

	if(dict->nvals == 0)
		return sprintf(str, "{}");

	n = 0;
	n += sprintf(str + n, "{\n");

	for(i = 0; i < dict->nvals - 1; i++) {
		n += indent(str + n, ind + 1);
		n += writeval((bencode_val *)dict->keys[i], str + n, ind + 1);
		n += sprintf(str + n, " : ");
		n += writeval(dict->vals[i], str + n, ind + 1);
		n += sprintf(str + n, ",\n");
	}

	n += indent(str + n, ind + 1);
	n += writeval((bencode_val *)dict->keys[i], str + n, ind + 1);
	n += sprintf(str + n, " : ");
	n += writeval(dict->vals[i], str + n, ind + 1);
	n += sprintf(str + n, "\n");

	n += indent(str + n, ind);
	n += sprintf(str + n, "}");

	return n;
}
