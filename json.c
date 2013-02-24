#include "bencoding.h"
#include <stdlib.h>
#include <assert.h>
#include <ivartj/io.h>
#include <string.h>

static size_t writechar(unsigned char c, io_writer *w);
static size_t writeval(bencode_val *val, io_writer *w, unsigned ind);
static size_t writestring(bencode_val *val, io_writer *w, unsigned ind);
static size_t writeinteger(bencode_val *val, io_writer *w, unsigned ind);
static size_t writelist(bencode_val *val, io_writer *w, unsigned ind);
static size_t writedict(bencode_val *val, io_writer *w, unsigned ind);
static size_t indent(io_writer *w, unsigned ind);

char *bencode_val_json(bencode_val *val, size_t *rlen)
{
	size_t len;
	io_buf buf;
	io_writer w;

	memset(&buf, 0, sizeof(buf));
	w.write = io_bufwrite;
	w.data = &buf;

	len = writeval(val, &w, 0);
	
	if(rlen != NULL)
		*rlen = len;

	return buf.buf;
}

size_t writeval(bencode_val *val, io_writer *w, unsigned ind)
{
	switch(val->type) {
	case BENCODE_STRING:
		return writestring(val, w, ind);
	case BENCODE_INTEGER:
		return writeinteger(val, w, ind);
	case BENCODE_LIST:
		return writelist(val, w, ind);
	case BENCODE_DICT:
		return writedict(val, w, ind);
	}
}

size_t writestring(bencode_val *val, io_writer *w, unsigned ind)
{

	bencode_string *string;
	size_t n;
	int i;

	n = 0;

	string = &(val->string);

	
	n += io_putc(w, '\"');

	for(i = 0; i < string->len; i++)
		n += writechar(string->val[i], w);

	n += io_putc(w, '\"');

	return n;
}

size_t writechar(unsigned char c, io_writer *w)
{
	size_t n;

	if(c == '\\')
		return io_printf(w, "\\\\");

	if(c == '\"')
		return io_printf(w, "\\\"");


	if(isspace(c) || (isgraph(c) && isascii(c)))
		return io_putc(w, c);

	n = io_printf(w, "\\x%.2hhX", c);

	return n;
}

size_t writeinteger(bencode_val *val, io_writer *w, unsigned ind)
{
	bencode_integer *integer;

	integer = &(val->integer);

	return io_printf(w, "%d", integer->val);
}

size_t indent(io_writer *w, unsigned ind)
{
	int i;

	for(i = 0; i < ind; i++)
		io_putc(w, '\t');

	return ind;
}

size_t writelist(bencode_val *val, io_writer *w, unsigned ind)
{
	size_t n;
	int i;
	bencode_list *list;

	list = &(val->list);

	if(list->nvals == 0)
		return io_printf(w, "[]");

	n = 0;
	n += io_printf(w, "[\n");

	for(i = 0; i < list->nvals - 1; i++) {
		n += indent(w, ind + 1);
		n += writeval(list->vals[i], w, ind + 1);
		n += io_printf(w, ",\n");
	}

	n += indent(w, ind + 1);
	n += writeval(list->vals[i], w, ind + 1);
	n += io_printf(w, "\n");

	n += indent(w, ind);
	n += io_printf(w, "]");

	return n;
}

size_t writedict(bencode_val *val, io_writer *w, unsigned ind)
{
	size_t n;
	int i;
	bencode_dict *dict;

	dict = &(val->dict);

	if(dict->nvals == 0)
		return io_printf(w, "{}");

	n = 0;
	n += io_printf(w, "{\n");

	for(i = 0; i < dict->nvals - 1; i++) {
		n += indent(w, ind + 1);
		n += writeval((bencode_val *)dict->keys[i], w, ind + 1);
		n += io_printf(w, " : ");
		n += writeval(dict->vals[i], w, ind + 1);
		n += io_printf(w, ",\n");
	}

	n += indent(w, ind + 1);
	n += writeval((bencode_val *)dict->keys[i], w, ind + 1);
	n += io_printf(w, " : ");
	n += writeval(dict->vals[i], w, ind + 1);
	n += io_printf(w, "\n");

	n += indent(w, ind);
	n += io_printf(w, "}");

	return n;
}
