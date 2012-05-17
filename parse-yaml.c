#include <yaml.h>
#include <glib.h>
#include <glib/gprintf.h>

static void parse_stream_rule   (yaml_parser_t *, yaml_event_t *);
static void parse_document_rule (yaml_parser_t *, yaml_event_t *);
static void parse_node_rule     (yaml_parser_t *, yaml_event_t *);
static void parse_sequence_rule (yaml_parser_t *, yaml_event_t *);
static void parse_mapping_rule  (yaml_parser_t *, yaml_event_t *);

static void parse_alias_terminal  (yaml_event_t *);
static void parse_scalar_terminal (yaml_event_t *);

static gboolean verify_symbol (yaml_event_t *, yaml_event_type_t);

static const gchar *yaml_event_type_to_string (yaml_event_type_t);

int
main (int argc, char **argv)
{
	yaml_parser_t parser;
	yaml_event_t  event;

	FILE *yaml_file;


	yaml_file = fopen (argv [1], "r");

	yaml_parser_initialize     (& parser);
	yaml_parser_set_input_file (& parser, yaml_file);

	parse_stream_rule (& parser, & event);

	return 0;
}

static void
parse_stream_rule (yaml_parser_t *parser, yaml_event_t *event)
{
	yaml_parser_parse (parser, event);
	verify_symbol (event, YAML_STREAM_START_EVENT);

	g_printf ("STREAM_START\n");

	while (TRUE) {
		yaml_parser_parse (parser, event);

		if (event->type == YAML_DOCUMENT_START_EVENT)
			parse_document_rule (parser, event);

		else if (event->type == YAML_STREAM_END_EVENT) {
			g_printf ("STREAM_END\n");

			return;
		}

		else
			g_error ("Parse error (parse_stream_rule [%s])\n", yaml_event_type_to_string (event->type));
	}
}

static void
parse_document_rule (yaml_parser_t *parser, yaml_event_t *event)
{
	g_printf ("DOCUMENT_START\n");

	yaml_parser_parse (parser, event);

	if (event->type == YAML_DOCUMENT_END_EVENT) {
		g_printf ("DOCUMENT_END\n");

		return;
	}

	parse_node_rule (parser, event);

	yaml_parser_parse (parser, event);

	if (event->type == YAML_DOCUMENT_END_EVENT) {
		g_printf ("DOCUMENT_END\n");

		return;
	}
	else
		g_error ("Parse error (parse_document_rule can only have one node)\n");
}

static void
parse_node_rule (yaml_parser_t *parser, yaml_event_t *event)
{
	if (event->type == YAML_ALIAS_EVENT)
		parse_alias_terminal (event);

	else if (event->type == YAML_SCALAR_EVENT)
		parse_scalar_terminal (event);

	else if (event->type == YAML_SEQUENCE_START_EVENT)
		parse_sequence_rule (parser, event);

	else if (event->type == YAML_MAPPING_START_EVENT)
		parse_mapping_rule (parser, event);

	else
		g_error ("Parse error (parse_node_rule [%s])\n", yaml_event_type_to_string (event->type));
}

static void
parse_sequence_rule (yaml_parser_t *parser, yaml_event_t *event)
{
	g_printf ("SEQUENCE_START\n");

	while (TRUE) {
		yaml_parser_parse (parser, event);

		if (event->type == YAML_SEQUENCE_END_EVENT) {
			g_printf ("SEQUENCE_END\n");

			return;
		}

		parse_node_rule (parser, event);
	}
}

static void
parse_mapping_rule (yaml_parser_t *parser, yaml_event_t *event)
{
	g_printf ("MAPPING_START\n");

	while (TRUE) {
		yaml_parser_parse (parser, event);

		if (event->type == YAML_MAPPING_END_EVENT) {
			g_printf ("MAPPING_END\n");

			return;
		}

		g_printf ("MAP_KEY\n");

		parse_node_rule (parser, event);

		yaml_parser_parse (parser, event);

		if (event->type == YAML_MAPPING_END_EVENT)
			g_error ("Parse error (parse_mapping_rule needs another node)\n");

		g_printf ("MAP_VALUE\n");

		parse_node_rule (parser, event);
	}
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

static gboolean
verify_symbol (yaml_event_t *event, yaml_event_type_t type)
{
	if (event->type != type) {
		g_error (
			"Parse error (got [%s], expected [%s])\n",
			yaml_event_type_to_string (event->type),
			yaml_event_type_to_string (type));

		return FALSE;
	}

	return TRUE;
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
