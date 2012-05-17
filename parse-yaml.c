#include <yaml.h>
#include <glib.h>
#include <glib/gprintf.h>

#define STREAM_RULE_PARSE_ERROR   1
#define DOCUMENT_RULE_PARSE_ERROR 2
#define NODE_RULE_PARSE_ERROR     3
#define MAPPING_RULE_PARSE_ERROR  4

static GQuark yaml_parser_error_domain;

static gboolean parse_stream_rule   (yaml_parser_t *, yaml_event_t *, GError **);
static gboolean parse_document_rule (yaml_parser_t *, yaml_event_t *, GError **);
static gboolean parse_node_rule     (yaml_parser_t *, yaml_event_t *, GError **);
static gboolean parse_sequence_rule (yaml_parser_t *, yaml_event_t *, GError **);
static gboolean parse_mapping_rule  (yaml_parser_t *, yaml_event_t *, GError **);

static void parse_alias_terminal  (yaml_event_t *);
static void parse_scalar_terminal (yaml_event_t *);

static void get_next_event (yaml_parser_t *, yaml_event_t *);

static const gchar *yaml_event_type_to_string (yaml_event_type_t);

int
main (int argc, char **argv)
{
	yaml_parser_t parser;
	yaml_event_t  event;

	FILE *yaml_file;

	GError *error;


	yaml_file = fopen (argv [1], "r");

	yaml_parser_initialize     (& parser);
	yaml_parser_set_input_file (& parser, yaml_file);

	yaml_parser_error_domain = g_quark_from_string ("YAML_PARSER_ERROR");
	error = NULL;

	if (! parse_stream_rule (& parser, & event, & error)) {
		g_fprintf (stderr, "Parse error (%d): %s\n", error->code, error->message);

		g_error_free (error);
	}

	yaml_event_delete  (& event);
	yaml_parser_delete (& parser);

	return 0;
}

static gboolean
parse_stream_rule (yaml_parser_t *parser, yaml_event_t *event, GError **error)
{
	get_next_event (parser, event);

	if (event->type != YAML_STREAM_START_EVENT) {
		g_set_error (
			error,
			yaml_parser_error_domain,
			STREAM_RULE_PARSE_ERROR, 
			"Missing symbol (%s)\n",
			yaml_event_type_to_string (YAML_STREAM_START_EVENT));

		return FALSE;
	}

	g_printf ("STREAM_START\n");

	while (TRUE) {
		get_next_event (parser, event);

		if (event->type == YAML_DOCUMENT_START_EVENT) {
			if (! parse_document_rule (parser, event, error))
				return FALSE;
		}

		else if (event->type == YAML_STREAM_END_EVENT) {
			g_printf ("STREAM_END\n");

			break;
		}

		else {
			g_set_error (
				error,
				yaml_parser_error_domain,
				STREAM_RULE_PARSE_ERROR, 
				"Unexpected symbol (%s)\n",
				yaml_event_type_to_string (event->type));

			return FALSE;
		}
	}

	return TRUE;
}

static gboolean
parse_document_rule (yaml_parser_t *parser, yaml_event_t *event, GError **error)
{
	g_printf ("DOCUMENT_START\n");

	get_next_event (parser, event);

	if (event->type == YAML_DOCUMENT_END_EVENT) {
		g_printf ("DOCUMENT_END\n");

		return TRUE;
	}

	if (! parse_node_rule (parser, event, error))
		return FALSE;

	get_next_event (parser, event);

	if (event->type == YAML_DOCUMENT_END_EVENT) {
		g_printf ("DOCUMENT_END\n");

		return TRUE;
	}

	g_set_error (
		error,
		yaml_parser_error_domain,
		DOCUMENT_RULE_PARSE_ERROR,
		"Document rule can have only one node\n");

	return FALSE;
}

static gboolean
parse_node_rule (yaml_parser_t *parser, yaml_event_t *event, GError **error)
{
	if (event->type == YAML_ALIAS_EVENT)
		parse_alias_terminal (event);

	else if (event->type == YAML_SCALAR_EVENT)
		parse_scalar_terminal (event);

	else if (event->type == YAML_SEQUENCE_START_EVENT) {
		if (! parse_sequence_rule (parser, event, error))
			return FALSE;
	}

	else if (event->type == YAML_MAPPING_START_EVENT) {
		if (! parse_mapping_rule (parser, event, error))
			return FALSE;
	}

	else {
		g_set_error (
			error,
			yaml_parser_error_domain,
			NODE_RULE_PARSE_ERROR,
			"Unexpected symbol (%s)\n",
			yaml_event_type_to_string (event->type));

		return FALSE;
	}

	return TRUE;
}

static gboolean
parse_sequence_rule (yaml_parser_t *parser, yaml_event_t *event, GError **error)
{
	g_printf ("SEQUENCE_START\n");

	while (TRUE) {
		get_next_event (parser, event);

		if (event->type == YAML_SEQUENCE_END_EVENT) {
			g_printf ("SEQUENCE_END\n");

			return TRUE;
		}

		if (! parse_node_rule (parser, event, error))
			return FALSE;
	}

	return TRUE;
}

static gboolean
parse_mapping_rule (yaml_parser_t *parser, yaml_event_t *event, GError **error)
{
	g_printf ("MAPPING_START\n");

	while (TRUE) {
		get_next_event (parser, event);

		if (event->type == YAML_MAPPING_END_EVENT) {
			g_printf ("MAPPING_END\n");

			break;
		}

		g_printf ("MAP_KEY\n");

		if (! parse_node_rule (parser, event, error))
			return FALSE;

		get_next_event (parser, event);

		if (event->type == YAML_MAPPING_END_EVENT) {
			g_set_error (
				error,
				yaml_parser_error_domain,
				MAPPING_RULE_PARSE_ERROR,
				"Mapping rule missing value node\n");

			return FALSE;
		}

		g_printf ("MAP_VALUE\n");

		if (! parse_node_rule (parser, event, error))
			return FALSE;
	}

	return TRUE;
}

static void
parse_alias_terminal (yaml_event_t *event)
{
	g_printf ("ALIAS [%s]\n", event->data.alias.anchor);
}

static void
parse_scalar_terminal (yaml_event_t *event)
{
	g_printf ("SCALAR [%s]\n", event->data.scalar.value);
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
