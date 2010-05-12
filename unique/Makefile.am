include $(top_srcdir)/build/autotools/Makefile.am.silent

NULL =

SUBDIRS =

if HAVE_BACON
SUBDIRS += bacon
endif

if HAVE_DBUS
SUBDIRS += dbus
endif

DIST_SUBDIRS = bacon dbus

INCLUDES = -I$(top_srcdir)

AM_CPPFLAGS = \
	-DG_DISABLE_SINGLE_INCLUDES	\
	-DGTK_DISABLE_SINGLE_INCLUDES	\
	-DG_DISABLE_DEPRECATED		\
	-DGTK_DISABLE_DEPRECATED	\
	-DG_LOG_DOMAIN=\"Unique\"	\
	-DPREFIX=\""$(prefix)"\"	\
	$(DISABLE_DEPRECATED_FLAGS)	\
	$(UNIQUE_DEBUG_CFLAGS)		\
	$(NULL)

AM_CFLAGS = $(MAINTAINER_CFLAGS) $(DBUS_CFLAGS) $(UNIQUE_CFLAGS)

BUILT_SOURCES =

EXTRA_DIST =
CLEANFILES =
DISTCLEANFILES =

unique_sources_h = \
	$(top_srcdir)/unique/uniqueapp.h 	\
	$(top_srcdir)/unique/uniquebackend.h	\
	$(top_srcdir)/unique/uniquemessage.h	\
	$(NULL)

unique_sources_c = \
	$(srcdir)/uniqueapp.c			\
	$(srcdir)/uniquebackend.c		\
	$(srcdir)/uniquemessage.c		\
	$(NULL)

unique_sources_priv_h = \
	$(top_srcdir)/unique/uniqueinternals.h	\
	$(NULL)

# glib-mkenums rules
glib_enum_h = uniqueenumtypes.h
glib_enum_c = uniqueenumtypes.c
glib_enum_headers = $(unique_sources_h)
include $(top_srcdir)/build/autotools/Makefile.am.enums

# glib-genmarshal rules
glib_marshal_list = uniquemarshal.list
glib_marshal_prefix = unique_marshal
include $(top_srcdir)/build/autotools/Makefile.am.marshal

unique_backend_libs =

if HAVE_BACON
unique_backend_libs += $(top_builddir)/unique/bacon/libunique-bacon.la
endif

if HAVE_DBUS
unique_backend_libs += $(top_builddir)/unique/dbus/libunique-dbus.la
endif

uniquedir = $(includedir)/unique-1.0/unique
unique_HEADERS = \
	$(unique_sources_h) 				\
	$(top_builddir)/unique/uniqueenumtypes.h	\
	$(top_builddir)/unique/uniqueversion.h		\
	$(top_srcdir)/unique/unique.h			\
	$(NULL)

lib_LTLIBRARIES = libunique-1.0.la

libunique_1_0_la_SOURCES = 		\
	$(unique_sources_c) 		\
	$(unique_sources_priv_h) 	\
	$(BUILT_SOURCES)

libunique_1_0_la_LIBADD = 		\
	$(unique_backend_libs) 		\
	$(UNIQUE_LIBS)			\
	$(DBUS_LIBS)

libunique_1_0_la_LDFLAGS = 			\
	-version-info $(UNIQUE_LT_VERSION_INFO)	\
	-export-dynamic				\
	-export-symbols-regex "^unique.*"	\
	-rpath $(libdir)			\
	-no-undefined

DISTCLEANFILES += uniqueversion.h

EXTRA_DIST += uniqueversion.h.in

if HAVE_INTROSPECTION
BUILT_GIRSOURCES =

Unique-1.0.gir: $(INTROSPECTION_SCANNER) libunique-1.0.la
	$(QUIET_GEN)$(INTROSPECTION_SCANNER) -v \
		--namespace Unique --nsversion=1.0 \
		$(INCLUDES) $(AM_CPPFLAGS) \
		--c-include='unique/unique.h' \
		--include=GObject-2.0 \
		--include=Gtk-2.0 \
		--library=libunique-1.0.la \
		--libtool="$(top_builddir)/libtool" \
		--pkg gobject-2.0 \
		--pkg gtk+-2.0 \
		--output $@ \
		$(unique_sources_h) \
		$(unique_sources_c)

BUILT_GIRSOURCES += Unique-1.0.gir

girdir = $(datadir)/gir-1.0
gir_DATA = $(BUILT_GIRSOURCES)

typelibsdir = $(libdir)/girepository-1.0
typelibs_DATA = $(BUILT_GIRSOURCES:.gir=.typelib)

%.typelib: %.gir $(INTROSPECTION_COMPILER)
	$(QUIET_GEN)$(DEBUG) $(INTROSPECTION_COMPILER) \
		--includedir=$(srcdir) \
		--includedir=. \
		$(INTROSPECTION_COMPILER_OPTS) \
	$< -o $(@F)

CLEANFILES += $(BUILT_GIRSOURCES) $(typelibs_DATA)
endif # HAVE_INTROSPECTION
