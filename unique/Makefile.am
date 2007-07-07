NULL =

SUBDIRS = $(uniquebackend)
DIST_SUBDIRS = dbus

target = $(uniquebackend)

INCLUDES = \
	-DG_LOG_DOMAIN=\"Unique\"	\
	-DPREFIX=\""$(prefix)"\"	\
	-I$(top_srcdir)			\
	$(UNIQUE_CFLAGS)		\
	$(UNIQUE_DEBUG_FLAGS)		\
	$(DISABLE_DEPRECATED_FLAGS)	\
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
	uniqueinternals.h	\
	uniquemarshal.c		\
	uniquemarshal.h		\
	uniquemessage.c		\
	$(NULL)

BUILT_SOURCES = uniquemarshal.h uniquemarshal.c

uniquedir = $(includedir)/unique-1.0/unique
unique_HEADERS = \
	$(unique_sources_h) 	\
	uniqueversion.h		\
	unique.h		\
	$(NULL)

libunique_bacon_1_0_la_SOURCES = $(unique_sources_c)
libunique_bacon_1_0_la_LIBADD = $(UNIQUE_LIBS) bacon/libunique-bacon.la
libunique_bacon_1_0_la_LDFLAGS = $(LDADD)

if HAVE_DBUS
libunique_dbus_1_0_la_SOURCES = $(unique_sources_c)
libunique_dbus_1_0_la_LIBADD = $(UNIQUE_LIBS) $(DBUS_LIBS) dbus/libunique-dbus.la
libunique_dbus_1_0_la_LDFLAGS = $(LDADD)
endif

libunique_xlibs_1_0_la_SOURCES = $(unique_sources_c)
libunique_xlibs_1_0_la_LIBADD = $(UNIQUE_LIBS) xlibs/libunique-xlibs.la
libunique_xlibs_1_0_la_LDFLAGS = $(LDADD)

lib_LTLIBRARIES = $(uniquebackendlib)

EXTRA_LTLIBRARIES = libunique-bacon-1.0.la libunique-xlibs-1.0.la

if HAVE_DBUS
EXTRA_LTLIBRARIES += libunique-dbus-1.0.la
endif

uniquemarshal.h: stamp-uniquemarshal.h
	@true
stamp-uniquemarshal.h: uniquemarshal.list Makefile
	$(GLIB_GENMARSHAL) --header \
		--prefix=_unique_marshal \
	$(top_srcdir)/unique/uniquemarshal.list >> xgen-umh && \
	(cmp -s xgen-umh uniquemarshal.h || cp xgen-umh uniquemarshal.h) && \
	rm -f xgen-umh && \
	echo timestamp > $(@F)

uniquemarshal.c: uniquemarshal.list Makefile
	( echo "#include \"uniquemarshal.h\"" ; \
	$(GLIB_GENMARSHAL) --body \
		--prefix=_unique_marshal \
	$(top_srcdir)/unique/uniquemarshal.list >> xgen-umc ) && \
	cp xgen-umc uniquemarshal.c && \
	rm -f xgen-umc

CLEANFILES = stamp-uniquemarshal.list

DISTCLEANFILES = \
	uniquemarshal.h \
	uniquemarshal.c \
	uniqueversion.h \
	$(NULL)

EXTRA_DIST = uniquemarshal.list uniqueversion.h.in

