ACLOCAL_AMFLAGS = -I m4
SUBDIRS = tests

bin_PROGRAMS = ben2js

ben2js_SOURCES = ben2js.c bencoding.c json.c bencoding.h
ben2js_CFLAGS = $(AM_CFLAGS)

ivartjincludedir = $(includedir)/ivartj
ivartjinclude_HEADERS = bencoding.h

lib_LTLIBRARIES = libivartjbencoding.la
libivartjbencoding_la_SOURCES = bencoding.h bencoding.c json.c

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = ivartjbencoding.pc 
