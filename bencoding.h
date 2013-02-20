#ifndef BENCODING_H
#define BENCODING_H

#include <stdio.h>

#define BENCODE_INTEGER 1
#define BENCODE_STRING 2
#define BENCODE_LIST 4
#define BENCODE_DICT 8

typedef struct bencode_string bencode_string;
typedef struct bencode_integer bencode_integer;
typedef struct bencode_list bencode_list;
typedef struct bencode_dict bencode_dict;

typedef union bencode_val bencode_val;

struct bencode_string {
	int type;
	char *val;
	size_t len;
};

struct bencode_integer {
	int type;
	int val;
};

struct bencode_list {
	int type;
	bencode_val **vals;
	int nvals;
};

struct bencode_dict {
	int type;
	bencode_string **keys;
	bencode_val **vals;
	int nvals;
};


union bencode_val {
	int type;
	bencode_string string;
	bencode_integer integer;
	bencode_list list;
	bencode_dict dict;
};

bencode_val *bencode_parse(const char *input, size_t len);
char *bencode_val_string(bencode_val *val, size_t *rlen);
char *bencode_val_json(bencode_val *val, size_t *rlen);

void bencode_free_recursive(bencode_val *val);
void bencode_list_add(bencode_list *list, bencode_val *val);
void bencode_dict_add(bencode_dict *dict, bencode_string *key, bencode_val *val);


#endif
