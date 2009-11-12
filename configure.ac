# versioning
m4_define([unique_major_version], [1])
m4_define([unique_minor_version], [1])
m4_define([unique_micro_version], [6])
m4_define([unique_version], [unique_major_version.unique_minor_version.unique_micro_version])
m4_define([unique_api_version], [unique_major_version.unique_minor_version])

# if functions have been added, set to 0; otherwise, increment
# with every release
m4_define([unique_interface_age], [6])
m4_define([unique_binary_age], [m4_eval(100 * unique_minor_version + unique_micro_version)])
# This is the X.Y used in -lunique-FOO-X.Y
m4_define([unique_api_version], [1.0])
# This is the X.Y used in the protocol negotiation
m4_define([unique_protocol_version], [1.0])

AC_PREREQ([2.59])

AC_INIT([unique], [unique_version],
        [http://bugzilla.gnome.org/enter_bug.cgi?product=libunique],
        [libunique])

AC_CONFIG_SRCDIR([unique/unique.h])
AC_CONFIG_MACRO_DIR([build/autotools])

AM_INIT_AUTOMAKE([1.10])
AM_CONFIG_HEADER([config.h])

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
m4_define([lt_current], [m4_eval(100 * unique_minor_version + unique_micro_version - unique_interface_age)])
m4_define([lt_revision], [unique_interface_age])
m4_define([lt_age], [m4_eval(unique_binary_age - unique_interface_age)])
UNIQUE_LT_VERSION_INFO="lt_current:lt_revision:lt_age"
UNIQUE_LT_CURRENT_MINUS_AGE=m4_eval(lt_current - lt_age)
AC_SUBST(UNIQUE_LT_VERSION_INFO)
AC_SUBST(UNIQUE_LT_CURRENT_MINUS_AGE)

AC_ISC_POSIX
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_MAKE_SET
AC_C_CONST
AC_PROG_LIBTOOL
AC_PATH_PROG(GLIB_MKENUMS, [glib-mkenums])
AC_PATH_PROG(GLIB_GENMARSHAL, [glib-genmarshal])

AM_SANITY_CHECK
AM_PROG_CC_STDC

# Honor aclocal flags
ACLOCAL="$ACLOCAL $ACLOCAL_FLAGS"

m4_define([glib_required], [2.12.0])
m4_define([gtk_required],  [2.11.0])
m4_define([dbus_required], [0.70])

PKG_CHECK_MODULES(UNIQUE, glib-2.0 >= glib_required dnl
                          gtk+-2.0 >= gtk_required)

gdktarget=`$PKG_CONFIG --variable=target gdk-2.0`
AS_IF([test "X$gdktarget" = "Xx11"],
      [
        AC_PATH_XTRA

        PKG_CHECK_MODULES(UNIQUE_X11, x11)

        UNIQUE_CFLAGS="$UNIQUE_CFLAGS $UNIQUE_X11_CFLAGS"
        UNIQUE_LIBS="$UNIQUE_LIBS $UNIQUE_X11_LIBS"
      ]
)

AC_SUBST(UNIQUE_CFLAGS)
AC_SUBST(UNIQUE_LIBS)

dnl D-Bus backend dependencies
m4_define([have_dbus_default], [yes])
AC_ARG_ENABLE([dbus],
              [AC_HELP_STRING([--enable-dbus=@<:@yes/no@:>@],
                              [Whether D-BUS IPC should be enabled])],
              [],
              [enable_dbus=have_dbus_default])

AS_CASE([$enable_dbus],

        [yes],
        [
          PKG_CHECK_MODULES(DBUS, dbus-glib-1 >= dbus_required,
                            [have_dbus=yes],
                            [have_dbus=no])
        ],

        [no], [have_dbus=no],

        [AC_MSG_ERROR([Unknown argument to --enable-dbus])]
)

AS_IF([test "x$have_dbus" = "xyes"],
      [
        AC_SUBST(DBUS_CFLAGS)
        AC_SUBST(DBUS_LIBS)
        AC_DEFINE([HAVE_DBUS], [1], [Building with D-Bus support])
        AC_PATH_PROG(DBUS_BINDING_TOOL, [dbus-binding-tool])
      ]
)

AM_CONDITIONAL([HAVE_DBUS], [test "x$have_dbus" = "xyes"])

dnl Bacon backend
dnl This is the fallback backend, so we *need* these headers and functions
dnl even if we end up using D-Bus
dnl D-Bus backend dependencies
m4_define([have_bacon_default], [yes])
AC_ARG_ENABLE([bacon],
              [AC_HELP_STRING([--enable-bacon=@<:@yes/no@:>@],
                              [Whether Unix domain sockets IPC should be enabled])],
              [],
              [enable_bacon=have_bacon_default])

AS_IF([test "x$enable_bacon" = "xyes"],
      [
        AC_CHECK_HEADERS([fcntl.h sys/types.h sys/socket.h sys/un.h],
                         [have_bacon=yes],
                         [have_bacon=no])
      ],

      [have_bacon=no]
)

AS_IF([test "x$have_bacon" = "xyes"],
      [
        AC_DEFINE([HAVE_BACON], [1], [Building with Unix domain socket support])
      ]
)

AM_CONDITIONAL([HAVE_BACON], [test "x$have_bacon" = "xyes"])

dnl Choose the default backend
AC_MSG_CHECKING([for default IPC mechanism])
AS_IF([test "x$have_dbus" = "xyes"],
      [
        UNIQUE_DEFAULT_BACKEND=dbus
        AC_MSG_RESULT([D-Bus])
      ],

      [test "x$have_bacon" = "xyes"],
      [
        UNIQUE_DEFAULT_BACKEND=bacon
        AC_MSG_RESULT([Unix domain sockets])
      ],

      [AC_MSG_ERROR([No IPC backend enabled.])]
)

AC_SUBST(UNIQUE_DEFAULT_BACKEND)

# use strict compiler flags only on development releases
m4_define([maintainer_flags_default], [m4_if(m4_eval(unique_minor_version % 2), [1], [yes], [no])])
AC_ARG_ENABLE([maintainer-flags],
              [AC_HELP_STRING([--enable-maintainer-flags=@<:@no/yes@:>@],
                              [Use strict compiler flags @<:@default=no@:>@])],
              [],
              [enable_maintainer_flags=maintainer_flags_default])

AS_IF([test "x$enable_maintainer_flags" = "xyes" &&  test "x$GCC" = "xyes"],
      [
        AS_COMPILER_FLAGS([MAINTAINER_CFLAGS],
                          ["-Werror -Wall -Wshadow -Wcast-align
                            -Wno-uninitialized -Wempty-body -Wformat-security
                            -Winit-self"])
      ]
)

AC_SUBST(MAINTAINER_CFLAGS)

# enable debug flags and symbols
m4_define([debug_default], [m4_if(m4_eval(unique_minor_version % 2), [1], [yes], [minimum])])
AC_ARG_ENABLE([debug],
              [AC_HELP_STRING([--enable-debug=@<:@no/minimum/yes@:>@],
                              [Turn on debugging @<:@default=debug_default@:>@])],
              [],
              [enable_debug=debug_default])

AS_CASE([$enable_debug],

        [yes],
        [
          test "$cflags_set" = set || CFLAGS="$CFLAGS -g"
          UNIQUE_DEBUG_CFLAGS="-DUNIQUE_ENABLE_DEBUG"
        ],

        [no],
        [
          UNIQUE_DEBUG_CFLAGS="-DG_DISABLE_ASSERT -DG_DISABLE_CHECKS -DG_DISABLE_CAST_CHECKS"
        ],

        [minimum],
        [
          UNIQUE_DEBUG_CFLAGS="-DG_DISABLE_CAST_CHECKS"
        ]

        [AC_MSG_ERROR([Unknown argument to --enable-debug])]
)

AC_SUBST(UNIQUE_DEBUG_CFLAGS)

# i18n stuff
ALL_LINGUAS=""
AM_GLIB_GNU_GETTEXT

GETTEXT_PACKAGE=unique
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED([GETTEXT_PACKAGE],
                   ["$GETTEXT_PACKAGE"],
                   [Define the gettext package to be used])

AM_GLIB_DEFINE_LOCALEDIR(UNIQUE_LOCALEDIR)

# introspection
GOBJECT_INTROSPECTION_CHECK([0.6.3])

# gtk-doc
GTK_DOC_CHECK([1.11])

# nice builds
m4_ifdef([AM_SILENT_RULES],
         [
           AM_SILENT_RULES([yes])
           use_shave=no
         ],
         [
           SHAVE_INIT([build/autotools], [enable])
           AC_CONFIG_FILES([
                build/autotools/shave-libtool
                build/autotools/shave
           ])
           use_shave=yes
         ])

AM_CONDITIONAL([USE_SHAVE], [test "x$use_shave" = "xyes"])

AC_CONFIG_FILES([
        Makefile
        unique.pc
        build/Makefile
        build/autotools/Makefile
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
Unique $UNIQUE_VERSION

Configuration:
            Maintainer flags: $enable_maintainer_flags
                 Debug level: $enable_debug
         Build documentation: $enable_gtk_doc
    Build introspection data: $enable_introspection

Backends:
          Unix Domain Socket: $have_bacon
                       D-BUS: $have_dbus

             Default backend: $UNIQUE_DEFAULT_BACKEND
"
