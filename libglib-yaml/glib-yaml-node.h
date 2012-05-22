#ifndef __GLIB_YAML_NODE_H__
#define __GLIB_YAML_NODE_H__

#include <stdio.h>
#include <glib-object.h>

/* GLibYAMLNode */

#define GLIB_YAML_NODE_TYPE                    (glib_yaml_node_get_type ())
#define GLIB_YAML_NODE(obj_instance)           (G_TYPE_CHECK_INSTANCE_CAST ((obj_instance), GLIB_YAML_NODE_TYPE, GLibYAMLNode))
#define IS_GLIB_YAML_NODE(obj_instance)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj_instance), GLIB_YAML_NODE_TYPE))
#define GLIB_YAML_NODE_CLASS(obj_class)        (G_TYPE_CHECK_CLASS_CAST ((obj_class), GLIB_YAML_NODE_TYPE, GLibYAMLNodeClass))
#define IS_GLIB_YAML_NODE_CLASS(obj_class)     (G_TYPE_CHECK_CLASS_TYPE ((obj_class), GLIB_YAML_NODE_TYPE))
#define GLIB_YAML_NODE_GET_CLASS(obj_instance) (G_TYPE_INSTANCE_GET_CLASS ((obj_instance), GLIB_YAML_NODE_TYPE, GLibYAMLNodeClass))

typedef struct GLibYAMLNode_s GLibYAMLNode;

typedef struct {
	GObjectClass g_object_class;

	gchar *(* to_string) (GLibYAMLNode *, guint);
} GLibYAMLNodeClass;

struct GLibYAMLNode_s {
	GObject g_object;
};

GType glib_yaml_node_get_type (void);

void glib_yaml_node_dump_to_file_handle (GLibYAMLNode *node, FILE *file_handle, gint indent_level);

guint    glib_yaml_node_hash  (gconstpointer key);
gboolean glib_yaml_node_equal (gconstpointer a, gconstpointer b);

/* GLibYAMLAliasNode */

#define GLIB_YAML_ALIAS_NODE_TYPE                    (glib_yaml_alias_node_get_type ())
#define GLIB_YAML_ALIAS_NODE(obj_instance)           (G_TYPE_CHECK_INSTANCE_CAST ((obj_instance), GLIB_YAML_ALIAS_NODE_TYPE, GLibYAMLAliasNode))
#define IS_GLIB_YAML_ALIAS_NODE(obj_instance)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj_instance), GLIB_YAML_ALIAS_NODE_TYPE))
#define GLIB_YAML_ALIAS_NODE_CLASS(obj_class)        (G_TYPE_CHECK_CLASS_CAST ((obj_class), GLIB_YAML_ALIAS_NODE_TYPE, GLibYAMLAliasNodeClass))
#define IS_GLIB_YAML_ALIAS_NODE_CLASS(obj_class)     (G_TYPE_CHECK_CLASS_TYPE ((obj_class), GLIB_YAML_ALIAS_NODE_TYPE))
#define GLIB_YAML_ALIAS_NODE_GET_CLASS(obj_instance) (G_TYPE_INSTANCE_GET_CLASS ((obj_instance), GLIB_YAML_ALIAS_NODE_TYPE, GLibYAMLAliasNodeClass))

typedef struct {
	GLibYAMLNodeClass glib_yaml_node_class;
} GLibYAMLAliasNodeClass;

typedef struct {
	GLibYAMLNode glib_yaml_node;

	GLibYAMLNode *anchor;
} GLibYAMLAliasNode;

GType glib_yaml_alias_node_get_type (void);

GLibYAMLAliasNode *glib_yaml_alias_node_new (GLibYAMLNode *anchor_node);

/* GLibYAMLScalarNode */

#define GLIB_YAML_SCALAR_NODE_TYPE                    (glib_yaml_scalar_node_get_type ())
#define GLIB_YAML_SCALAR_NODE(obj_instance)           (G_TYPE_CHECK_INSTANCE_CAST ((obj_instance), GLIB_YAML_SCALAR_NODE_TYPE, GLibYAMLScalarNode))
#define IS_GLIB_YAML_SCALAR_NODE(obj_instance)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj_instance), GLIB_YAML_SCALAR_NODE_TYPE))
#define GLIB_YAML_SCALAR_NODE_CLASS(obj_class)        (G_TYPE_CHECK_CLASS_CAST ((obj_class), GLIB_YAML_SCALAR_NODE_TYPE, GLibYAMLScalarNodeClass))
#define IS_GLIB_YAML_SCALAR_NODE_CLASS(obj_class)     (G_TYPE_CHECK_CLASS_TYPE ((obj_class), GLIB_YAML_SCALAR_NODE_TYPE))
#define GLIB_YAML_SCALAR_NODE_GET_CLASS(obj_instance) (G_TYPE_INSTANCE_GET_CLASS ((obj_instance), GLIB_YAML_SCALAR_NODE_TYPE, GLibYAMLScalarNodeClass))

typedef struct {
	GLibYAMLNodeClass glib_yaml_node_class;
} GLibYAMLScalarNodeClass;

typedef struct {
	GLibYAMLNode glib_yaml_node;

	gchar *value;
} GLibYAMLScalarNode;

GType glib_yaml_scalar_node_get_type (void);

GLibYAMLScalarNode *glib_yaml_scalar_node_new (const gchar *value);

gboolean  glib_yaml_scalar_node_get_boolean (GLibYAMLScalarNode *scalar);
guint     glib_yaml_scalar_node_get_uint    (GLibYAMLScalarNode *scalar);
gint      glib_yaml_scalar_node_get_int     (GLibYAMLScalarNode *scalar);
gdouble   glib_yaml_scalar_node_get_double  (GLibYAMLScalarNode *scalar);
gchar    *glib_yaml_scalar_node_get_string  (GLibYAMLScalarNode *scalar);

/* GLibYAMLSequenceNode */

#define GLIB_YAML_SEQUENCE_NODE_TYPE                    (glib_yaml_sequence_node_get_type ())
#define GLIB_YAML_SEQUENCE_NODE(obj_instance)           (G_TYPE_CHECK_INSTANCE_CAST ((obj_instance), GLIB_YAML_SEQUENCE_NODE_TYPE, GLibYAMLSequenceNode))
#define IS_GLIB_YAML_SEQUENCE_NODE(obj_instance)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj_instance), GLIB_YAML_SEQUENCE_NODE_TYPE))
#define GLIB_YAML_SEQUENCE_NODE_CLASS(obj_class)        (G_TYPE_CHECK_CLASS_CAST ((obj_class), GLIB_YAML_SEQUENCE_NODE_TYPE, GLibYAMLSequenceNodeClass))
#define IS_GLIB_YAML_SEQUENCE_NODE_CLASS(obj_class)     (G_TYPE_CHECK_CLASS_TYPE ((obj_class), GLIB_YAML_SEQUENCE_NODE_TYPE))
#define GLIB_YAML_SEQUENCE_NODE_GET_CLASS(obj_instance) (G_TYPE_INSTANCE_GET_CLASS ((obj_instance), GLIB_YAML_SEQUENCE_NODE_TYPE, GLibYAMLSequenceNodeClass))

typedef struct {
	GLibYAMLNodeClass glib_yaml_node_class;
} GLibYAMLSequenceNodeClass;

typedef struct {
	GLibYAMLNode glib_yaml_node;

	GPtrArray *nodes;
} GLibYAMLSequenceNode;

GType glib_yaml_sequence_node_get_type (void);

GLibYAMLSequenceNode *glib_yaml_sequence_node_new (void);

void glib_yaml_sequence_node_append (GLibYAMLSequenceNode *sequence, GLibYAMLNode *node);

/* GLibYAMLMappingNode */

#define GLIB_YAML_MAPPING_NODE_TYPE                    (glib_yaml_mapping_node_get_type ())
#define GLIB_YAML_MAPPING_NODE(obj_instance)           (G_TYPE_CHECK_INSTANCE_CAST ((obj_instance), GLIB_YAML_MAPPING_NODE_TYPE, GLibYAMLMappingNode))
#define IS_GLIB_YAML_MAPPING_NODE(obj_instance)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj_instance), GLIB_YAML_MAPPING_NODE_TYPE))
#define GLIB_YAML_MAPPING_NODE_CLASS(obj_class)        (G_TYPE_CHECK_CLASS_CAST ((obj_class), GLIB_YAML_MAPPING_NODE_TYPE, GLibYAMLMappingNodeClass))
#define IS_GLIB_YAML_MAPPING_NODE_CLASS(obj_class)     (G_TYPE_CHECK_CLASS_TYPE ((obj_class), GLIB_YAML_MAPPING_NODE_TYPE))
#define GLIB_YAML_MAPPING_NODE_GET_CLASS(obj_instance) (G_TYPE_INSTANCE_GET_CLASS ((obj_instance), GLIB_YAML_MAPPING_NODE_TYPE, GLibYAMLMappingNodeClass))

typedef struct {
	GLibYAMLNodeClass glib_yaml_node_class;
} GLibYAMLMappingNodeClass;

typedef struct {
	GLibYAMLNode glib_yaml_node;

	GHashTable *nodes;
} GLibYAMLMappingNode;

GType glib_yaml_mapping_node_get_type (void);

GLibYAMLMappingNode *glib_yaml_mapping_node_new (void);

void          glib_yaml_mapping_node_add            (GLibYAMLMappingNode *mapping, GLibYAMLNode *key, GLibYAMLNode *value);
GLibYAMLNode *glib_yaml_mapping_node_find_by_scalar (GLibYAMLMappingNode *mapping, const gchar *key);

gboolean  glib_yaml_mapping_node_get_boolean (GLibYAMLMappingNode *mapping, const gchar *key);
guint     glib_yaml_mapping_node_get_uint    (GLibYAMLMappingNode *mapping, const gchar *key);
gint      glib_yaml_mapping_node_get_int     (GLibYAMLMappingNode *mapping, const gchar *key);
gdouble   glib_yaml_mapping_node_get_double  (GLibYAMLMappingNode *mapping, const gchar *key);
gchar    *glib_yaml_mapping_node_get_string  (GLibYAMLMappingNode *mapping, const gchar *key);

#endif
