## -*- Mode: make; tab-width: 4; indent-tabs-mode: tabs -*-

bin_PROGRAMS = ebook2html

AM_CXXFLAGS = \
	-I$(top_srcdir)/inc \
	$(REVENGE_CFLAGS) \
	$(REVENGE_GENERATORS_CFLAGS) \
	$(REVENGE_STREAM_CFLAGS) \
	$(BOOST_CFLAGS) \
	$(DEBUG_CXXFLAGS)

ebook2html_DEPENDENCIES = @EBOOK2HTML_WIN32_RESOURCE@
ebook2html_LDADD = \
	../../lib/libe-book-@EBOOK_MAJOR_VERSION@.@EBOOK_MINOR_VERSION@.la \
	$(ICU_LIBS) \
	$(REVENGE_LIBS) \
	$(REVENGE_GENERATORS_LIBS) \
	$(REVENGE_STREAM_LIBS) \
	@EBOOK2HTML_WIN32_RESOURCE@

ebook2html_SOURCES = ebook2html.cpp

if OS_WIN32

@EBOOK2HTML_WIN32_RESOURCE@ : ebook2html.rc $(ebook2html_OBJECTS)
	chmod +x $(top_srcdir)/build/win32/*compile-resource && \
	WINDRES=@WINDRES@ $(top_srcdir)/build/win32/lt-compile-resource ebook2html.rc @EBOOK2HTML_WIN32_RESOURCE@
endif

# Include the ebook2html_SOURCES in case we build a tarball without stream
EXTRA_DIST = \
	$(ebook2html_SOURCES)	\
	ebook2html.rc.in

# These may be in the builddir too
BUILD_EXTRA_DIST = \
	ebook2html.rc

## vim:set shiftwidth=4 tabstop=4 noexpandtab:
