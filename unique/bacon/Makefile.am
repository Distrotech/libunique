libuniqueincludedir = $(includedir)/unique-1.0/unique

INCLUDES = \
	-DG_LOG_DOMAIN=\"Unique-Bacon\"	\
	-I$(top_srcdir)			\
	$(UNIQUE_CFLAGS)		\
	$(UNIQUE_DEBUG_FLAGS)

LDADD = $(UNIQUE_LIBS)

noinst_LTLIBRARIES = libunique-bacon.la

libunique_bacon_la_SOURCES = 	\
	uniquebackend-bacon.c	\
	uniquebackend-bacon.h	\
	uniquefactory-bacon.c	\
	uniquefactory-bacon.h	\
	uniquemessage-bacon.c	\
	uniquemessage-bacon.h
