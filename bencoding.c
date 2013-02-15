#include "bencoding.h"
#include <stdlib.h>
#include <string.h>

static bencode_string *parsestring(const char *input, unsigned len, unsigned *rlen);
static bencode_integer *parseinteger(const char *input, unsigned len, unsigned *rlen);
static bencode_list *parselist(const char *input, unsigned len, unsigned *rlen);
static bencode_dict *parsedict(const char *input, unsigned len, unsigned *rlen);
static bencode_val *parseval(const char *input, unsigned len, unsigned *rlen);

bencode_val *bencode_parse(const char *input, unsigned len)
{
	int rlen;
	return parseval(input, len, &rlen);
}

bencode_val *parseval(const char *input, unsigned len, unsigned *rlen)
{
	if(len < 2)
		return 0;
	switch(input[0]) {
	case 'i':
		return (bencode_val *)parseinteger(input, len, rlen);
	case 'l':
		return (bencode_val *)parselist(input, len, rlen);
	case 'd':
		return (bencode_val *)parsedict(input, len, rlen);
	default:
		return (bencode_val *)parsestring(input, len, rlen);
	}
}

bencode_integer *parseinteger(const char *input, unsigned len, unsigned *rlen)
{
	int n;
	int negative;
	int val;
	bencode_integer *out;

	n = 0;
	val = 0;
	if(len < 2)
		return 0;
	if(input[0] != 'i')
		return 0;
	n++;

	negative = 0;
	if(input[n] == '-') {
		negative = 1;
		n++;
	}

	while(n < len) {
		if(input[n] >= '0' && input[n] <= '9') {
			val *= 10;
			val += input[n] - '0';
			n++;
			continue;
		} else if(input[n] == 'e') {
			n++;
			break;
		}
		return 0;
	}

	if(negative)
		val = -val;
	out = calloc(1, sizeof(bencode_integer));
	out->type = BENCODE_INTEGER;
	out->val = val;
	*rlen = n;

	return out;
}

bencode_string *parsestring(const char *input, unsigned len, unsigned *rlen)
{
	char *val;
	int slen;
	int n;
	bencode_string *out;

	slen = 0;
	n = 0;

	while(n < len) {
		if(input[n] >= '0' && input[n] <= '9') {
			slen *= 10;
			slen += input[n] - '0';
			n++;
			continue;
		} else if(input[n] == ':') {
			n++;
			break;
		}
		return 0;
	}

	if(slen > len - n)
		return 0;

	val = malloc(slen + 1);
	val[slen] = '\0';
	strncpy(val, input + n, slen);
	n += slen;

	out = calloc(1, sizeof(bencode_string));
	out->type = BENCODE_STRING;
	out->val = val;
	out->len = slen;
	*rlen = n;

	return out;
}

bencode_list *parselist(const char *input, unsigned len, unsigned *rlen)
{
	int n;
	int eln;
	bencode_val *el;
	bencode_list *out;

	n = 0;

	if(len < 2)
		return 0;

	if(input[0] != 'l')
		return 0;
	n++;

	out = calloc(1, sizeof(bencode_list));

	while(n < len) {
		if(input[n] == 'e') {
			n++;
			goto ret;
		}
		el = parseval(input + n, len - n, &eln);
		if(el == 0)
			goto err;
		bencode_list_add(out, el);
		n += eln;
	}
err:
	bencode_free_recursive((bencode_val *)out);
	return 0;
ret:
	*rlen = n;
	out->type = BENCODE_LIST;
	return out;
}

void bencode_free_recursive(bencode_val *val)
{
	int i;

	switch(val->type) {
	case BENCODE_STRING:
		free(val->string.val);
	case BENCODE_INTEGER:
		free(val);
		return;
	case BENCODE_LIST:
		for(i = 0; i < val->list.nvals; i++) {
			bencode_free_recursive(val->list.vals[i]);	
			free(val->list.vals);
		}
		free(val);
		return;
	case BENCODE_DICT:
		for(i = 0; i < val->dict.nvals; i++) {
			bencode_free_recursive((bencode_val *)(val->dict.keys[i]));
			free(val->dict.keys);
			bencode_free_recursive((bencode_val *)(val->dict.vals[i]));
			free(val->dict.vals);
		}
		free(val);
		return;
	}
}

void bencode_list_add(bencode_list *list, bencode_val *val)
{
	list->nvals++;
	list->vals = realloc(list->vals, sizeof(bencode_val *) * list->nvals);
	list->vals[list->nvals - 1] = val;
}

bencode_dict *parsedict(const char *input, unsigned len, unsigned *rlen)
{
	int n;
	int eln;
	bencode_string *key;
	bencode_val *el;
	bencode_dict *out;

	n = 0;

	if(len < 2)
		return 0;

	if(input[0] != 'd')
		return 0;
	n++;

	out = calloc(1, sizeof(bencode_dict));

	while(n < len) {
		if(input[n] == 'e') {
			n++;
			goto ret;
		}
		key = parsestring(input +n, len - n, &eln);
		if(key == 0)
			goto err;
		n += eln;
		el = parseval(input + n, len - n, &eln);
		if(el == 0) {
			bencode_free_recursive((bencode_val *)key);
			goto err;
		}
		bencode_dict_add(out, key, el);
		n += eln;
	}
err:
	bencode_free_recursive((bencode_val *)out);
	return 0;
ret:
	*rlen = n;
	out->type = BENCODE_DICT;
	return out;
}

void bencode_dict_add(bencode_dict *dict, bencode_string *key, bencode_val *val)
{
	dict->nvals++;
	dict->keys = realloc(dict->keys, sizeof(bencode_string *) * dict->nvals);
	dict->vals = realloc(dict->vals, sizeof(bencode_val *) * dict->nvals);
	dict->keys[dict->nvals - 1] = key;
	dict->vals[dict->nvals - 1] = val;
}
