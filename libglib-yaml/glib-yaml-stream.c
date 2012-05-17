#include "glib-document.h"

G_DEFINE_TYPE (GLibYAMLStream, glib_yaml_stream, G_TYPE_OBJECT)

static void finalize (GObject *);

GLibYAMLStream *
glib_yaml_stream_load_from_file_path (const gchar *yaml_path)
{
	GLibYAMLStream *stream;

	stream = g_object_new (GLIB_YAML_STREAM_TYPE, NULL);

	return stream;
}

void
glib_yaml_stream_dump_to_file_handle (GLibYAMLStream *this, FILE *file_handle)
{
	GList *document;

	for (document = g_list_first (this->documents); document; document = g_list_next (document)) {
		g_printf ("DOCUMENT-START\n");

		dump_glib_yaml_node ((GLibYAMLNode *) (document->data), stdout, 1);

		g_printf ("DOCUMENT-END\n");
	}
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

	(*G_OBJECT_CLASS (glib_yaml_stream_parent_class)->finalize) (g_object);
}
