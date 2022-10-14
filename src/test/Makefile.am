## -*- Mode: make; tab-width: 4; indent-tabs-mode: tabs -*-

target_test = test

check_PROGRAMS = $(target_test)

AM_CXXFLAGS = \
	-I$(top_srcdir)/inc \
	-I$(top_srcdir)/src/lib \
	$(CPPUNIT_CFLAGS) \
	$(LANGTAG_CFLAGS) \
	$(REVENGE_CFLAGS) \
	$(REVENGE_STREAM_CFLAGS) \
	$(BOOST_CFLAGS) \
	$(DEBUG_CXXFLAGS)

if ENABLE_EXPERIMENTAL
AM_CXXFLAGS += \
	$(CSS_CFLAGS) \
	$(HUBBUB_CFLAGS)
endif

test_LDFLAGS = -L$(top_srcdir)/src/lib
test_LDADD = \
	$(top_builddir)/src/lib/libe-book-internal.la \
	$(CPPUNIT_LIBS) \
	$(ICU_LIBS) \
	$(LANGTAG_LIBS) \
	$(REVENGE_LIBS) \
	$(REVENGE_STREAM_LIBS) \
	$(ZLIB_LIBS)

if ENABLE_EXPERIMENTAL
test_LDADD += \
	$(CSS_LIBS) \
	$(HUBBUB_LIBS) \
	$(MSPACK_LIBS)
endif

test_SOURCES = \
	EBOOKBitStreamTest.cpp \
	EBOOKMemoryStreamTest.cpp \
	PDBLZ77StreamTest.cpp \
	SoftBookLZSSStreamTest.cpp \
	test.cpp

if WITH_LIBLANGTAG
test_SOURCES += \
	EBOOKLanguageManagerTest.cpp
endif

if ENABLE_EXPERIMENTAL
test_SOURCES += \
	XMLCollectorTest.cpp \
	XMLTreeWalkerTest.cpp
endif

TESTS = $(target_test)

## vim:set shiftwidth=4 tabstop=4 noexpandtab:
