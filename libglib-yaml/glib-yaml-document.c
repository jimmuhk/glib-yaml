#include "glib-yaml-document.h"

#include <glib/gprintf.h>

#include "glib-yaml-stream.h"

G_DEFINE_TYPE (GLibYAMLDocument, glib_yaml_document, G_TYPE_OBJECT)

typedef struct {
	GHashTable *anchors;
} GLibYAMLDocumentPrivate;

#define GLIB_YAML_DOCUMENT_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), GLIB_YAML_DOCUMENT_TYPE, GLibYAMLDocumentPrivate))

static void finalize (GObject *);

GLibYAMLDocument *
glib_yaml_document_new ()
{
	return g_object_new (GLIB_YAML_DOCUMENT_TYPE, NULL);
}

void
glib_yaml_document_set_version_directive (GLibYAMLDocument *this, gint major_number, gint minor_number)
{
	this->version_directive.major_number = major_number;
	this->version_directive.minor_number = minor_number;
}

void
glib_yaml_document_add_anchor (GLibYAMLDocument *this, const gchar *anchor_key, GLibYAMLNode *anchor_node)
{
	GLibYAMLDocumentPrivate *priv = GLIB_YAML_DOCUMENT_GET_PRIVATE (this);

	if (priv->anchors == NULL)
		priv->anchors = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, g_object_unref);

	g_hash_table_insert (priv->anchors, g_strdup (anchor_key), g_object_ref (anchor_node));
}

GLibYAMLNode *
glib_yaml_document_get_anchor (GLibYAMLDocument *this, const gchar *anchor_key)
{
	return GLIB_YAML_NODE (g_hash_table_lookup (GLIB_YAML_DOCUMENT_GET_PRIVATE (this)->anchors, anchor_key));
}

void
glib_yaml_document_dump_to_file_handle (GLibYAMLDocument *this, FILE *file_handle, gint indent_level)
{
	gchar *indent_string;


	indent_string = glib_yaml_stream_get_indent_string (indent_level);

	g_fprintf (file_handle, "%sDOCUMENT-START\n", indent_string);
	g_fprintf (file_handle, "%s  version: ", indent_string);

	if (this->version_directive.major_number >= 0)
		g_fprintf (file_handle, "%d.%d\n", this->version_directive.major_number, this->version_directive.minor_number);
	else
		g_fprintf (file_handle, "unspecified\n");

	glib_yaml_node_dump_to_file_handle (GLIB_YAML_NODE (this->root), file_handle, indent_level + 1);

	g_fprintf (file_handle, "%sDOCUMENT-END\n", indent_string);

	g_free (indent_string);
}

static void
glib_yaml_document_class_init (GLibYAMLDocumentClass *this_class)
{
	G_OBJECT_CLASS (this_class)->finalize = finalize;

	g_type_class_add_private (this_class, sizeof (GLibYAMLDocumentPrivate));
}

static void
glib_yaml_document_init (GLibYAMLDocument *this)
{
	this->root = NULL;

	this->version_directive.major_number = -1;
	this->version_directive.minor_number = -1;

	GLIB_YAML_DOCUMENT_GET_PRIVATE (this)->anchors = NULL;
}

static void
finalize (GObject *g_object)
{
	g_object_unref     (GLIB_YAML_DOCUMENT             (g_object)->root);
	g_hash_table_unref (GLIB_YAML_DOCUMENT_GET_PRIVATE (g_object)->anchors);

	(* G_OBJECT_CLASS (glib_yaml_document_parent_class)->finalize) (g_object);
}
