## -*- Mode: make; tab-width: 4; indent-tabs-mode: tabs -*-

bin_PROGRAMS = ebook2raw

AM_CXXFLAGS = \
	-I$(top_srcdir)/inc \
	$(REVENGE_CFLAGS) \
	$(REVENGE_GENERATORS_CFLAGS) \
	$(REVENGE_STREAM_CFLAGS) \
	$(BOOST_CFLAGS) \
	$(DEBUG_CXXFLAGS)

ebook2raw_DEPENDENCIES = @EBOOK2RAW_WIN32_RESOURCE@
ebook2raw_LDADD = \
	../../lib/libe-book-@EBOOK_MAJOR_VERSION@.@EBOOK_MINOR_VERSION@.la \
	$(ICU_LIBS) \
	$(REVENGE_LIBS) \
	$(REVENGE_GENERATORS_LIBS) \
	$(REVENGE_STREAM_LIBS) \
	@EBOOK2RAW_WIN32_RESOURCE@

ebook2raw_SOURCES = ebook2raw.cpp

if OS_WIN32

@EBOOK2RAW_WIN32_RESOURCE@ : ebook2raw.rc $(ebook2raw_OBJECTS)
	chmod +x $(top_srcdir)/build/win32/*compile-resource && \
	WINDRES=@WINDRES@ $(top_srcdir)/build/win32/lt-compile-resource ebook2raw.rc @EBOOK2RAW_WIN32_RESOURCE@
endif

# Include the ebook2raw_SOURCES in case we build a tarball without stream
EXTRA_DIST = \
	$(ebook2raw_SOURCES)	\
	ebook2raw.rc.in

# These may be in the builddir too
BUILD_EXTRA_DIST = \
	ebook2raw.rc
 
## vim:set shiftwidth=4 tabstop=4 noexpandtab:
