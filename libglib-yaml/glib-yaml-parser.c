#include "glib-yaml-parser.h"

#include <yaml.h>

#include "glib-yaml-document.h"
#include "glib-yaml-node.h"

GQuark
glib_yaml_parser_error_quark ()
{
	static GQuark parser_error_quark = 0;

	if (parser_error_quark == 0)
		parser_error_quark = g_quark_from_static_string ("glib-yaml-parser-error-quark");

	return parser_error_quark;
}

static gboolean parse_stream_rule   (GLibYAMLStream   *,                 yaml_parser_t *, yaml_event_t *, GError **);
static gboolean parse_document_rule (GLibYAMLDocument *,                 yaml_parser_t *, yaml_event_t *, GError **);
static gboolean parse_node_rule     (GLibYAMLDocument *, GLibYAMLNode *, yaml_parser_t *, yaml_event_t *, GError **);
static gboolean parse_sequence_rule (GLibYAMLDocument *, GLibYAMLNode *, yaml_parser_t *, yaml_event_t *, GError **);
static gboolean parse_mapping_rule  (GLibYAMLDocument *, GLibYAMLNode *, yaml_parser_t *, yaml_event_t *, GError **);

static void parse_alias_terminal  (GLibYAMLDocument *, GLibYAMLNode *, yaml_event_t *);
static void parse_scalar_terminal (GLibYAMLNode *, yaml_event_t *);

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
			document = glib_yaml_document_new ();

			if (! parse_document_rule (document, parser, event, error)) {
				g_object_unref (document);

				return FALSE;
			}

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
parse_document_rule (GLibYAMLDocument *document, yaml_parser_t *parser, yaml_event_t *event, GError **error)
{
	if (event->data.document_start.version_directive != NULL)
		glib_yaml_document_set_version_directive (
			document,
			event->data.document_start.version_directive->major,
			event->data.document_start.version_directive->minor);

	get_next_event (parser, event);

	if (event->type == YAML_DOCUMENT_END_EVENT)
		return TRUE;

	if (! parse_node_rule (document, document->root, parser, event, error))
		return FALSE;

	get_next_event (parser, event);

	if (event->type == YAML_DOCUMENT_END_EVENT)
		return TRUE;

	g_set_error (
		error,
		GLIB_YAML_PARSER_ERROR_DOMAIN,
		GLIB_YAML_PARSER_ERROR,
		"Document rule can have only one node");

	return FALSE;
}

static gboolean
parse_node_rule (GLibYAMLDocument *document, GLibYAMLNode *node, yaml_parser_t *parser, yaml_event_t *event, GError **error)
{
	gchar *anchor_key;


	if (event->type == YAML_ALIAS_EVENT)
		parse_alias_terminal (document, node, event);

	else if (event->type == YAML_SCALAR_EVENT) {
		parse_scalar_terminal (node, event);

		if ((anchor_key = (gchar *) (event->data.scalar.anchor)) != NULL)
			glib_yaml_document_add_anchor (document, anchor_key, node);
	}

	else if (event->type == YAML_SEQUENCE_START_EVENT) {
		if (! parse_sequence_rule (document, node, parser, event, error))
			return FALSE;

		if ((anchor_key = (gchar *) (event->data.sequence_start.anchor)) != NULL)
			glib_yaml_document_add_anchor (document, anchor_key, node);
	}

	else if (event->type == YAML_MAPPING_START_EVENT) {
		if (! parse_mapping_rule (document, node, parser, event, error))
			return FALSE;

		if ((anchor_key = (gchar *) (event->data.mapping_start.anchor)) != NULL)
			glib_yaml_document_add_anchor (document, anchor_key, node);
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

	return TRUE;
}

static gboolean
parse_sequence_rule (GLibYAMLDocument *document, GLibYAMLNode *sequence, yaml_parser_t *parser, yaml_event_t *event, GError **error)
{
	GLibYAMLNode *node;


	glib_yaml_node_assign_as_sequence (sequence);

	while (TRUE) {
		get_next_event (parser, event);

		if (event->type == YAML_SEQUENCE_END_EVENT)
			return TRUE;

		node = glib_yaml_node_new ();

		if (! parse_node_rule (document, node, parser, event, error)) {
			g_object_unref (node);

			return FALSE;
		}

		glib_yaml_node_add_sequence_element (sequence, node);
	}

	return TRUE;
}

static gboolean
parse_mapping_rule (GLibYAMLDocument *document, GLibYAMLNode *mapping, yaml_parser_t *parser, yaml_event_t *event, GError **error)
{
	GLibYAMLNode *key;
	GLibYAMLNode *value;


	glib_yaml_node_assign_as_mapping (mapping);

	while (TRUE) {
		get_next_event (parser, event);

		if (event->type == YAML_MAPPING_END_EVENT)
			break;

		key = glib_yaml_node_new ();

		if (! parse_node_rule (document, key, parser, event, error)) {
			g_object_unref (key);

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

			return FALSE;
		}

		value = glib_yaml_node_new ();

		if (! parse_node_rule (document, value, parser, event, error)) {
			g_object_unref (key);
			g_object_unref (value);

			return FALSE;
		}

		glib_yaml_node_add_mapping_element (mapping, key, value);
	}

	return TRUE;
}

static void
parse_alias_terminal (GLibYAMLDocument *document, GLibYAMLNode *node, yaml_event_t *event)
{
	glib_yaml_node_assign_as_alias (
		node,
		glib_yaml_document_get_anchor (
			document, (gchar *) event->data.alias.anchor));
}

static void
parse_scalar_terminal (GLibYAMLNode *node, yaml_event_t *event)
{
	glib_yaml_node_assign_as_scalar (node, (gchar *) event->data.scalar.value);
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
