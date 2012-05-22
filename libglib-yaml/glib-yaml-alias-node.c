#include "glib-yaml-node.h"

#include "glib-yaml-stream.h"

G_DEFINE_TYPE (GLibYAMLAliasNode, glib_yaml_alias_node, GLIB_YAML_NODE_TYPE)

static void   finalize  (GObject *);
static gchar *to_string (GLibYAMLNode *, guint);

GLibYAMLAliasNode *
glib_yaml_alias_node_new (GLibYAMLNode *anchor)
{
	GLibYAMLAliasNode *this;

	this = g_object_new (GLIB_YAML_ALIAS_NODE_TYPE, NULL);

	if (anchor != NULL)
		this->anchor = g_object_ref (anchor);

	return this;
}

static void
glib_yaml_alias_node_class_init (GLibYAMLAliasNodeClass *this_class)
{
	G_OBJECT_CLASS       (this_class)->finalize  = finalize;
	GLIB_YAML_NODE_CLASS (this_class)->to_string = to_string;
}

static void
glib_yaml_alias_node_init (GLibYAMLAliasNode *this)
{
	this->anchor = NULL;
}

static void
finalize (GObject *g_object)
{
	g_object_unref (GLIB_YAML_ALIAS_NODE (g_object)->anchor);

	G_OBJECT_CLASS (glib_yaml_alias_node_parent_class)->finalize (g_object);
}

static gchar *
to_string (GLibYAMLNode *glib_yaml_node, guint indent_level)
{
	GLibYAMLAliasNode *this = GLIB_YAML_ALIAS_NODE (glib_yaml_node);

	gchar *node_string;
	gchar *child_string;

	GString *buf;
	gchar   *indent_string;


	indent_string = glib_yaml_stream_get_indent_string (indent_level);

	buf = g_string_new (NULL);

	g_string_append_printf (buf, "%sALIAS\n", indent_string);

	if (this->anchor != NULL) {
		child_string = GLIB_YAML_NODE_GET_CLASS (this->anchor)->to_string (GLIB_YAML_NODE (this->anchor), indent_level + 2);
		g_string_append (buf, child_string);
		g_free (child_string);
	}
	else
		g_string_append_printf (buf,"%s  MISSING ANCHOR\n", indent_string);

	g_free (indent_string);

	node_string = g_string_free (buf, FALSE);

	return node_string;
}
