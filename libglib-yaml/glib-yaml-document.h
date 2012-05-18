#ifndef __GLIB_YAML_DOCUMENT_H__
#define __GLIB_YAML_DOCUMENT_H__

#include <stdio.h>
#include <yaml.h>
#include <glib-object.h>

#include "glib-yaml-node.h"

#define GLIB_YAML_DOCUMENT_TYPE                    (glib_yaml_document_get_type ())
#define GLIB_YAML_DOCUMENT(obj_instance)           (G_TYPE_CHECK_INSTANCE_CAST ((obj_instance), GLIB_YAML_DOCUMENT_TYPE, GLibYAMLDocument))
#define IS_GLIB_YAML_DOCUMENT(obj_instance)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj_instance), GLIB_YAML_DOCUMENT_TYPE))
#define GLIB_YAML_DOCUMENT_CLASS(obj_class)        (G_TYPE_CHECK_CLASS_CAST ((obj_class), GLIB_YAML_DOCUMENT_TYPE, GLibYAMLDocumentClass))
#define IS_GLIB_YAML_DOCUMENT_CLASS(obj_class)     (G_TYPE_CHECK_CLASS_TYPE ((obj_class), GLIB_YAML_DOCUMENT_TYPE))
#define GLIB_YAML_DOCUMENT_GET_CLASS(obj_instance) (G_TYPE_INSTANCE_GET_CLASS ((obj_instance), GLIB_YAML_DOCUMENT_TYPE, GLibYAMLDocumentClass))

typedef struct {
	GObjectClass g_object_class;
} GLibYAMLDocumentClass;

typedef struct {
	GObject g_object;

	struct {
		gint major_number;
		gint minor_number;
	} version_directive;

	GLibYAMLNode *root;
} GLibYAMLDocument;

GType glib_yaml_document_get_type (void);

GLibYAMLDocument *glib_yaml_document_new ();

void glib_yaml_document_set_version_directive (GLibYAMLDocument *document, gint major_number, gint minor_number);
void glib_yaml_document_dump_to_file_handle   (GLibYAMLDocument *document, FILE *handle, gint indent_level);

#endif
