NULL =

SUBDIRS = bacon

if HAVE_DBUS
SUBDIRS += dbus
endif

DIST_SUBDIRS = bacon dbus

INCLUDES = \
	-DG_LOG_DOMAIN=\"Unique\"				\
	-DPREFIX=\""$(prefix)"\"				\
	-I$(top_srcdir)						\
	$(UNIQUE_CFLAGS)					\
	$(UNIQUE_DEBUG_CFLAGS)					\
	$(DBUS_CFLAGS)						\
	$(DISABLE_DEPRECATED_FLAGS)				\
	$(NULL)

LDADD = \
	-version-info $(LT_VERSION_INFO)	\
	-export-dynamic				\
	-rpath $(libdir)			\
	$(NULL)

unique_sources_h = \
	uniqueapp.h 		\
	uniquebackend.h		\
	uniquemessage.h		\
	$(NULL)

unique_sources_c = \
	uniqueapp.c		\
	uniquebackend.c		\
	uniquemessage.c		\
	$(NULL)

unique_sources_priv_h = uniqueinternals.h

BUILT_SOURCES = uniquemarshal.h uniquemarshal.c

unique_backend_libs = $(top_builddir)/unique/bacon/libunique-bacon.la

if HAVE_DBUS
unique_backend_libs += $(top_builddir)/unique/dbus/libunique-dbus.la
endif

uniquedir = $(includedir)/unique-1.0/unique
unique_HEADERS = \
	$(unique_sources_h) 	\
	uniqueversion.h		\
	unique.h		\
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

libunique_1_0_la_LDFLAGS = $(LDADD)

uniquemarshal.h: stamp-uniquemarshal.h
	@true
stamp-uniquemarshal.h: uniquemarshal.list Makefile
	$(GLIB_GENMARSHAL) --header \
		--prefix=unique_marshal \
	$(top_srcdir)/unique/uniquemarshal.list >> xgen-umh && \
	(cmp -s xgen-umh uniquemarshal.h || cp xgen-umh uniquemarshal.h) && \
	rm -f xgen-umh && \
	echo timestamp > $(@F)

uniquemarshal.c: uniquemarshal.list Makefile
	( echo "#include \"uniquemarshal.h\"" ; \
	$(GLIB_GENMARSHAL) --body \
		--prefix=unique_marshal \
	$(top_srcdir)/unique/uniquemarshal.list >> xgen-umc ) && \
	cp xgen-umc uniquemarshal.c && \
	rm -f xgen-umc

CLEANFILES = stamp-uniquemarshal.h

DISTCLEANFILES = \
	uniquemarshal.h \
	uniquemarshal.c \
	uniqueversion.h \
	$(NULL)

EXTRA_DIST = uniquemarshal.list uniqueversion.h.in

