## -*- Mode: make; tab-width: 4; indent-tabs-mode: tabs -*-

ACLOCAL_AMFLAGS = -I m4

pkgconfdir = $(libdir)/pkgconfig

pkgconf_DATA = libe-book-@EBOOK_MAJOR_VERSION@.@EBOOK_MINOR_VERSION@.pc

SUBDIRS = build inc src data docs

EXTRA_DIST = \
	AUTHORS \
	COPYING \
	NEWS \
	README \
	autogen.sh \
	libe-book.pc.in

distclean-local:
	rm -rf *.cache *~ *.out *.pc

dist-hook:
	git log --date=short --pretty="format:@%cd  %an  <%ae>  [%H]%n%n%s%n%n%e%b" | sed -e "s|^\([^@]\)|\t\1|" -e "s|^@||" >$(distdir)/ChangeLog

astyle:
	astyle --options=astyle.options \*.cpp \*.h

## vim:set shiftwidth=4 tabstop=4 noexpandtab:
