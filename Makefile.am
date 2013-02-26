ACLOCAL_AMFLAGS = -I m4
SUBDIRS = tests

bin_PROGRAMS = ben2js

ben2js_SOURCES = ben2js.c bencoding.c json.c bencoding.h
ben2js_CFLAGS = $(IO_CFLAGS)
ben2js_LDADD = $(IO_LIBS)

ivartjincludedir = $(includedir)/ivartj
ivartjinclude_HEADERS = bencoding.h

lib_LTLIBRARIES = libivartjbencoding.la
libivartjbencoding_la_SOURCES = bencoding.h bencoding.c json.c
libivartjbencoding_la_CFLAGS = $(IO_CFLAGS)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = ivartjbencoding.pc 
