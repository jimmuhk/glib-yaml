#include "glib-yaml-node.h"

#include <glib/gprintf.h>

#include "glib-yaml-stream.h"

G_DEFINE_TYPE (GLibYAMLNode, glib_yaml_node, G_TYPE_OBJECT)

typedef struct {
	gboolean  assigned;
	guint    *hash_key;
} GLibYAMLNodePrivate;

#define GLIB_YAML_NODE_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), GLIB_YAML_NODE_TYPE, GLibYAMLNodePrivate))

static void finalize (GObject *);

GLibYAMLNode *
glib_yaml_node_new ()
{
	return g_object_new (GLIB_YAML_NODE_TYPE, NULL);
}

void
glib_yaml_node_assign_as_alias (GLibYAMLNode *this, GLibYAMLNode *anchor_node)
{
	this->type       = GLIB_YAML_ALIAS_NODE;
	this->data.alias = g_object_ref (anchor_node);

	GLIB_YAML_NODE_GET_PRIVATE (this)->assigned = TRUE;
}

void
glib_yaml_node_assign_as_scalar (GLibYAMLNode *this, const gchar *scalar)
{
	this->type        = GLIB_YAML_SCALAR_NODE;
	this->data.scalar = g_strdup (scalar);

	GLIB_YAML_NODE_GET_PRIVATE (this)->assigned = TRUE;
}

void
glib_yaml_node_assign_as_sequence (GLibYAMLNode *this)
{
	this->type          = GLIB_YAML_SEQUENCE_NODE;
	this->data.sequence = g_ptr_array_new_with_free_func (g_object_unref);

	GLIB_YAML_NODE_GET_PRIVATE (this)->assigned = TRUE;
}

void
glib_yaml_node_assign_as_mapping (GLibYAMLNode *this)
{
	this->type         = GLIB_YAML_MAPPING_NODE;
	this->data.mapping = g_hash_table_new_full (g_direct_hash, g_direct_equal, g_object_unref, g_object_unref);

	GLIB_YAML_NODE_GET_PRIVATE (this)->assigned = TRUE;
}

void
glib_yaml_node_add_sequence_element (GLibYAMLNode *this, GLibYAMLNode *node)
{
	g_assert (this->type == GLIB_YAML_SEQUENCE_NODE);

	g_ptr_array_add (this->data.sequence, node);
}

void
glib_yaml_node_add_mapping_element (GLibYAMLNode *this, GLibYAMLNode *key, GLibYAMLNode *value)
{
	g_assert (this->type == GLIB_YAML_MAPPING_NODE);

	g_hash_table_insert (this->data.mapping, key, value);
}

void
glib_yaml_node_dump_to_file_handle (GLibYAMLNode *this, FILE *file_handle, gint indent_level)
{
	gchar *indent_string;

	GHashTableIter iter;
	gpointer       key, value;

	gint i;


	indent_string = glib_yaml_stream_get_indent_string (indent_level);

	switch (this->type) {
		case GLIB_YAML_ALIAS_NODE:
			g_fprintf (file_handle, "%sALIAS\n", indent_string);

			if (IS_GLIB_YAML_NODE (this->data.alias))
				glib_yaml_node_dump_to_file_handle (this->data.alias, file_handle, indent_level + 1);
			else
				g_fprintf (file_handle, "%s  MISSING ANCHOR\n", indent_string);

			break;

		case GLIB_YAML_SCALAR_NODE:
			g_fprintf (file_handle, "%sSCALAR [%s]\n", indent_string, this->data.scalar);
			break;

		case GLIB_YAML_SEQUENCE_NODE:
			g_fprintf (file_handle, "%sSEQUENCE-START\n", indent_string);

			for (i = 0; i < this->data.sequence->len; ++ i)
				glib_yaml_node_dump_to_file_handle (
					(GLibYAMLNode *) g_ptr_array_index (this->data.sequence, i),
					file_handle,
					indent_level + 1);

			g_fprintf (file_handle, "%sSEQUENCE-END\n", indent_string);

			break;

		case GLIB_YAML_MAPPING_NODE:
			g_fprintf (file_handle, "%sMAPPING-START\n", indent_string);

			g_hash_table_iter_init (& iter, this->data.mapping);

			while (g_hash_table_iter_next (& iter, & key, & value)) {
				g_fprintf (file_handle, "%sKEY\n", indent_string);
				glib_yaml_node_dump_to_file_handle ((GLibYAMLNode *) key, file_handle, indent_level + 2);

				g_fprintf (file_handle, "%sVALUE\n", indent_string);
				glib_yaml_node_dump_to_file_handle ((GLibYAMLNode *) value, file_handle, indent_level + 2);
			}

			g_fprintf (file_handle, "%sMAPPING-END\n", indent_string);

			break;
	}

	g_free (indent_string);
}

static void
glib_yaml_node_class_init (GLibYAMLNodeClass *this_class)
{
	G_OBJECT_CLASS (this_class)->finalize = finalize;

	g_type_class_add_private (this_class, sizeof (GLibYAMLNodePrivate));
}

static void
glib_yaml_node_init (GLibYAMLNode *this)
{
	GLIB_YAML_NODE_GET_PRIVATE (this)->assigned = FALSE;
}

static void
finalize (GObject *g_object)
{
	GLibYAMLNode *this = GLIB_YAML_NODE (g_object);
	GLibYAMLNodePrivate *priv = GLIB_YAML_NODE_GET_PRIVATE (this);


	if (priv->assigned) {
		switch (this->type) {
			case GLIB_YAML_ALIAS_NODE:
				g_object_unref (this->data.alias);
				break;

			case GLIB_YAML_SCALAR_NODE:
				g_free (this->data.scalar);
				break;

			case GLIB_YAML_SEQUENCE_NODE:
				g_ptr_array_free (this->data.sequence, TRUE);
				break;

			case GLIB_YAML_MAPPING_NODE:
				g_hash_table_unref (this->data.mapping);
				break;
		}
	}

	(* G_OBJECT_CLASS (glib_yaml_node_parent_class)->finalize) (g_object);
}
