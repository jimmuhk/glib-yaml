#ifndef __GLIB_YAML_STREAM_H__
#define __GLIB_YAML_STREAM_H__

#include <stdio.h>
#include <yaml.h>
#include <glib-object.h>

#define GLIB_YAML_STREAM_TYPE                    (glib_yaml_stream_get_type ())
#define GLIB_YAML_STREAM(obj_instance)           (G_TYPE_CHECK_INSTANCE_CAST ((obj_instance), GLIB_YAML_STREAM_TYPE, GLibYAMLStream))
#define IS_GLIB_YAML_STREAM(obj_instance)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj_instance), GLIB_YAML_STREAM_TYPE))
#define GLIB_YAML_STREAM_CLASS(obj_class)        (G_TYPE_CHECK_CLASS_CAST ((obj_class), GLIB_YAML_STREAM_TYPE, GLibYAMLStreamClass))
#define IS_GLIB_YAML_STREAM_CLASS(obj_class)     (G_TYPE_CHECK_CLASS_TYPE ((obj_class), GLIB_YAML_STREAM_TYPE))
#define GLIB_YAML_STREAM_GET_CLASS(obj_instance) (G_TYPE_INSTANCE_GET_CLASS ((obj_instance), GLIB_YAML_STREAM_TYPE, GLibYAMLStreamClass))

typedef struct {
	GObjectClass g_object_class;
} GLibYAMLStreamClass;

typedef struct {
	GObject g_object;

	yaml_encoding_t  encoding;
	GList           *documents;
} GLibYAMLStream;

GType glib_yaml_stream_get_type (void);

GLibYAMLStream *glib_yaml_stream_load_from_file_path (const gchar *yaml_path, GError **error);

void   glib_yaml_stream_dump_to_file_handle (GLibYAMLStream *stream, FILE *handle);
gchar *glib_yaml_stream_get_indent_string   (gint level);

#endif
