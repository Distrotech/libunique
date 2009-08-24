NULL =

QUIET_GEN = $(Q:@=@echo '  GEN   '$@;)

SUBDIRS = bacon

if HAVE_DBUS
SUBDIRS += dbus
endif

DIST_SUBDIRS = bacon dbus

INCLUDES = \
	-DG_DISABLE_SINGLE_INCLUDES	\
	-DGTK_DISABLE_SINGLE_INCLUDES	\
	-DG_DISABLE_DEPRECATED		\
	-DGTK_DISABLE_DEPRECATED	\
	-DG_LOG_DOMAIN=\"Unique\"	\
	-DPREFIX=\""$(prefix)"\"	\
	-I$(top_srcdir)			\
	$(NULL)

LDADD = \
	-version-info $(UNIQUE_LT_VERSION_INFO)	\
	-export-dynamic				\
	-export-symbols-regex "^unique.*"
	-rpath $(libdir)			\
	$(NULL)

unique_sources_h = \
	$(top_srcdir)/unique/uniqueapp.h 	\
	$(top_srcdir)/unique/uniquebackend.h	\
	$(top_srcdir)/unique/uniquemessage.h	\
	$(NULL)

unique_sources_c = \
	$(srcdir)/uniqueapp.c		\
	$(srcdir)/uniquebackend.c	\
	$(srcdir)/uniquemessage.c	\
	$(NULL)

unique_sources_priv_h = $(top_srcdir)/unique/uniqueinternals.h

BUILT_SOURCES = uniquemarshal.h uniquemarshal.c

unique_backend_libs = $(top_builddir)/unique/bacon/libunique-bacon.la

if HAVE_DBUS
unique_backend_libs += $(top_builddir)/unique/dbus/libunique-dbus.la
endif

uniquedir = $(includedir)/unique-1.0/unique
unique_HEADERS = \
	$(unique_sources_h) 			\
	$(top_builddir)/unique/uniqueversion.h	\
	$(top_srcdir)/unique/unique.h		\
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

libunique_1_0_la_CFLAGS =		\
	$(UNIQUE_CFLAGS)		\
	$(UNIQUE_DEBUG_CFLAGS)		\
	$(MAINTAINER_CFLAGS) 		\
	$(DBUS_CFLAGS)			\
	$(DISABLE_DEPRECATED_FLAGS)

libunique_1_0_la_LDFLAGS = $(LDADD)

uniquemarshal.h: stamp-uniquemarshal.h
	@true
stamp-uniquemarshal.h: uniquemarshal.list Makefile
	$(QUIET_GEN)$(GLIB_GENMARSHAL) \
		--prefix=unique_marshal \
		--header \
	$(srcdir)/uniquemarshal.list > xgen-umh \
	&& (cmp -s xgen-umh uniquemarshal.h || cp xgen-umh uniquemarshal.h) \
	&& rm -f xgen-umh \
	&& echo timestamp > $(@F)

uniquemarshal.c: uniquemarshal.list Makefile
	$(QUIET_GEN)( echo "#include \"uniquemarshal.h\"" ; \
	  $(GLIB_GENMARSHAL) \
		--prefix=unique_marshal \
		--body \
	  $(srcdir)/uniquemarshal.list ) >> xgen-umc \
	&& cp xgen-umc uniquemarshal.c \
	&& rm -f xgen-umc

CLEANFILES = stamp-uniquemarshal.h

DISTCLEANFILES = \
	uniquemarshal.h \
	uniquemarshal.c \
	uniqueversion.h \
	$(NULL)

EXTRA_DIST = uniquemarshal.list uniqueversion.h.in

if HAVE_INTROSPECTION
BUILT_GIRSOURCES =

Unique-1.0.gir: $(INTROSPECTION_SCANNER) libunique-1.0.la
	$(INTROSPECTION_SCANNER) -v \
		--namespace Unique --nsversion=1.0 \
		$(INCLUDES) \
		--c-include='unique/unique.h' \
		--include=GObject-2.0 \
		--include=Gtk-2.0 \
		--library=unique-1.0 \
		--libtool="$(top_builddir)/libtool" \
		--pkg gobject-2.0 \
		--pkg gtk+-2.0 \
		--pkg unique-1.0 \
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
