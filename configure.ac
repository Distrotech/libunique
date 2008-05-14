# versioning
m4_define([unique_major_version], [0])
m4_define([unique_minor_version], [9])
m4_define([unique_micro_version], [5])
m4_define([unique_version],
          [unique_major_version.unique_minor_version.unique_micro_version])
m4_define([unique_api_version],
          [unique_major_version.unique_minor_version])

# if functions have been added, set to 0; otherwise, increment
# with every release
m4_define([unique_interface_age], [0])
m4_define([unique_binary_age],
          [m4_eval(100 * unique_minor_version + unique_micro_version)])
# This is the X.Y used in -lunique-FOO-X.Y
m4_define([unique_api_version], [1.0])
# This is the X.Y used in the protocol negotiation
m4_define([unique_protocol_version], [1.0])

AC_PREREQ(2.54)

AC_INIT([unique], [unique_version], [ebassi@gnome.org], [unique])

AC_CONFIG_SRCDIR([unique/unique.h])

AM_INIT_AUTOMAKE
AM_CONFIG_HEADER(config.h)

# version symbols
UNIQUE_MAJOR_VERSION=unique_major_version
UNIQUE_MINOR_VERSION=unique_minor_version
UNIQUE_MICRO_VERSION=unique_micro_version
UNIQUE_VERSION=unique_version
UNIQUE_API_VERSION=unique_api_version
UNIQUE_PROTOCOL_VERSION=unique_protocol_version
AC_SUBST(UNIQUE_MAJOR_VERSION)
AC_SUBST(UNIQUE_MINOR_VERSION)
AC_SUBST(UNIQUE_MICRO_VERSION)
AC_SUBST(UNIQUE_VERSION)
AC_SUBST(UNIQUE_API_VERSION)
AC_SUBST(UNIQUE_PROTOCOL_VERSION)

# libtool
m4_define([lt_current],
          [m4_eval(100 * unique_minor_version + unique_micro_version - unique_interface_age)])
m4_define([lt_revision], [unique_interface_age])
m4_define([lt_age], [m4_eval(unique_binary_age - unique_interface_age)])
LT_VERSION_INFO="lt_current:lt_revision:lt_age"
LT_CURRENT_MINUS_AGE=m4_eval(lt_current - lt_age)
AC_SUBST(LT_VERSION_INFO)
AC_SUBST(LT_CURRENT_MINUS_AGE)

AC_ISC_POSIX
AC_PROG_CC
AC_PATH_XTRA
AC_PROG_INSTALL
AC_PROG_MAKE_SET
AC_C_CONST
AC_PROG_LIBTOOL
AC_PATH_PROG(GLIB_MKENUMS, glib-mkenums)
AC_PATH_PROG(GLIB_GENMARSHAL, glib-genmarshal)

AM_SANITY_CHECK
AM_PROG_CC_STDC

# Honor aclocal flags
ACLOCAL="$ACLOCAL $ACLOCAL_FLAGS"

GLIB_REQUIRED=2.12.0
GTK_REQUIRED=2.11.0
DBUS_GLIB_REQUIRED=0.70

PKG_CHECK_MODULES(UNIQUE, glib-2.0 >= $GLIB_REQUIRED dnl
                          gtk+-2.0 >= $GTK_REQUIRED)

dnl D-Bus backend dependencies
if $PKG_CONFIG --atleast-version $DBUS_GLIB_REQUIRED dbus-glib-1; then
  have_dbus=yes
  PKG_CHECK_MODULES(DBUS, dbus-glib-1 >= $DBUS_GLIB_REQUIRED)
  AC_SUBST(DBUS_CFLAGS)
  AC_SUBST(DBUS_LIBS)
  AC_DEFINE([HAVE_DBUS], [1], [Building with D-Bus support]) 
  AC_PATH_PROG(DBUS_BINDING_TOOL, dbus-binding-tool)
else
  have_dbus=no
fi

AM_CONDITIONAL([HAVE_DBUS], [test x$have_dbus = xyes])

dnl Bacon backend
dnl This is the fallback backend, so we *need* these headers and functions
dnl even if we end up using D-Bus
AC_CHECK_HEADERS([fcntl.h sys/types.h sys/socket.h sys/un.h])

dnl Choose the default backend
if test x$have_dbus = xyes; then
  UNIQUE_DEFAULT_BACKEND=dbus
else
  UNIQUE_DEFAULT_BACKEND=bacon
fi

AC_SUBST(UNIQUE_DEFAULT_BACKEND)

# use strict compiler flags only on development releases
m4_define([maintainer_flags_default],
          m4_if(m4_eval(unique_minor_version % 2), [1], [yes], [no]))
AC_ARG_ENABLE([maintainer-flags],
              AC_HELP_STRING([--enable-maintainer-flags=@<:@no/yes@:>@],
                             [Use strict compiler flags @<:@default=no@:>@]),,
              enable_maintainer_flags=maintainer_flags_default)

if test "x$enable_maintainer_flags" = "xyes"; then
  CPPFLAGS="$CPPFLAGS -Werror -Wall -Wshadow -Wcast-align -Wno-uninitialized"
else
  CPPFLAGS="-g -Wall"
fi

# enable debug flags and symbols
m4_define([debug_default],
          m4_if(m4_eval(unique_minor_version % 2), [1], [yes], [minimum]))
AC_ARG_ENABLE([debug],
              AC_HELP_STRING([--enable-debug=@<:@no/minimum/yes@:>@],
                             [Turn on debugging @<:@default=debug_default@:>@]),,
              enable_debug=debug_default)
if test "x$enable_debug" = "xyes"; then
  UNIQUE_DEBUG_CFLAGS="-DUNIQUE_ENABLE_DEBUG"
else
  if test "x$enable_debug" = "xno"; then
    UNIQUE_DEBUG_CFLAGS="-DG_DISABLE_ASSERT -DG_DISABLE_CHECKS -DG_DISABLE_CAST_CHECKS"
  else
    # test x$enable_debug = xminimum
    UNIQUE_DEBUG_CFLAGS="-DG_DISABLE_CAST_CHECKS"
  fi
fi

AC_SUBST(UNIQUE_DEBUG_CFLAGS)

# i18n stuff
ALL_LINGUAS=""
AM_GLIB_GNU_GETTEXT

GETTEXT_PACKAGE=unique
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED([GETTEXT_PACKAGE], ["$GETTEXT_PACKAGE"],
                   [Define the gettext package to be used])

AM_GLIB_DEFINE_LOCALEDIR(UNIQUE_LOCALEDIR)

GTK_DOC_CHECK([1.6])

AC_CONFIG_FILES([
        Makefile
        unique.pc
        doc/Makefile
        doc/reference/Makefile
        doc/reference/version.xml
        unique/Makefile
        unique/uniqueversion.h
        unique/bacon/Makefile
        unique/dbus/Makefile
        tests/Makefile
        po/Makefile.in
])

AC_OUTPUT

echo "
Configuration:
            Maintainer flags: $enable_maintainer_flags
                 Debug level: $enable_debug
         Build documentation: $enable_gtk_doc
"
