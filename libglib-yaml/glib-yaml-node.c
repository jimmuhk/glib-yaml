#include "glib-yaml-node.h"

G_DEFINE_TYPE (GLibYAMLNode, glib_yaml_node, G_TYPE_OBJECT)

static void finalize (GObject *);

static void write_indent_string (FILE *, gint);

GLibYAMLNode *
glib_yaml_node_new_alias (const gchar *alias)
{
	GLibYAMLNode *this = g_object_new (GLIB_YAML_NODE_TYPE, NULL);

	this->type       = GLIB_YAML_ALIAS_NODE;
	this->data.alias = g_strdup (alias);

	return this;
}

GLibYAMLNode *
glib_yaml_node_new_scalar (const gchar *scalar)
{
	GLibYAMLNode *this = g_object_new (GLIB_YAML_NODE_TYPE, NULL);

	this->type        = GLIB_YAML_SCALAR_NODE;
	this->data.scalar = g_strdup (scalar);

	return this;
}

GLibYAMLNode *
glib_yaml_node_new_sequence ()
{
	GLibYAMLNode *this = g_object_new (GLIB_YAML_NODE_TYPE, NULL);

	this->type          = GLIB_YAML_SEQUENCE_NODE;
	this->data.sequence = g_ptr_array_new_with_free_func (g_object_unref);

	return this;
}

GLibYAMLNode *
glib_yaml_node_new_mapping ()
{
	GLibYAMLNode *this = g_object_new (GLIB_YAML_NODE_TYPE, NULL);

	this->type         = GLIB_YAML_MAPPING_NODE;
	this->data.mapping = g_hash_table_new_full (g_direct_hash, g_direct_equal, g_object_unref, g_object_unref);

	return this;
}

void
glib_yaml_node_add_sequence_element (GLibYAMLNode *this, GLibYAMLNode *node)
{
	g_assert (this->type == GLIB_YAML_SEQUENCE_NODE);

	g_ptr_array_add ((* this)->data.sequence, node);
}

void
glib_yaml_node_add_mapping_element (GLibYAMLNode *this, GLibYAMLNode *key, GLibYAMLNode *value)
{
	g_assert (this->type == GLIB_YAML_MAPPING_NODE);

	g_hash_table_insert ((* this)->data.mapping, key, value);
}

void
glib_yaml_node_dump_to_file_handle (GLibYAMLNode *this, FILE *handle, gint indent_level)
{
	GHashTableIter iter;
	gpointer       key, value;

	gint i;


	write_indent_string (handle, indent_level);

	switch (this->type) {
		case GLIB_YAML_ALIAS_NODE:
			g_fprintf (handle, "ALIAS [%s]\n", this->data.alias);
			break;

		case GLIB_YAML_SCALAR_NODE:
			g_fprintf (handle, "SCALAR [%s]\n", this->data.scalar);
			break;

		case GLIB_YAML_SEQUENCE_NODE:
			g_fprintf (handle, "SEQUENCE-START\n");

			for (i = 0; i < this->data.sequence->len; ++ i)
				glib_yaml_node_dump_to_file_handle (
					(GLibYAMLNode *) g_ptr_array_index (this->data.sequence, i),
					handle,
					indent_level + 1);

			write_indent_string (handle, indent_level);
			g_fprintf (handle, "SEQUENCE-END\n");

			break;

		case GLIB_YAML_MAPPING_NODE:
			g_fprintf (handle, "MAPPING-START\n");

			g_hash_table_iter_init (& iter, this->data.mapping);

			while (g_hash_table_iter_next (& iter, & key, & value)) {
				write_indent_string (handle, indent_level + 1);
				g_fprintf (handle, "KEY\n");

				glib_yaml_node_dump_to_file_handle ((GLibYAMLNode *) key, handle, indent_level + 2);

				write_indent_string (handle, indent_level + 1);
				g_fprintf (handle, "VALUE\n");

				glib_yaml_node_dump_to_file_handle ((GLibYAMLNode *) value, handle, indent_level + 2);
			}

			write_indent_string (handle, indent_level);
			g_fprintf (handle, "MAPPING-END\n");

			break;
	}
}

static void
glib_yaml_node_class_init (GLibYAMLNodeClass *this_class)
{
	G_OBJECT_CLASS (this_class)->finalize = finalize;
}

static void
glib_yaml_node_init (GLibYAMLNode *this)
{
}

static void
finalize (GObject *g_object)
{
	GLibYAMLNode *this = GLIB_YAML_NODE (g_object);

	switch (this->type) {
		case GLIB_YAML_ALIAS_NODE:
			g_free (this->data.alias);
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

	(*G_OBJECT_CLASS (glib_yaml_node_parent_class)->finalize) (g_object);
}

static void
write_indent_string (FILE *handle, gint level)
{
	gint i;

	for (i = 0; i < level; ++ i)
		g_fprintf (handle, "    ");
}
