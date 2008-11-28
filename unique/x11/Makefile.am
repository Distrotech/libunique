libuniqueincludedir = $(includedir)/unique-1.0/unique

INCLUDES = \
	-DG_LOG_DOMAIN=\"Unique-X11\"  	\
	-I$(top_srcdir)			\
	$(UNIQUE_CFLAGS)		\
	$(UNIQUE_DEBUG_FLAGS)

LDADD = $(UNIQUE_LIBS)

noinst_LTLIBRARIES = libunique-x11.la

libunique_x11_la_SOURCES = 	\
	uniquebackend-x11.c	\
	uniquebackend-x11.h
