TESTS = integer string list dict valstring metafile json jsonmetafile

check_PROGRAMS = $(TESTS)

integer_SOURCES = integer.c
integer_LDADD = $(IO_LIBS)
string_SOURCES = string.c
string_LDADD = $(IO_LIBS)
list_SOURCES = list.c
list_LDADD = $(IO_LIBS)
dict_SOURCES = dict.c
dict_LDADD = $(IO_LIBS)

valstring_SOURCES = valstring.c
valstring_LDADD = $(IO_LIBS)

metafile_SOURCES = metafile.c
metafile_LDADD = ../libivartjbencoding.la $(IO_LIBS)

json_SOURCES = json.c
json_LDADD = ../libivartjbencoding.la $(IO_LIBS)

jsonmetafile_SOURCES = jsonmetafile.c
jsonmetafile_LDADD = ../libivartjbencoding.la $(IO_LIBS)
