#include "glib-yaml-parser.h"

#include <yaml.h>

#include "glib-yaml-document.h"
#include "glib-yaml-node.h"

#include <glib/gprintf.h>

GQuark
glib_yaml_parser_error_quark ()
{
	static GQuark parser_error_quark = 0;

	if (parser_error_quark == 0)
		parser_error_quark = g_quark_from_static_string ("glib-yaml-parser-error-quark");

	return parser_error_quark;
}

static gboolean parse_stream_rule   (GLibYAMLStream    *,                 yaml_parser_t *, yaml_event_t *, GError **);
static gboolean parse_document_rule (GLibYAMLDocument **,                 yaml_parser_t *, yaml_event_t *, GError **);
static gboolean parse_node_rule     (GLibYAMLDocument  *, GLibYAMLNode **, yaml_parser_t *, yaml_event_t *, GError **);
static gboolean parse_sequence_rule (GLibYAMLDocument  *, GLibYAMLNode **, yaml_parser_t *, yaml_event_t *, GError **);
static gboolean parse_mapping_rule  (GLibYAMLDocument  *, GLibYAMLNode **, yaml_parser_t *, yaml_event_t *, GError **);

static GLibYAMLNode *parse_alias_terminal  (GLibYAMLDocument *, yaml_event_t *);
static GLibYAMLNode *parse_scalar_terminal (yaml_event_t *);

static void get_next_event (yaml_parser_t *, yaml_event_t *);

static const gchar *yaml_event_type_to_string (yaml_event_type_t);

gboolean
glib_yaml_parser_parse_stream (GLibYAMLStream *stream, FILE *yaml_file_handle, GError **error)
{
	yaml_parser_t parser;
	yaml_event_t  event;

	gboolean success;


	yaml_parser_initialize     (& parser);
	yaml_parser_set_input_file (& parser, yaml_file_handle);

	success = parse_stream_rule (stream, & parser, & event, error);

	yaml_event_delete  (& event);
	yaml_parser_delete (& parser);

	return success;
}

static gboolean
parse_stream_rule (GLibYAMLStream *stream, yaml_parser_t *parser, yaml_event_t *event, GError **error)
{
	GLibYAMLDocument *document;


	yaml_parser_parse (parser, event);

	if (event->type != YAML_STREAM_START_EVENT) {
		g_set_error (
			error,
			GLIB_YAML_PARSER_ERROR_DOMAIN,
			GLIB_YAML_PARSER_ERROR,
			"Missing symbol (%s)",
			yaml_event_type_to_string (YAML_STREAM_START_EVENT));

		return FALSE;
	}

	stream->encoding = event->data.stream_start.encoding;

	while (TRUE) {
		get_next_event (parser, event);

		if (event->type == YAML_DOCUMENT_START_EVENT) {
			if (! parse_document_rule (& document, parser, event, error))
				return FALSE;

			stream->documents = g_list_append (stream->documents, document);
		}

		else if (event->type == YAML_STREAM_END_EVENT)
			break;

		else {
			g_set_error (
				error,
				GLIB_YAML_PARSER_ERROR_DOMAIN,
				GLIB_YAML_PARSER_ERROR,
				"Unexpected symbol (%s)",
				yaml_event_type_to_string (event->type));

			return FALSE;
		}
	}

	return TRUE;
}

static gboolean
parse_document_rule (GLibYAMLDocument **document, yaml_parser_t *parser, yaml_event_t *event, GError **error)
{
	* document = glib_yaml_document_new ();

	if (event->data.document_start.version_directive != NULL)
		glib_yaml_document_set_version_directive (
			* document,
			event->data.document_start.version_directive->major,
			event->data.document_start.version_directive->minor);

	get_next_event (parser, event);

	if (event->type == YAML_DOCUMENT_END_EVENT)
		return TRUE;

	if (! parse_node_rule (* document, & ((* document)->root), parser, event, error)) {
		g_object_unref (* document);

		return FALSE;
	}

	get_next_event (parser, event);

	if (event->type == YAML_DOCUMENT_END_EVENT)
		return TRUE;

	g_set_error (
		error,
		GLIB_YAML_PARSER_ERROR_DOMAIN,
		GLIB_YAML_PARSER_ERROR,
		"Document rule can have only one node");

	g_object_unref (* document);

	return FALSE;
}

static gboolean
parse_node_rule (GLibYAMLDocument *document, GLibYAMLNode **node, yaml_parser_t *parser, yaml_event_t *event, GError **error)
{
	gchar *anchor_key;


	anchor_key = NULL;

	if (event->type == YAML_ALIAS_EVENT)
		* node = parse_alias_terminal (document, event);

	else if (event->type == YAML_SCALAR_EVENT) {
		anchor_key = g_strdup ((gchar *) event->data.scalar.anchor);

		* node = parse_scalar_terminal (event);
	}

	else if (event->type == YAML_SEQUENCE_START_EVENT) {
		anchor_key = g_strdup ((gchar *) event->data.sequence_start.anchor);

		if (! parse_sequence_rule (document, node, parser, event, error))
			return FALSE;
	}

	else if (event->type == YAML_MAPPING_START_EVENT) {
		anchor_key = g_strdup ((gchar *) event->data.mapping_start.anchor);

		if (! parse_mapping_rule (document, node, parser, event, error))
			return FALSE;
	}

	else {
		g_set_error (
			error,
			GLIB_YAML_PARSER_ERROR_DOMAIN,
			GLIB_YAML_PARSER_ERROR,
			"Unexpected symbol (%s)",
			yaml_event_type_to_string (event->type));

		return FALSE;
	}

	if (anchor_key != NULL) {
		glib_yaml_document_add_anchor (document, anchor_key, * node);

		g_free (anchor_key);
	}

	return TRUE;
}

static gboolean
parse_sequence_rule (GLibYAMLDocument *document, GLibYAMLNode **sequence, yaml_parser_t *parser, yaml_event_t *event, GError **error)
{
	GLibYAMLNode *node;


	* sequence = GLIB_YAML_NODE (glib_yaml_sequence_node_new ());

	while (TRUE) {
		get_next_event (parser, event);

		if (event->type == YAML_SEQUENCE_END_EVENT)
			return TRUE;

		if (! parse_node_rule (document, & node, parser, event, error)) {
			g_object_unref (* sequence);

			return FALSE;
		}

		glib_yaml_sequence_node_append (GLIB_YAML_SEQUENCE_NODE (* sequence), node);
	}

	return TRUE;
}

static gboolean
parse_mapping_rule (GLibYAMLDocument *document, GLibYAMLNode **mapping, yaml_parser_t *parser, yaml_event_t *event, GError **error)
{
	GLibYAMLNode *key;
	GLibYAMLNode *value;


	* mapping = GLIB_YAML_NODE (glib_yaml_mapping_node_new ());

	while (TRUE) {
		get_next_event (parser, event);

		if (event->type == YAML_MAPPING_END_EVENT)
			break;

		if (! parse_node_rule (document, & key, parser, event, error)) {
			g_object_unref (* mapping);

			return FALSE;
		}

		get_next_event (parser, event);

		if (event->type == YAML_MAPPING_END_EVENT) {
			g_set_error (
				error,
				GLIB_YAML_PARSER_ERROR_DOMAIN,
				GLIB_YAML_PARSER_ERROR,
				"Mapping rule missing value node");

			g_object_unref (key);
			g_object_unref (* mapping);

			return FALSE;
		}

		if (! parse_node_rule (document, & value, parser, event, error)) {
			g_object_unref (key);
			g_object_unref (* mapping);

			return FALSE;
		}

		glib_yaml_mapping_node_add (GLIB_YAML_MAPPING_NODE (* mapping), key, value);
	}

	return TRUE;
}

static GLibYAMLNode *
parse_alias_terminal (GLibYAMLDocument *document, yaml_event_t *event)
{
	GLibYAMLNode *anchor;

	anchor = glib_yaml_document_get_anchor (document, (gchar *) event->data.alias.anchor);

	return GLIB_YAML_NODE (glib_yaml_alias_node_new (anchor));
}

static GLibYAMLNode *
parse_scalar_terminal (yaml_event_t *event)
{
	return GLIB_YAML_NODE (glib_yaml_scalar_node_new ((gchar *) event->data.scalar.value));
}

static void
get_next_event (yaml_parser_t *parser, yaml_event_t *event)
{
	yaml_event_delete (event);
	yaml_parser_parse (parser, event);
}

static const gchar *
yaml_event_type_to_string (yaml_event_type_t t)
{
	switch (t) {
		case YAML_NO_EVENT:
			return "YAML_NO_EVENT";

		case YAML_STREAM_START_EVENT:
			return "YAML_STREAM_START_EVENT";

		case YAML_STREAM_END_EVENT:
			return "YAML_STREAM_END_EVENT";

		case YAML_DOCUMENT_START_EVENT:
			return "YAML_DOCUMENT_START_EVENT";

		case YAML_DOCUMENT_END_EVENT:
			return "YAML_DOCUMENT_END_EVENT";

		case YAML_ALIAS_EVENT:
			return "YAML_ALIAS_EVENT";

		case YAML_SCALAR_EVENT:
			return "YAML_SCALAR_EVENT";

		case YAML_SEQUENCE_START_EVENT:
			return "YAML_SEQUENCE_START_EVENT";

		case YAML_SEQUENCE_END_EVENT:
			return "YAML_SEQUENCE_END_EVENT";

		case YAML_MAPPING_START_EVENT:
			return "YAML_MAPPING_START_EVENT";

		case YAML_MAPPING_END_EVENT:
			return "YAML_MAPPING_END_EVENT";
	}

	return "YAML_UNKNOWN_EVENT";
}
