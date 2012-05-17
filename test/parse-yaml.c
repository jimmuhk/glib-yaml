#include <yaml.h>
#include <glib.h>
#include <glib/gprintf.h>

#define STREAM_RULE_PARSE_ERROR   1
#define DOCUMENT_RULE_PARSE_ERROR 2
#define NODE_RULE_PARSE_ERROR     3
#define MAPPING_RULE_PARSE_ERROR  4

static GQuark yaml_parser_error_domain;

typedef struct {
	GList *documents;
} GLibYAMLStream;

typedef enum {
	GLIB_YAML_ALIAS_NODE,
	GLIB_YAML_SCALAR_NODE,
	GLIB_YAML_SEQUENCE_NODE,
	GLIB_YAML_MAPPING_NODE
} GLibYAMLNodeType;

typedef struct {
	GLibYAMLNodeType type;

	union {
		gchar      *alias;
		gchar      *scalar;
		GPtrArray  *sequence;
		GHashTable *mapping;
	} data;
} GLibYAMLNode;

static void dump_glib_yaml_node (GLibYAMLNode *, gint);

static gboolean parse_stream_rule   (GLibYAMLStream **, yaml_parser_t *, yaml_event_t *, GError **);
static gboolean parse_document_rule (GLibYAMLNode   **, yaml_parser_t *, yaml_event_t *, GError **);
static gboolean parse_node_rule     (GLibYAMLNode   **, yaml_parser_t *, yaml_event_t *, GError **);
static gboolean parse_sequence_rule (GLibYAMLNode   **, yaml_parser_t *, yaml_event_t *, GError **);
static gboolean parse_mapping_rule  (GLibYAMLNode   **, yaml_parser_t *, yaml_event_t *, GError **);

static void parse_alias_terminal  (GLibYAMLNode **, yaml_event_t *);
static void parse_scalar_terminal (GLibYAMLNode **, yaml_event_t *);

static void get_next_event (yaml_parser_t *, yaml_event_t *);

static const gchar *yaml_event_type_to_string (yaml_event_type_t);

int
main (int argc, char **argv)
{
	yaml_parser_t parser;
	yaml_event_t  event;

	GLibYAMLStream *stream;

	FILE *yaml_file;

	GList *document;

	GError *error;


	yaml_file = fopen (argv [1], "r");

	yaml_parser_initialize     (& parser);
	yaml_parser_set_input_file (& parser, yaml_file);

	stream            = g_new (GLibYAMLStream, 1);
	stream->documents = NULL;

	yaml_parser_error_domain = g_quark_from_string ("YAML_PARSER_ERROR");
	error                    = NULL;

	if (! parse_stream_rule (& stream, & parser, & event, & error)) {
		g_fprintf (stderr, "Parse error (%d): %s\n", error->code, error->message);

		g_error_free (error);
	}

	for (document = g_list_first (stream->documents); document; document = g_list_next (document)) {
		g_printf ("DOCUMENT-START\n");

		dump_glib_yaml_node ((GLibYAMLNode *) (document->data), 1);

		g_printf ("DOCUMENT-END\n");
	}

	yaml_event_delete  (& event);
	yaml_parser_delete (& parser);

	return 0;
}

static void
dump_glib_yaml_node (GLibYAMLNode *node, gint level)
{
	GHashTableIter iter;
	gpointer       key, value;

	gint i;


	for (i = 0; i < level; ++ i)
		g_printf ("    ");

	switch (node->type) {
		case GLIB_YAML_ALIAS_NODE:
			g_printf ("ALIAS [%s]\n", node->data.alias);
			break;

		case GLIB_YAML_SCALAR_NODE:
			g_printf ("SCALAR [%s]\n", node->data.scalar);
			break;

		case GLIB_YAML_SEQUENCE_NODE:
			g_printf ("SEQUENCE-START\n");

			for (i = 0; i < node->data.sequence->len; ++ i)
				dump_glib_yaml_node ((GLibYAMLNode *) g_ptr_array_index (node->data.sequence, i), level + 1);

			for (i = 0; i < level; ++ i)
				g_printf ("    ");
			g_printf ("SEQUENCE-END\n");

			break;

		case GLIB_YAML_MAPPING_NODE:
			g_printf ("MAPPING-START\n");

			g_hash_table_iter_init (& iter, node->data.mapping);

			while (g_hash_table_iter_next (& iter, & key, & value)) {
				for (i = 0; i <= level; ++ i)
					g_printf ("    ");
				g_printf ("KEY\n");

				dump_glib_yaml_node ((GLibYAMLNode *) key, level + 2);

				for (i = 0; i <= level; ++ i)
					g_printf ("    ");
				g_printf ("VALUE\n");

				dump_glib_yaml_node ((GLibYAMLNode *) value, level + 2);
			}

			for (i = 0; i < level; ++ i)
				g_printf ("    ");
			g_printf ("MAPPING-END\n");

			break;
	}
}

static gboolean
parse_stream_rule (GLibYAMLStream **stream, yaml_parser_t *parser, yaml_event_t *event, GError **error)
{
	GLibYAMLNode *document;


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

	while (TRUE) {
		get_next_event (parser, event);

		if (event->type == YAML_DOCUMENT_START_EVENT) {
			document = g_new (GLibYAMLNode, 1);

			if (! parse_document_rule (& document, parser, event, error)) {
				g_free (document);

				return FALSE;
			}

			(* stream)->documents = g_list_append ((* stream)->documents, document);
		}

		else if (event->type == YAML_STREAM_END_EVENT)
			break;

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
parse_document_rule (GLibYAMLNode **node, yaml_parser_t *parser, yaml_event_t *event, GError **error)
{
	get_next_event (parser, event);

	if (event->type == YAML_DOCUMENT_END_EVENT)
		return TRUE;

	if (! parse_node_rule (node, parser, event, error))
		return FALSE;

	get_next_event (parser, event);

	if (event->type == YAML_DOCUMENT_END_EVENT)
		return TRUE;

	g_set_error (
		error,
		yaml_parser_error_domain,
		DOCUMENT_RULE_PARSE_ERROR,
		"Document rule can have only one node\n");

	return FALSE;
}

static gboolean
parse_node_rule (GLibYAMLNode **node, yaml_parser_t *parser, yaml_event_t *event, GError **error)
{
	if (event->type == YAML_ALIAS_EVENT)
		parse_alias_terminal (node, event);

	else if (event->type == YAML_SCALAR_EVENT)
		parse_scalar_terminal (node, event);

	else if (event->type == YAML_SEQUENCE_START_EVENT) {
		if (! parse_sequence_rule (node, parser, event, error))
			return FALSE;
	}

	else if (event->type == YAML_MAPPING_START_EVENT) {
		if (! parse_mapping_rule (node, parser, event, error))
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
parse_sequence_rule (GLibYAMLNode **sequence, yaml_parser_t *parser, yaml_event_t *event, GError **error)
{
	GLibYAMLNode *node;


	(* sequence)->type          = GLIB_YAML_SEQUENCE_NODE;
	(* sequence)->data.sequence = g_ptr_array_new ();

	while (TRUE) {
		get_next_event (parser, event);

		if (event->type == YAML_SEQUENCE_END_EVENT)
			return TRUE;

		node = g_new (GLibYAMLNode, 1);

		if (! parse_node_rule (& node, parser, event, error)) {
			g_free (node);

			return FALSE;
		}

		g_ptr_array_add ((* sequence)->data.sequence, node);
	}

	return TRUE;
}

static gboolean
parse_mapping_rule (GLibYAMLNode **mapping, yaml_parser_t *parser, yaml_event_t *event, GError **error)
{
	GLibYAMLNode *key;
	GLibYAMLNode *value;


	(* mapping)->type         = GLIB_YAML_MAPPING_NODE;
	(* mapping)->data.mapping = g_hash_table_new (g_direct_hash, g_direct_equal);

	while (TRUE) {
		get_next_event (parser, event);

		if (event->type == YAML_MAPPING_END_EVENT)
			break;

		key = g_new (GLibYAMLNode, 1);

		if (! parse_node_rule (& key, parser, event, error)) {
			g_free (key);

			return FALSE;
		}

		get_next_event (parser, event);

		if (event->type == YAML_MAPPING_END_EVENT) {
			g_set_error (
				error,
				yaml_parser_error_domain,
				MAPPING_RULE_PARSE_ERROR,
				"Mapping rule missing value node\n");

			return FALSE;
		}

		value = g_new (GLibYAMLNode, 1);

		if (! parse_node_rule (& value, parser, event, error)) {
			g_free (value);

			return FALSE;
		}

		g_hash_table_insert ((* mapping)->data.mapping, key, value);
	}

	return TRUE;
}

static void
parse_alias_terminal (GLibYAMLNode **node, yaml_event_t *event)
{
	(* node)->type       = GLIB_YAML_ALIAS_NODE;
	(* node)->data.alias = g_strdup ((const gchar *) event->data.alias.anchor);
}

static void
parse_scalar_terminal (GLibYAMLNode **node, yaml_event_t *event)
{
	(* node)->type        = GLIB_YAML_SCALAR_NODE;
	(* node)->data.scalar = g_strdup ((const gchar *) event->data.scalar.value);
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
