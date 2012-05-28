#include "glib-yaml-node.h"

G_DEFINE_TYPE (GLibYAMLNode, glib_yaml_node, G_TYPE_OBJECT)

GLibYAMLNode *
glib_yaml_node_path_get_node (GLibYAMLNode *this, const gchar *path)
{
	GLibYAMLNode *node;

	gchar **map_keys;

	gint i;


	node = this;

	map_keys = g_strsplit (path, ".", 0);

	for (i = 0; map_keys [i] != NULL; ++ i) {
		g_assert (IS_GLIB_YAML_MAPPING_NODE (node));

		node = glib_yaml_mapping_node_find_by_scalar (GLIB_YAML_MAPPING_NODE (node), map_keys [i]);
	}

	g_strfreev (map_keys);

	return node;
}

gboolean
glib_yaml_node_path_get_boolean (GLibYAMLNode *this, const gchar *path)
{
	return glib_yaml_scalar_node_get_boolean (GLIB_YAML_SCALAR_NODE (glib_yaml_node_path_get_node (this, path)));
}

guint
glib_yaml_node_path_get_uint (GLibYAMLNode *this, const gchar *path)
{
	return glib_yaml_scalar_node_get_uint (GLIB_YAML_SCALAR_NODE (glib_yaml_node_path_get_node (this, path)));
}

gint
glib_yaml_node_path_get_int (GLibYAMLNode *this, const gchar *path)
{
	return glib_yaml_scalar_node_get_int (GLIB_YAML_SCALAR_NODE (glib_yaml_node_path_get_node (this, path)));
}

gdouble
glib_yaml_node_path_get_double (GLibYAMLNode *this, const gchar *path)
{
	return glib_yaml_scalar_node_get_double (GLIB_YAML_SCALAR_NODE (glib_yaml_node_path_get_node (this, path)));
}

const gchar *
glib_yaml_node_path_get_string (GLibYAMLNode *this, const gchar *path)
{
	return glib_yaml_scalar_node_get_string (GLIB_YAML_SCALAR_NODE (glib_yaml_node_path_get_node (this, path)));
}

guint
glib_yaml_node_hash (gconstpointer key)
{
	GLibYAMLNode *key_node;

	gchar *node_string;
	guint  node_hash;


	key_node    = GLIB_YAML_NODE (key);
	node_string = GLIB_YAML_NODE_GET_CLASS (key_node)->to_string (key_node, 0);

	node_hash = g_str_hash (node_string);

	g_free (node_string);

	return node_hash;
}

gboolean
glib_yaml_node_equal (gconstpointer a, gconstpointer b)
{
	GLibYAMLNode *a_node;
	GLibYAMLNode *b_node;

	gchar *a_string;
	gchar *b_string;

	gboolean equal;


	a_node = GLIB_YAML_NODE (a);
	b_node = GLIB_YAML_NODE (b);

	a_string = GLIB_YAML_NODE_GET_CLASS (a_node)->to_string (a_node, 0);
	b_string = GLIB_YAML_NODE_GET_CLASS (b_node)->to_string (b_node, 0);

	equal = g_str_equal (a_string, b_string);

	g_free (a_string);
	g_free (b_string);

	return equal;
}

void
glib_yaml_node_dump_to_file_handle (GLibYAMLNode *this, FILE *file_handle, gint indent_level)
{
	gchar *node_string;

	fputs (node_string = GLIB_YAML_NODE_GET_CLASS (this)->to_string (this, indent_level), file_handle);

	g_free (node_string);
}

static void
glib_yaml_node_class_init (GLibYAMLNodeClass *this_class)
{}

static void
glib_yaml_node_init (GLibYAMLNode *this)
{}
