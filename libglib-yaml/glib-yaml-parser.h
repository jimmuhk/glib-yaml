#ifndef __GLIB_YAML_PARSER_H__
#define __GLIB_YAML_PARSER_H__

#include <glib.h>
#include <stdio.h>

#include "glib-yaml-stream.h"

#define GLIB_YAML_PARSER_ERROR_DOMAIN glib_yaml_parser_error_quark ()
#define GLIB_YAML_PARSER_ERROR        1

GQuark glib_yaml_parser_error_quark (void);

gboolean glib_yaml_parser_parse_stream (GLibYAMLStream *stream, FILE *yaml_file_handle, GError **error);

#endif
