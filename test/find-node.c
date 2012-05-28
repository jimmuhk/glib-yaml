#include <glib.h>
#include <glib/gprintf.h>
#include <glib-yaml.h>

int
main (int argc, char **argv)
{
	GLibYAMLStream   *stream;
	GLibYAMLDocument *doc;


	g_type_init ();

	stream = glib_yaml_stream_load_from_file_path (argv [1], NULL);
	doc    = GLIB_YAML_DOCUMENT (g_list_first (stream->documents)->data);

	glib_yaml_node_dump_to_file_handle (glib_yaml_node_path_get_node (doc->root, argv [2]), stdout, 0);

	g_object_unref (stream);

	return 0;
}
