AC_INIT([libglib-yaml], [1.0.0], [])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

AC_PROG_CC
AC_PROG_LIBTOOL

PKG_CHECK_MODULES(LIBGLIB_YAML, [ "glib-2.0 gobject-2.0 yaml-0.1" ])
AC_SUBST(LIBGLIB_YAML_CFLAGS)
AC_SUBST(LIBGLIB_YAML_LIBS)

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([
	Makefile
	libglib-yaml/Makefile
	pkg-config/Makefile
	pkg-config/libglib-yaml.pc])
AC_OUTPUT
