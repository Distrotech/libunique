libuniqueincludedir = $(includedir)/unique-1.0/unique

INCLUDES = \
	-DG_LOG_DOMAIN=\"Unique-DBus\"	\
	-I$(top_srcdir)			\
	$(UNIQUE_CFLAGS)		\
	$(UNIQUE_DEBUG_FLAGS)		\
	$(DBUS_CFLAGS)

LDADD = $(UNIQUE_LIBS) $(DBUS_LIBS)

noinst_LTLIBRARIES = libunique-dbus.la

BUILT_SOURCES =	uniquebackend-bindings.h uniquebackend-glue.h

libunique_dbus_la_SOURCES = 	\
	uniquefactory-dbus.h	\
	uniquefactory-dbus.c	\
	uniquebackend-dbus.h	\
	uniquebackend-dbus.c	\
	$(BUILT_SOURCES)

uniquebackend-bindings.h: uniquebackend-dbus.xml $(DBUS_BINDING_TOOL)
	$(DBUS_BINDING_TOOL) \
		--mode=glib-server \
		--output=$@ \
		--prefix=uniquebackend \
	uniquebackend-dbus.xml
uniquebackend-glue.h: uniquebackend-dbus.xml $(DBUS_BINDING_TOOL)
	$(DBUS_BINDING_TOOL) \
		--mode=glib-client \
		--output=$@ \
		--prefix=uniquebackend \
	uniquebackend-dbus.xml

EXTRA_DIST = uniquebackend-dbus.xml
