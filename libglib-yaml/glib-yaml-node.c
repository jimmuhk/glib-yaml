#include "glib-yaml-node.h"

#include <glib/gprintf.h>

#include "glib-yaml-stream.h"

G_DEFINE_TYPE (GLibYAMLNode, glib_yaml_node, G_TYPE_OBJECT)

void
glib_yaml_node_dump_to_file_handle (GLibYAMLNode *this, FILE *file_handle, gint indent_level)
{
	gchar *node_string;

	fputs (node_string = GLIB_YAML_NODE_GET_CLASS (this)->to_string (this, indent_level), file_handle);

	g_free (node_string);
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

static void
glib_yaml_node_class_init (GLibYAMLNodeClass *this_class)
{}

static void
glib_yaml_node_init (GLibYAMLNode *this)
{}
