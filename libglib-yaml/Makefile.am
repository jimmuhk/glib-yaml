AM_CPPFLAGS = $(LIBGLIB_YAML_CFLAGS)

lib_LTLIBRARIES = libglib-yaml.la

INSTALL_INCLUDE_DIR = $(includedir)/glib-yaml

libglib_yaml_subincludedir = $(INSTALL_INCLUDE_DIR)/yaml-components
libglib_yaml_subinclude_HEADERS = \
	glib-yaml-parser.h   \
	glib-yaml-stream.h   \
	glib-yaml-document.h \
	glib-yaml-node.h

libglib_yaml_includedir = $(INSTALL_INCLUDE_DIR)
libglib_yaml_include_HEADERS = glib-yaml.h

libglib_yaml_la_SOURCES = \
	glib-yaml-parser.c   \
	glib-yaml-stream.c   \
	glib-yaml-document.c \
	glib-yaml-node.c

libglib_yaml_la_LIBADD = $(LIBGLIB_YAML_LIBS)
