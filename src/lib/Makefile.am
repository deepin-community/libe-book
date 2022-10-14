## -*- Mode: make; tab-width: 4; indent-tabs-mode: tabs -*-

GPERFFLAGS = --compare-strncmp --enum --null-strings --readonly-tables

BUILT_SOURCES = \
	EBOOKHTMLToken.inc \
	EBOOKOPFToken.inc \
	EPubToken.inc \
	FictionBook2Token.inc \
	FictionBook3Token.inc \
	RocketEBookToken.inc

if PLATFORM_WIN32
version_info = -version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE) -avoid-version
else
version_info = -version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE)
endif

noinst_LTLIBRARIES = libe-book-internal.la
lib_LTLIBRARIES = libe-book-@EBOOK_MAJOR_VERSION@.@EBOOK_MINOR_VERSION@.la

AM_CXXFLAGS = \
	-DLIBE_BOOK_BUILD=1 \
	-I$(top_srcdir)/inc \
	$(REVENGE_CFLAGS) \
	$(REVENGE_STREAM_CFLAGS) \
	$(XML_CFLAGS) \
	$(ICU_CFLAGS) \
	$(ZLIB_CFLAGS) \
	$(BOOST_CFLAGS) \
	$(LANGTAG_CFLAGS) \
	$(DEBUG_CXXFLAGS)

if ENABLE_EXPERIMENTAL
AM_CXXFLAGS += \
	$(CSS_CFLAGS) \
	$(HUBBUB_CFLAGS) \
	$(MSPACK_CFLAGS)
endif

libe_book_@EBOOK_MAJOR_VERSION@_@EBOOK_MINOR_VERSION@_la_LIBADD = \
	libe-book-internal.la \
	$(REVENGE_LIBS) \
	$(XML_LIBS) \
	$(ICU_LIBS) \
	$(ZLIB_LIBS) \
	$(LANGTAG_LIBS) \
	@LIBEBOOK_WIN32_RESOURCE@

if ENABLE_EXPERIMENTAL
libe_book_@EBOOK_MAJOR_VERSION@_@EBOOK_MINOR_VERSION@_la_LIBADD += \
	$(CSS_LIBS) \
	$(HUBBUB_LIBS) \
	$(MSPACK_LIBS)

libe_book_@EBOOK_MAJOR_VERSION@_@EBOOK_MINOR_VERSION@_la_CPPFLAGS = -DLIBE_BOOK_EXPERIMENTAL
endif

libe_book_@EBOOK_MAJOR_VERSION@_@EBOOK_MINOR_VERSION@_la_DEPENDENCIES = libe-book-internal.la @LIBEBOOK_WIN32_RESOURCE@
libe_book_@EBOOK_MAJOR_VERSION@_@EBOOK_MINOR_VERSION@_la_LDFLAGS = $(version_info) -export-dynamic -no-undefined
libe_book_@EBOOK_MAJOR_VERSION@_@EBOOK_MINOR_VERSION@_la_SOURCES = \
	EBOOKDocument.cpp

libe_book_internal_la_CPPFLAGS = -DBOOST_ERROR_CODE_HEADER_ONLY
libe_book_internal_la_SOURCES = \
	BBeBCollector.cpp \
	BBeBCollector.h \
	BBeBMetadataParser.cpp \
	BBeBMetadataParser.h \
	BBeBParser.cpp \
	BBeBParser.h \
	BBeBTypes.cpp \
	BBeBTypes.h \
	EBOOKBitStream.cpp \
	EBOOKBitStream.h \
	EBOOKCharsetConverter.cpp \
	EBOOKCharsetConverter.h \
	EBOOKHTMLToken.cpp \
	EBOOKHTMLToken.h \
	EBOOKLanguageManager.cpp \
	EBOOKLanguageManager.h \
	EBOOKMemoryStream.cpp \
	EBOOKMemoryStream.h \
	EBOOKOPFToken.cpp \
	EBOOKOPFToken.h \
	EBOOKOutputElements.cpp \
	EBOOKOutputElements.h \
	EBOOKStreamView.cpp \
	EBOOKStreamView.h \
	EBOOKSubDocument.cpp \
	EBOOKSubDocument.h \
	EBOOKToken.h \
	EBOOKTokenizer.cpp \
	EBOOKTokenizer.h \
	EBOOKUTF8Stream.cpp \
	EBOOKUTF8Stream.h \
	EBOOKXMLContext.cpp \
	EBOOKXMLContext.h \
	EBOOKXMLContextBase.cpp \
	EBOOKXMLContextBase.h \
	EBOOKXMLParser.cpp \
	EBOOKXMLParser.h \
	EBOOKZlibStream.cpp \
	EBOOKZlibStream.h \
	EPubToken.cpp \
	EPubToken.h \
	FictionBook2BinaryContext.cpp \
	FictionBook2BinaryContext.h \
	FictionBook2BlockContext.cpp \
	FictionBook2BlockContext.h \
	FictionBook2Collector.cpp \
	FictionBook2Collector.h \
	FictionBook2ContentCollector.cpp \
	FictionBook2ContentCollector.h \
	FictionBook2ExtrasCollector.cpp \
	FictionBook2ExtrasCollector.h \
	FictionBook2MetadataCollector.cpp \
	FictionBook2MetadataCollector.h \
	FictionBook2MetadataContext.cpp \
	FictionBook2MetadataContext.h \
	FictionBook2Parser.cpp \
	FictionBook2Parser.h \
	FictionBook2ParserContext.cpp \
	FictionBook2ParserContext.h \
	FictionBook2Style.cpp \
	FictionBook2Style.h \
	FictionBook2TableContext.cpp \
	FictionBook2TableContext.h \
	FictionBook2TextContext.cpp \
	FictionBook2TextContext.h \
	FictionBook2Token.cpp \
	FictionBook2Token.h \
	FictionBook2XMLParserContext.cpp \
	FictionBook2XMLParserContext.h \
	FictionBook3Parser.cpp \
	FictionBook3Parser.h \
	FictionBook3Token.cpp \
	FictionBook3Token.h \
	PDBLZ77Stream.cpp \
	PDBLZ77Stream.h \
	PDBParser.cpp \
	PDBParser.h \
	PalmDocParser.cpp \
	PalmDocParser.h \
	PeanutPressParser.cpp \
	PeanutPressParser.h \
	PeanutPressTypes.cpp \
	PeanutPressTypes.h \
	PluckerParser.cpp \
	PluckerParser.h \
	QiOOParser.cpp \
	QiOOParser.h \
	RocketEBookToken.cpp \
	RocketEBookToken.h \
	SoftBookCollector.cpp \
	SoftBookCollector.h \
	SoftBookHeader.cpp \
	SoftBookHeader.h \
	SoftBookLZSSStream.cpp \
	SoftBookLZSSStream.h \
	SoftBookParser.cpp \
	SoftBookParser.h \
	SoftBookResourceDir.cpp \
	SoftBookResourceDir.h \
	SoftBookText.cpp \
	SoftBookText.h \
	SoftBookTypes.cpp \
	SoftBookTypes.h \
	TCRParser.cpp \
	TCRParser.h \
	TealDocParser.cpp \
	TealDocParser.h \
	ZTXTParser.cpp \
	ZTXTParser.h \
	ZVRParser.cpp \
	ZVRParser.h \
	libebook_utils.cpp \
	libebook_utils.h \
	libebook_xml.cpp \
	libebook_xml.h

if ENABLE_EXPERIMENTAL
libe_book_internal_la_SOURCES += \
	EBOOKHTMLParser.cpp \
	EBOOKHTMLParser.h \
	EBOOKHTMLTypes.cpp \
	EBOOKHTMLTypes.h \
	EBOOKOPFParser.cpp \
	EBOOKOPFParser.h \
	EBOOKSAXParser.cpp \
	EBOOKSAXParser.h \
	EPubParser.cpp \
	EPubParser.h \
	HTMLHelpParser.cpp \
	HTMLHelpParser.h \
	HTMLHelpStream.cpp \
	HTMLHelpStream.h \
	HTMLParser.cpp \
	HTMLParser.h \
	LITParser.cpp \
	LITParser.h \
	LITStream.cpp \
	LITStream.h \
	MobiPocketParser.cpp \
	MobiPocketParser.h \
	OpenEBookParser.cpp \
	OpenEBookParser.h \
	RocketEBookHeader.cpp \
	RocketEBookHeader.h \
	RocketEBookParser.cpp \
	RocketEBookParser.h \
	RocketEBookStream.cpp \
	RocketEBookStream.h \
	XMLCollector.cpp \
	XMLCollector.h \
	XMLStylesheet.cpp \
	XMLStylesheet.h \
	XMLTreeNode.cpp \
	XMLTreeNode.h \
	XMLTreeWalker.cpp \
	XMLTreeWalker.h \
	libebook_hubbub.cpp \
	libebook_hubbub.h \
	libebook_libcss.cpp \
	libebook_libcss.h
endif

if OS_WIN32

@LIBEBOOK_WIN32_RESOURCE@ : libe-book.rc $(libe_book_@EBOOK_MAJOR_VERSION@_@EBOOK_MINOR_VERSION@_la_OBJECTS)
	chmod +x $(top_srcdir)/build/win32/*compile-resource
	WINDRES=@WINDRES@ $(top_srcdir)/build/win32/lt-compile-resource libe-book.rc @LIBEBOOK_WIN32_RESOURCE@

endif

distclean-local :
	rm -f $(BUILT_SOURCES)

EXTRA_DIST = \
	EBOOKHTMLToken.gperf \
	EBOOKOPFToken.gperf \
	EPubToken.gperf \
	FictionBook2Token.gperf \
	FictionBook3Token.gperf \
	RocketEBookToken.gperf \
	libe-book.rc.in \
	libe-book.rc

%.inc : %.gperf
	if $(GPERF) $(GPERFFLAGS) --language C++ $< | sed -e 's/\<register //g' > $@.tmp; then \
		mv $@.tmp $@; \
	else \
		rm -f $@.tmp; \
		if $(GPERF) --version >/dev/null 2>&1; then \
			exit 1; \
		else \
			touch $@; \
		fi \
	fi

## vim:set shiftwidth=4 tabstop=4 noexpandtab:
