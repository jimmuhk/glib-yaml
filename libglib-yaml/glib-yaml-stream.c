#include "glib-yaml-stream.h"

#include <glib/gprintf.h>

#include "glib-yaml-parser.h"
#include "glib-yaml-document.h"
#include "glib-yaml-node.h"

G_DEFINE_TYPE (GLibYAMLStream, glib_yaml_stream, G_TYPE_OBJECT)

static void finalize (GObject *);

GLibYAMLStream *
glib_yaml_stream_load_from_file_path (const gchar *yaml_path, GError **error)
{
	GLibYAMLStream *stream;

	FILE *yaml_file_handle;


	stream = g_object_new (GLIB_YAML_STREAM_TYPE, NULL);

	yaml_file_handle = fopen (yaml_path, "r");

	if (! glib_yaml_parser_parse_stream (stream, yaml_file_handle, error)) {
		g_object_unref (stream);

		stream = NULL;
	}

	fclose (yaml_file_handle);

	return stream;
}

void
glib_yaml_stream_dump_to_file_handle (GLibYAMLStream *this, FILE *file_handle)
{
	gchar *encoding;
	GList *document;


	switch (this->encoding) {
		case YAML_ANY_ENCODING:
			encoding = "YAML_ANY_ENCODING";
			break;

		case YAML_UTF8_ENCODING:
			encoding = "YAML_UTF8_ENCODING";
			break;

		case YAML_UTF16LE_ENCODING:
			encoding = "YAML_UTF16LE_ENCODING";
			break;

		case YAML_UTF16BE_ENCODING:
			encoding = "YAML_UTF16BE_ENCODING";
			break;
	}

	g_fprintf (file_handle, "STREAM-START\n");
	g_fprintf (file_handle, "  encoding: %s\n", encoding);

	for (document = g_list_first (this->documents); document; document = g_list_next (document))
		glib_yaml_document_dump_to_file_handle (GLIB_YAML_DOCUMENT (document->data), file_handle, 1);

	g_fprintf (file_handle, "STREAM-END\n");
}

gchar *
glib_yaml_stream_get_indent_string (gint level)
{
	gchar *indent_string;
	gint   i;


	indent_string = g_new (gchar, 4 * level + 1);

	for (i = 0; i < 4 * level; ++ i)
		indent_string [i] = ' ';

	indent_string [4 * level] = '\0';

	return indent_string;
}

static void
glib_yaml_stream_class_init (GLibYAMLStreamClass *this_class)
{
	G_OBJECT_CLASS (this_class)->finalize = finalize;
}

static void
glib_yaml_stream_init (GLibYAMLStream *this)
{
	this->documents = NULL;
}

static void
finalize (GObject *g_object)
{
	g_list_free (GLIB_YAML_STREAM (g_object)->documents);

	(* G_OBJECT_CLASS (glib_yaml_stream_parent_class)->finalize) (g_object);
}
