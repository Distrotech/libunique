SUBDIRS = unique tests doc po

AUTOMAKE_OPTIONS = 1.7

unique-1.0.pc: unique.pc
	@cp -f unique.pc unique-1.0.pc

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = unique-1.0.pc

EXTRA_DIST = unique.pc.in NEWS TODO

DISTCHECK_CONFIGURE_FLAGS = --enable-gtk-doc --disable-rebuilds

DISTCLEANFILES = unique-1.0.pc

dist-hook:
	@git-changelog > ChangeLog.in && \
	cp -f ChangeLog.in $(distdir)/ChangeLog && \
	rm -f ChangeLog.in

snapshot:
	$(MAKE) dist distdir=$(PACKAGE)-snap`date +"%Y%m%d"`
