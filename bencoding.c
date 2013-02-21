#include "bencoding.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

static bencode_string *parsestring(const char *input, size_t len, size_t *rlen);
static bencode_integer *parseinteger(const char *input, size_t len, size_t *rlen);
static bencode_list *parselist(const char *input, size_t len, size_t *rlen);
static bencode_dict *parsedict(const char *input, size_t len, size_t *rlen);
static bencode_val *parseval(const char *input, size_t len, size_t *rlen);
static size_t writeval(bencode_val *val, char *str);
static size_t valstrlen(bencode_val *val);

bencode_val *bencode_parse(const char *input, size_t len)
{
	size_t rlen;
	return parseval(input, len, &rlen);
}

bencode_val *parseval(const char *input, size_t len, size_t *rlen)
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

bencode_integer *parseinteger(const char *input, size_t len, size_t *rlen)
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

bencode_string *parsestring(const char *input, size_t len, size_t *rlen)
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
	memcpy(val, input + n, slen);
	n += slen;

	out = calloc(1, sizeof(bencode_string));
	out->type = BENCODE_STRING;
	out->val = val;
	out->len = slen;
	*rlen = n;

	return out;
}

bencode_list *parselist(const char *input, size_t len, size_t *rlen)
{
	size_t n;
	size_t eln;
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
		}
		free(val->list.vals);
		free(val);
		return;
	case BENCODE_DICT:
		for(i = 0; i < val->dict.nvals; i++) {
			bencode_free_recursive((bencode_val *)(val->dict.keys[i]));
			bencode_free_recursive((bencode_val *)(val->dict.vals[i]));
		}
		free(val->dict.keys);
		free(val->dict.vals);
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

bencode_dict *parsedict(const char *input, size_t len, size_t *rlen)
{
	size_t n;
	size_t eln;
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
		key = parsestring(input + n, len - n, &eln);
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

/* Should insert sort in the future, according the bencoding spec */
void bencode_dict_add(bencode_dict *dict, bencode_string *key, bencode_val *val)
{
	dict->nvals++;
	dict->keys = realloc(dict->keys, sizeof(bencode_string *) * dict->nvals);
	dict->vals = realloc(dict->vals, sizeof(bencode_val *) * dict->nvals);
	dict->keys[dict->nvals - 1] = key;
	dict->vals[dict->nvals - 1] = val;
}

char *bencode_val_string(bencode_val *val, size_t *rlen)
{
	size_t len;
	char *out;

	len = valstrlen(val);
	out = malloc(len + 1);
	out[len] = '\0';

	writeval(val, out);

	if(rlen != NULL)
		*rlen = len;

	return out;
}

size_t writeval(bencode_val *val, char *str)
{
	size_t n;
	int i;

	n = 0;

	switch(val->type) {
	case BENCODE_STRING:
		n += sprintf(str, "%u:", val->string.len);
		memcpy(str + n, val->string.val, val->string.len);
		n += val->string.len;
		return n;
	case BENCODE_INTEGER:
		n += sprintf(str, "i%de", val->integer.val);
		return n;
	case BENCODE_LIST:
		str[0] = 'l';
		n++;
		for(i = 0; i < val->list.nvals; i++)
			n += writeval(val->list.vals[i], str + n);
		str[n] = 'e';
		n++;
		return n;
	case BENCODE_DICT:
		str[0] = 'd';
		n++;
		for(i = 0; i < val->dict.nvals; i++) {
			n += writeval((bencode_val *)(val->dict.keys[i]), str + n);
			n += writeval(val->dict.vals[i], str + n);
		}
		str[n] = 'e';
		n++;
		return n;
	}
}

size_t intstrlen(int i)
{
	size_t len;

	len = 0;

	if(i < 0) {
		len++;
		i *= -1;
	}

	if(i == 0)
		len++;

	while(i) {
		len++;
		i /= 10;
	}

	return len;
}

size_t valstrlen(bencode_val *val)
{
	size_t sum;
	int i;

	sum = 0;

	switch(val->type) {
	case BENCODE_STRING:
		return intstrlen(val->string.len) + 1 + val->string.len;
	case BENCODE_INTEGER:
		return 1 + intstrlen(val->integer.val) + 1;
	case BENCODE_LIST:
		sum = 1;	
		for(i = 0; i < val->list.nvals; i++)
			sum += valstrlen(val->list.vals[i]);
		sum += 1;
		return sum;
	case BENCODE_DICT:
		sum = 1;	
		for(i = 0; i < val->dict.nvals; i++) {
			sum += valstrlen((bencode_val *)(val->dict.keys[i]));
			sum += valstrlen(val->dict.vals[i]);
		}
		sum += 1;
		return sum;
	}

	return 0;
}

bencode_val *bencode_dict_get(bencode_dict *dict, const char *key)
{
	int i;

	if(dict->type != BENCODE_DICT)
		return NULL;

	for(i = 0; i < dict->nvals; i++)
		if(strcmp(dict->keys[i]->val, key) == 0)
			break;

	if(i == dict->nvals)
		return NULL;

	return dict->vals[i];
}
