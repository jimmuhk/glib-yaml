#include "glib-yaml-node.h"

#include "glib-yaml-stream.h"

G_DEFINE_TYPE (GLibYAMLScalarNode, glib_yaml_scalar_node, GLIB_YAML_NODE_TYPE)

static void   finalize  (GObject *);
static gchar *to_string (GLibYAMLNode *, guint);

GLibYAMLScalarNode *
glib_yaml_scalar_node_new (const gchar *value)
{
	GLibYAMLScalarNode *this;

	this        = g_object_new (GLIB_YAML_SCALAR_NODE_TYPE, NULL);
	this->value = g_strdup (value);

	return this;
}

static void
glib_yaml_scalar_node_class_init (GLibYAMLScalarNodeClass *this_class)
{
	G_OBJECT_CLASS       (this_class)->finalize  = finalize;
	GLIB_YAML_NODE_CLASS (this_class)->to_string = to_string;
}

static void
glib_yaml_scalar_node_init (GLibYAMLScalarNode *this)
{
	this->value = NULL;
}

static void
finalize (GObject *g_object)
{
	g_free (GLIB_YAML_SCALAR_NODE (g_object)->value);

	G_OBJECT_CLASS (glib_yaml_scalar_node_parent_class)->finalize (g_object);
}

static gchar *
to_string (GLibYAMLNode *glib_yaml_node, guint indent_level)
{
	GLibYAMLScalarNode *this = GLIB_YAML_SCALAR_NODE (glib_yaml_node);

	gchar *node_string;
	gchar *indent_string;


	indent_string = glib_yaml_stream_get_indent_string (indent_level);
	node_string   = g_strdup_printf ("%sSCALAR [%s]\n", indent_string, this->value);

	g_free (indent_string);

	return node_string;
}
