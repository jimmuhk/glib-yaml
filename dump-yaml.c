#include <yaml.h>
#include <glib.h>
#include <glib/gprintf.h>

static void write_indent (gint);

int
main(int argc, char **argv)
{
	yaml_parser_t parser;
	yaml_token_t  token;

	FILE *yaml_file;

	gchar *encoding;
	gchar *scalar_style;

	gint level;


	yaml_file = fopen (argv [1], "r");

	yaml_parser_initialize     (& parser);
	yaml_parser_set_input_file (& parser, yaml_file);

	level = 0;

	do {
		yaml_parser_scan (& parser, & token);

		switch (token.type) { 
			case YAML_NO_TOKEN:
				write_indent (level);
				g_printf ("*no token*\n");
				break;

			case YAML_STREAM_START_TOKEN:
				switch (token.data.stream_start.encoding) {
					case YAML_ANY_ENCODING:
						encoding = "YAML_ANY_ENCODING";
						break;

					case YAML_UTF8_ENCODING:
						encoding = "YAML_UTF8_ENCODING";
						break;

					case YAML_UTF16LE_ENCODING:
						encoding = "YAML_UTF16LE_ENCODING";
						break;

					case YAML_UTF16BE_ENCODING:
						encoding = "YAML_UTF16BE_ENCODING";
						break;

					default:
						encoding = "unknown encoding";
						break;
				}

				write_indent (level ++);
				g_printf ("<stream encoding=\"%s\">\n", encoding);
				break;

			case YAML_STREAM_END_TOKEN:
				write_indent (-- level);
				g_printf ("</stream>\n");
				break;

			case YAML_VERSION_DIRECTIVE_TOKEN:
				write_indent (level);
				g_printf ("<version>%d.%d</version>\n", token.data.version_directive.major, token.data.version_directive.minor);
				break;

			case YAML_TAG_DIRECTIVE_TOKEN:
				write_indent (level);
				g_printf ("<tag handle=\"%s\" prefix=\"%s\"/>\n", token.data.tag_directive.handle, token.data.tag_directive.prefix);
				break;

			case YAML_DOCUMENT_START_TOKEN:
				write_indent (level ++);
				g_printf ("<document>\n");
				break;

			case YAML_DOCUMENT_END_TOKEN:
				write_indent (-- level);
				g_printf ("</document>\n");
				break;

			case YAML_BLOCK_SEQUENCE_START_TOKEN:
				write_indent (level ++);
				g_printf ("<block:sequence>\n");
				break;

			case YAML_BLOCK_MAPPING_START_TOKEN:
				write_indent (level ++);
				g_printf ("<block:mapping>\n");
				break;

			case YAML_BLOCK_END_TOKEN:
				write_indent (-- level);
				g_printf ("</block>\n");
				break;

			case YAML_FLOW_SEQUENCE_START_TOKEN:
				write_indent (level ++);
				g_printf ("<flow-sequence>\n");
				break;

			case YAML_FLOW_SEQUENCE_END_TOKEN:
				write_indent (-- level);
				g_printf ("</flow-sequence>\n");
				break;

			case YAML_FLOW_MAPPING_START_TOKEN:
				write_indent (level ++);
				g_printf ("<map-sequence>\n");
				break;

			case YAML_FLOW_MAPPING_END_TOKEN:
				write_indent (-- level);
				g_printf ("</map-sequence>\n");
				break;

			case YAML_BLOCK_ENTRY_TOKEN:
				write_indent (level);
				g_printf ("<block-entry/>\n");
				break;

			case YAML_FLOW_ENTRY_TOKEN:
				write_indent (level);
				g_printf ("<flow-entry/>\n");
				break;

			case YAML_KEY_TOKEN:
				write_indent (level);
				g_printf ("<key/>\n");
				break;

			case YAML_VALUE_TOKEN:
				write_indent (level);
				g_printf ("<value/>\n");
				break;

			case YAML_ALIAS_TOKEN:
				write_indent (level);
				g_printf ("<alias>%s</alias>\n", token.data.alias.value);
				break;

			case YAML_ANCHOR_TOKEN:
				write_indent (level);
				g_printf ("<anchor>%s</anchor>\n", token.data.anchor.value);
				break;

			case YAML_TAG_TOKEN:
				write_indent (level);
				g_printf ("<tag handle=\"%s\" suffix=\"%s\"/>\n", token.data.tag.handle, token.data.tag.suffix);
				break;

			case YAML_SCALAR_TOKEN:
				switch (token.data.scalar.style) {
					case YAML_ANY_SCALAR_STYLE:
						scalar_style = "YAML_ANY_SCALAR_STYLE";
						break;

					case YAML_PLAIN_SCALAR_STYLE:
						scalar_style = "YAML_PLAIN_SCALAR_STYLE";
						break;

					case YAML_SINGLE_QUOTED_SCALAR_STYLE:
						scalar_style = "YAML_SINGLE_QUOTED_SCALAR_STYLE";
						break;

					case YAML_DOUBLE_QUOTED_SCALAR_STYLE:
						scalar_style = "YAML_DOUBLE_QUOTED_SCALAR_STYLE";
						break;

					case YAML_LITERAL_SCALAR_STYLE:
						scalar_style = "YAML_LITERAL_SCALAR_STYLE";
						break;

					case YAML_FOLDED_SCALAR_STYLE:
						scalar_style = "YAML_FOLDED_SCALAR_STYLE";
						break;

					default:
						scalar_style = "unknown scalar style";
						break;
				}

				write_indent (level);
				g_printf ("<scalar size=\"%zu\" style=\"%s\">%s</scalar>\n", token.data.scalar.length, scalar_style, token.data.scalar.value);
				break;
		}

		if (token.type != YAML_STREAM_END_TOKEN)
			yaml_token_delete (& token);

	} while (token.type != YAML_STREAM_END_TOKEN);

	yaml_token_delete  (& token);
	yaml_parser_delete (& parser);

	fclose (yaml_file);

	return 0;
}

static void
write_indent (gint level)
{
	gint i;

	for (i = 0; i < level; ++i)
		g_printf ("\t");
}
