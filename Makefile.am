ACLOCAL_AMFLAGS = -I m4
SUBDIRS = tests

ivartjincludedir = $(includedir)/ivartj
ivartjinclude_HEADERS = bencoding.h

lib_LTLIBRARIES = libivartjbencoding.la
libivartjbencoding_la_SOURCES = bencoding.h bencoding.c json.c
libivartjbencoding_la_CFLAGS = $(IO_CFLAGS)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = ivartjbencoding.pc 
