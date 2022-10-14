noinst_PROGRAMS = lrffuzzer pdbfuzzer

AM_CXXFLAGS = -I$(top_srcdir)/inc \
	$(REVENGE_GENERATORS_CFLAGS) \
	$(REVENGE_CFLAGS) \
	$(REVENGE_STREAM_CFLAGS) \
	$(DEBUG_CXXFLAGS)

commonfuzzer_ldadd = \
	$(top_builddir)/src/lib/libe-book-@EBOOK_MAJOR_VERSION@.@EBOOK_MINOR_VERSION@.la \
	$(REVENGE_GENERATORS_LIBS) \
	$(REVENGE_LIBS) \
	$(REVENGE_STREAM_LIBS) \
	-lFuzzingEngine

lrffuzzer_LDADD = $(commonfuzzer_ldadd)

lrffuzzer_SOURCES = \
	lrffuzzer.cpp

pdbfuzzer_LDADD = $(commonfuzzer_ldadd)

pdbfuzzer_SOURCES = \
	pdbfuzzer.cpp
