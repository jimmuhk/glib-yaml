#include "glib-yaml-node.h"

#include "glib-yaml-stream.h"

G_DEFINE_TYPE (GLibYAMLSequenceNode, glib_yaml_sequence_node, GLIB_YAML_NODE_TYPE)

static void   finalize  (GObject *);
static gchar *to_string (GLibYAMLNode *, guint);

GLibYAMLSequenceNode *
glib_yaml_sequence_node_new ()
{
	return g_object_new (GLIB_YAML_SEQUENCE_NODE_TYPE, NULL);
}

void
glib_yaml_sequence_node_append (GLibYAMLSequenceNode *this, GLibYAMLNode *node)
{
	if (this->list == NULL)
		this->list = g_ptr_array_new_with_free_func (g_object_unref);

	g_ptr_array_add (this->list, node);
}

static void
glib_yaml_sequence_node_class_init (GLibYAMLSequenceNodeClass *this_class)
{
	G_OBJECT_CLASS       (this_class)->finalize  = finalize;
	GLIB_YAML_NODE_CLASS (this_class)->to_string = to_string;
}

static void
glib_yaml_sequence_node_init (GLibYAMLSequenceNode *this)
{
	this->list = NULL;
}

static void
finalize (GObject *g_object)
{
	g_ptr_array_free (GLIB_YAML_SEQUENCE_NODE (g_object)->list, TRUE);

	G_OBJECT_CLASS (glib_yaml_sequence_node_parent_class)->finalize (g_object);
}

static gchar *
to_string (GLibYAMLNode *glib_yaml_node, guint indent_level)
{
	GLibYAMLSequenceNode *this = GLIB_YAML_SEQUENCE_NODE (glib_yaml_node);

	gchar *node_string;
	gchar *child_string;

	GString *buf;
	gchar   *indent_string;

	GLibYAMLNode *node;

	gint i;


	indent_string = glib_yaml_stream_get_indent_string (indent_level);

	buf = g_string_new (NULL);

	g_string_append_printf (buf, "%sSEQUENCE-START\n", indent_string);

	for (i = 0; i < this->list->len; ++ i) {
		node = GLIB_YAML_NODE (g_ptr_array_index (this->list, i));

		child_string = GLIB_YAML_NODE_GET_CLASS (node)->to_string (node, indent_level + 1);
		g_string_append (buf, child_string);
		g_free (child_string);
	}

	g_string_append_printf (buf, "%sSEQUENCE-END\n", indent_string);

	g_free (indent_string);

	node_string = g_string_free (buf, FALSE);

	return node_string;
}
