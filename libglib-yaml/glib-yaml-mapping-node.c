#include "glib-yaml-node.h"

#include "glib-yaml-stream.h"

G_DEFINE_TYPE (GLibYAMLMappingNode, glib_yaml_mapping_node, GLIB_YAML_NODE_TYPE)

static void   finalize  (GObject *);
static gchar *to_string (GLibYAMLNode *, guint);

GLibYAMLMappingNode *
glib_yaml_mapping_node_new ()
{
	return g_object_new (GLIB_YAML_MAPPING_NODE_TYPE, NULL);
}

void
glib_yaml_mapping_node_add (GLibYAMLMappingNode *this, GLibYAMLNode *key, GLibYAMLNode *value)
{
	if (this->map == NULL)
		this->map = g_hash_table_new_full (glib_yaml_node_hash, glib_yaml_node_equal, g_object_unref, g_object_unref);

	g_hash_table_insert (this->map, key, value);
}

GLibYAMLNode *
glib_yaml_mapping_node_find_by_scalar (GLibYAMLMappingNode *this, const gchar *key)
{
	GLibYAMLScalarNode *key_node;
	GLibYAMLNode       *value_node;


	key_node   = glib_yaml_scalar_node_new (key);
	value_node = GLIB_YAML_NODE (g_hash_table_lookup (this->map, key_node));

	g_object_unref (key_node);

	return value_node;
}

gboolean
glib_yaml_mapping_node_get_boolean (GLibYAMLMappingNode *this, const gchar *key)
{
	return glib_yaml_scalar_node_get_boolean (
		GLIB_YAML_SCALAR_NODE (
			glib_yaml_mapping_node_find_by_scalar (this, key)));
}

guint
glib_yaml_mapping_node_get_uint (GLibYAMLMappingNode *this, const gchar *key)
{
	return glib_yaml_scalar_node_get_uint (
		GLIB_YAML_SCALAR_NODE (
			glib_yaml_mapping_node_find_by_scalar (this, key)));
}

gint
glib_yaml_mapping_node_get_int (GLibYAMLMappingNode *this, const gchar *key)
{
	return glib_yaml_scalar_node_get_int (
		GLIB_YAML_SCALAR_NODE (
			glib_yaml_mapping_node_find_by_scalar (this, key)));
}

gdouble
glib_yaml_mapping_node_get_double (GLibYAMLMappingNode *this, const gchar *key)
{
	return glib_yaml_scalar_node_get_double (
		GLIB_YAML_SCALAR_NODE (
			glib_yaml_mapping_node_find_by_scalar (this, key)));
}

const gchar *
glib_yaml_mapping_node_get_string (GLibYAMLMappingNode *this, const gchar *key)
{
	return glib_yaml_scalar_node_get_string (
		GLIB_YAML_SCALAR_NODE (
			glib_yaml_mapping_node_find_by_scalar (this, key)));
}

static void
glib_yaml_mapping_node_class_init (GLibYAMLMappingNodeClass *this_class)
{
	G_OBJECT_CLASS       (this_class)->finalize  = finalize;
	GLIB_YAML_NODE_CLASS (this_class)->to_string = to_string;
}

static void
glib_yaml_mapping_node_init (GLibYAMLMappingNode *this)
{
	this->map = NULL;
}

static void
finalize (GObject *g_object)
{
	g_hash_table_unref (GLIB_YAML_MAPPING_NODE (g_object)->map);

	G_OBJECT_CLASS (glib_yaml_mapping_node_parent_class)->finalize (g_object);
}

static gchar *
to_string (GLibYAMLNode *glib_yaml_node, guint indent_level)
{
	GLibYAMLMappingNode *this = GLIB_YAML_MAPPING_NODE (glib_yaml_node);

	gchar *node_string;
	gchar *child_string;

	GString *buf;
	gchar   *indent_string;
	gchar   *indent_string_labels;

	GHashTableIter iter;
	gpointer       key, value;


	indent_string        = glib_yaml_stream_get_indent_string (indent_level);
	indent_string_labels = glib_yaml_stream_get_indent_string (indent_level + 1);

	buf = g_string_new (NULL);

	g_string_append_printf (buf, "%sMAPPING-START\n", indent_string);

	g_hash_table_iter_init (& iter, this->map);

	while (g_hash_table_iter_next (& iter, & key, & value)) {
		g_string_append_printf (buf, "%sKEY\n", indent_string_labels);

		child_string = GLIB_YAML_NODE_GET_CLASS (key)->to_string (GLIB_YAML_NODE (key), indent_level + 2);
		g_string_append (buf, child_string);
		g_free (child_string);

		g_string_append_printf (buf, "%sVALUE\n", indent_string_labels);

		child_string = GLIB_YAML_NODE_GET_CLASS (value)->to_string (GLIB_YAML_NODE (value), indent_level + 2);
		g_string_append (buf, child_string);
		g_free (child_string);
	}

	g_string_append_printf (buf, "%sMAPPING-END\n", indent_string);

	g_free (indent_string);
	g_free (indent_string_labels);

	node_string = g_string_free (buf, FALSE);

	return node_string;
}
