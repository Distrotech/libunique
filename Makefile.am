SUBDIRS = unique doc tests po

AUTOMAKE_OPTIONS = 1.7

unique-1.0.pc: unique.pc
	@cp -f unique.pc unique-1.0.pc

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = unique-1.0.pc

EXTRA_DIST = unique.pc.in NEWS TODO

DISTCHECK_CONFIGURE_FLAGS = --enable-gtk-doc --disable-rebuilds

DISTCLEANFILES = unique-1.0.pc

release:
	@rm -rf .deps */.deps
	$(MAKE) distcheck

snapshot:
	$(MAKE) dist distdir=$(PACKAGE)-snap`date +"%Y%m%d"`
