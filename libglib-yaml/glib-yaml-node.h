#ifndef __GLIB_YAML_NODE_H__
#define __GLIB_YAML_NODE_H__

#include <stdio.h>
#include <glib-object.h>

#define GLIB_YAML_NODE_TYPE                    (glib_yaml_node_get_type ())
#define GLIB_YAML_NODE(obj_instance)           (G_TYPE_CHECK_INSTANCE_CAST ((obj_instance), GLIB_YAML_NODE_TYPE, GLibYAMLNode))
#define IS_GLIB_YAML_NODE(obj_instance)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj_instance), GLIB_YAML_NODE_TYPE))
#define GLIB_YAML_NODE_CLASS(obj_class)        (G_TYPE_CHECK_CLASS_CAST ((obj_class), GLIB_YAML_NODE_TYPE, GLibYAMLNodeClass))
#define IS_GLIB_YAML_NODE_CLASS(obj_class)     (G_TYPE_CHECK_CLASS_TYPE ((obj_class), GLIB_YAML_NODE_TYPE))
#define GLIB_YAML_NODE_GET_CLASS(obj_instance) (G_TYPE_INSTANCE_GET_CLASS ((obj_instance), GLIB_YAML_NODE_TYPE, GLibYAMLNodeClass))

typedef struct {
	GObjectClass g_object_class;
} GLibYAMLNodeClass;

typedef enum {
	GLIB_YAML_ALIAS_NODE,
	GLIB_YAML_SCALAR_NODE,
	GLIB_YAML_SEQUENCE_NODE,
	GLIB_YAML_MAPPING_NODE
} GLibYAMLNodeType;

typedef struct GLibYAMLNode_s GLibYAMLNode;

struct GLibYAMLNode_s {
	GObject g_object;

	GLibYAMLNodeType type;

	union {
		GLibYAMLNode *alias;
		gchar        *scalar;
		GPtrArray    *sequence;
		GHashTable   *mapping;
	} data;
};

GType glib_yaml_node_get_type (void);

GLibYAMLNode *glib_yaml_node_new (void);

void glib_yaml_node_assign_as_alias    (GLibYAMLNode *node, GLibYAMLNode *anchor_node);
void glib_yaml_node_assign_as_scalar   (GLibYAMLNode *node, const gchar *scalar);
void glib_yaml_node_assign_as_sequence (GLibYAMLNode *node);
void glib_yaml_node_assign_as_mapping  (GLibYAMLNode *node);

void glib_yaml_node_add_sequence_element (GLibYAMLNode *sequence, GLibYAMLNode *node);
void glib_yaml_node_add_mapping_element  (GLibYAMLNode *mapping,  GLibYAMLNode *key, GLibYAMLNode *value);

void glib_yaml_node_dump_to_file_handle (GLibYAMLNode *node, FILE *file_handle, gint indent_level);

guint    glib_yaml_node_hash  (gconstpointer key);
gboolean glib_yaml_node_equal (gconstpointer a, gconstpointer b);

#endif
