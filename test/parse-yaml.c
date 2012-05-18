#include <stdlib.h>
#include <glib.h>
#include <glib/gprintf.h>
#include <glib-yaml.h>

int
main (int argc, char **argv)
{
	GLibYAMLStream *stream;

	GError *error;
	gint    error_code;


	g_type_init ();

	error = NULL;

	stream = glib_yaml_stream_load_from_file_path (argv [1], & error);

	if (stream == NULL) {
		error_code = 13;

		if (error != NULL) {
			g_fprintf (stderr, "Parse error (%d): %s\n", error->code, error->message);

			error_code = error->code;

			g_error_free (error);
		}
		else
			g_fprintf (stderr, "Unknown error\n");

		exit (error_code);
	}

	glib_yaml_stream_dump_to_file_handle (stream, stdout);

	g_object_unref (stream);

	return 0;
}
