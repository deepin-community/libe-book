/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "config.h"

#include <algorithm>
#include <cstring>

#include <libe-book/libe-book.h>

#include <librevenge-stream/librevenge-stream.h>

#include <libxml/HTMLparser.h>
#include <libxml/xmlreader.h>

#include "libebook_utils.h"
#include "libebook_xml.h"
#include "EBOOKHTMLToken.h"
#include "EBOOKOPFToken.h"
#include "EPubToken.h"
#include "FictionBook2Parser.h"
#include "FictionBook2Token.h"
#include "SoftBookParser.h"
#include "BBeBParser.h"
#include "PalmDocParser.h"
#include "PluckerParser.h"
#include "PeanutPressParser.h"
#include "QiOOParser.h"
#include "TCRParser.h"
#include "TealDocParser.h"
#include "ZTXTParser.h"
#include "ZVRParser.h"

#if defined LIBE_BOOK_EXPERIMENTAL
#include "HTMLHelpParser.h"
#include "HTMLHelpStream.h"
#include "EBOOKOPFParser.h"
#include "EPubParser.h"
#include "HTMLParser.h"
#include "OpenEBookParser.h"
#include "RocketEBookParser.h"
#endif

using std::unique_ptr;
using std::shared_ptr;

using librevenge::RVNGInputStream;

using std::equal;

namespace libebook
{

namespace
{

enum BOMEncoding
{
  BOM_ENCODING_UTF8,
  BOM_ENCODING_UTF16BE,
  BOM_ENCODING_UTF16LE,
  BOM_ENCODING_OTHER
};

static const unsigned char BOM_UTF8[] = "\xef\xbb\xbf";
static const unsigned char BOM_UTF16BE[] = "\xfe\xff";
static const unsigned char BOM_UTF16LE[] = "\xff\xfe";

static const unsigned char XML_DECL_UTF8[] = "<?xml ";
static const unsigned char XML_DECL_UTF16BE[] = "\0<\0?\0x\0m\0l\0 ";
static const unsigned char XML_DECL_UTF16LE[] = "<\0?\0x\0m\0l\0 \0";

BOMEncoding detectBOMEncoding(RVNGInputStream *const input)
{
  BOMEncoding encoding = BOM_ENCODING_OTHER;

  const unsigned char *const bom = readNBytes(input, 3);
  if (equal(BOM_UTF8, BOM_UTF8 + EBOOK_NUM_ELEMENTS(BOM_UTF8) - 1, bom))
    encoding = BOM_ENCODING_UTF8;
  else if (equal(BOM_UTF16BE, BOM_UTF16BE + EBOOK_NUM_ELEMENTS(BOM_UTF16BE) - 1, bom))
    encoding = BOM_ENCODING_UTF16BE;
  else if (equal(BOM_UTF16LE, BOM_UTF16LE + EBOOK_NUM_ELEMENTS(BOM_UTF16LE) - 1, bom))
    encoding = BOM_ENCODING_UTF16LE;
  else
    seek(input, 0); // no BOM, return the swallowed chars back

  return encoding;
}

bool isXML(RVNGInputStream *const input)
{
  bool xml = false;

  const unsigned char *decl = nullptr;
  unsigned len = 0;

  const BOMEncoding bom = detectBOMEncoding(input);
  switch (bom)
  {
  case BOM_ENCODING_UTF16BE :
    decl = XML_DECL_UTF16BE;
    len = EBOOK_NUM_ELEMENTS(XML_DECL_UTF16BE) - 1;
    break;
  case BOM_ENCODING_UTF16LE :
    decl = XML_DECL_UTF16LE;
    len = EBOOK_NUM_ELEMENTS(XML_DECL_UTF16LE) - 1;
    break;
  case BOM_ENCODING_UTF8 :
  case BOM_ENCODING_OTHER:
  default :
    len = EBOOK_NUM_ELEMENTS(XML_DECL_UTF8) - 1;
    decl = XML_DECL_UTF8;
  }

  const unsigned char *const data = readNBytes(input, len);
  xml = equal(decl, decl + len, data);

  seek(input, 0);

  return xml;
}

EBOOKDocument::Type detectXML(RVNGInputStream *const input) try
{
  EBOOKDocument::Type type = EBOOKDocument::TYPE_UNKNOWN;

  seek(input, 0);

  if (isXML(input))
  {
    const shared_ptr<xmlTextReader> reader(xmlReaderForIO(ebookXMLReadFromStream, ebookXMLCloseStream, input, "", nullptr, 0), xmlFreeTextReader);

    if (bool(reader))
    {
      int ret = xmlTextReaderRead(reader.get());

      // seek to the first element and check its name
      while (1 == ret)
      {
        if (XML_READER_TYPE_ELEMENT == xmlTextReaderNodeType(reader.get()))
        {
          const char *const name = char_cast(xmlTextReaderConstLocalName(reader.get()));
          const char *const uri = char_cast(xmlTextReaderConstNamespaceUri(reader.get()));
          if ((EBOOKHTMLToken::html | EBOOKHTMLToken::NS_html) == getHTMLTokenId(name, uri))
            type = EBOOKDocument::TYPE_XHTML;
          else if ((EPubToken::container | EPubToken::NS_container) == getEPubTokenId(name, uri))
            type = EBOOKDocument::TYPE_EPUB;
          else if ((EBOOKOPFToken::package | EBOOKOPFToken::NS_opf) == getOPFTokenId(name, uri))
            type = EBOOKDocument::TYPE_EPUB;
          else if (EBOOKOPFToken::package == getOPFTokenId(name, uri))
            type = EBOOKDocument::TYPE_OPENEBOOK;
          else if ((FictionBook2Token::FictionBook == getFictionBook2TokenID(name)) && (FictionBook2Token::NS_FICTIONBOOK == getFictionBook2TokenID(uri)))
            type = EBOOKDocument::TYPE_FICTIONBOOK2;
          break;
        }
        ret = xmlTextReaderRead(reader.get());
      }
    }
  }

  return type;
}
catch (...)
{
  return EBOOKDocument::TYPE_UNKNOWN;
}

#if defined LIBE_BOOK_EXPERIMENTAL

extern "C"
{

  void detectInternalSubset(void *const ctx, const xmlChar *const name, const xmlChar *, const xmlChar *);
  void detectExternalSubset(void *const ctx, const xmlChar *const name, const xmlChar *, const xmlChar *);
  void detectStartElement(void *const ctx, const xmlChar *const name, const xmlChar **);

}

struct HTMLContext
{
  xmlSAXHandler handler;
  bool detected;
  bool html;

  HTMLContext();
};

void detectHTMLContent(void *const ctx, const xmlChar *const name)
{
  HTMLContext *const context = reinterpret_cast<HTMLContext *>(ctx);

  assert(context);
  assert(name);

  if (context->detected)
    return;

  context->detected = true;
  context->html = EBOOKHTMLToken::html == getHTMLTokenId(char_cast(name));
}

HTMLContext::HTMLContext()
  : handler()
  , detected(false)
  , html(false)
{
  std::memset(&handler, 0, sizeof(handler));

  handler.internalSubset = &detectInternalSubset;
  handler.externalSubset = &detectExternalSubset;
  handler.startElement = &detectStartElement;
}

bool detectHTML(RVNGInputStream *const input) try
{
  assert(input);

  seek(input, 0);

  const unsigned len = 40;
  const char *data = char_cast(readNBytes(input, len));

  HTMLContext context;
  const shared_ptr<htmlParserCtxt> ctxt(htmlCreatePushParserCtxt(&context.handler, &context, data, len, "", XML_CHAR_ENCODING_NONE), htmlFreeParserCtxt);

  while ((XML_ERR_OK == htmlParseChunk(ctxt.get(), data, len, 0)) && !context.detected)
    data = char_cast(readNBytes(input, len));

  return context.html;
}
catch (...)
{
  return false;
}

extern "C"
{

  void detectInternalSubset(void *const ctx, const xmlChar *const name, const xmlChar *, const xmlChar *)
  {
    detectHTMLContent(ctx, name);
  }

  void detectExternalSubset(void *const ctx, const xmlChar *const name, const xmlChar *, const xmlChar *)
  {
    detectHTMLContent(ctx, name);
  }

  void detectStartElement(void *const ctx, const xmlChar *const name, const xmlChar **)
  {
    detectHTMLContent(ctx, name);
  }

}

#endif

bool findFB2Stream(const RVNGInputStreamPtr_t &package, unsigned &id)
{
  return findSubStreamByExt(package, ".fb2", id);
}


template<class Parser>
bool probe(const RVNGInputStreamPtr_t &input, const EBOOKDocument::Type type, EBOOKDocument::Type *const typeOut, EBOOKDocument::Confidence &confidence) try
{
  seek(input, 0);

  Parser parser(input);

  if (typeOut)
    *typeOut = type;
  confidence = EBOOKDocument::CONFIDENCE_EXCELLENT;

  return true;
}
catch (const UnsupportedEncryption &)
{
  confidence = EBOOKDocument::CONFIDENCE_UNSUPPORTED_ENCRYPTION;
  return false;
}
catch (...)
{
  confidence = EBOOKDocument::CONFIDENCE_NONE;
  return false;
}

template<class Parser>
bool probePtr(RVNGInputStream *input, const EBOOKDocument::Type type, EBOOKDocument::Type *const typeOut, EBOOKDocument::Confidence &confidence) try
{
  seek(input, 0);

  Parser parser(input);

  if (typeOut)
    *typeOut = type;
  confidence = EBOOKDocument::CONFIDENCE_EXCELLENT;

  return true;
}
catch (const UnsupportedEncryption &)
{
  confidence = EBOOKDocument::CONFIDENCE_UNSUPPORTED_ENCRYPTION;
  return false;
}
catch (...)
{
  confidence = EBOOKDocument::CONFIDENCE_NONE;
  return false;
}

typedef bool (*CheckTypeFun_t)(unsigned, unsigned);
typedef bool (*ProbeFun_t)(RVNGInputStream *, EBOOKDocument::Type, EBOOKDocument::Type *, EBOOKDocument::Confidence &);

struct PalmDetector
{
  CheckTypeFun_t checkFun;
  ProbeFun_t probeFun;
  EBOOKDocument::Type type;
};

static PalmDetector PALM_DETECTORS[] =
{
  {PalmDocParser::checkType, probePtr<PalmDocParser>, EBOOKDocument::TYPE_PALMDOC},
  {PluckerParser::checkType, probePtr<PluckerParser>, EBOOKDocument::TYPE_PLUCKER},
  {PeanutPressParser::checkType, probePtr<PeanutPressParser>, EBOOKDocument::TYPE_PEANUTPRESS},
  {TealDocParser::checkType, probePtr<TealDocParser>, EBOOKDocument::TYPE_TEALDOC},
  {ZTXTParser::checkType, probePtr<ZTXTParser>, EBOOKDocument::TYPE_ZTXT}
};

bool detectPalm(RVNGInputStream *const input, EBOOKDocument::Type *const type, EBOOKDocument::Confidence &confidence)
{
  unsigned typ = 0;
  unsigned creator = 0;

  try
  {
    seek(input, 60);
    typ = readU32(input, true);
    creator = readU32(input, true);
  }
  catch (...)
  {
    return false;
  }

  for (int i = 0; EBOOK_NUM_ELEMENTS(PALM_DETECTORS) != i; ++i)
  {
    const PalmDetector &detector = PALM_DETECTORS[i];
    if ((detector.checkFun)(typ, creator))
      return detector.probeFun(input, detector.type, type, confidence);
  }

  return false;
}

template<class Parser>
EBOOKDocument::Result doParse(RVNGInputStream *const input, librevenge::RVNGTextInterface *const document)
{
  Parser parser(input, document);
  parser.parse();
  return EBOOKDocument::RESULT_OK;
}

template<class Parser>
EBOOKDocument::Result doParse(const RVNGInputStreamPtr_t &input, librevenge::RVNGTextInterface *const document)
{
  Parser parser(input, document);
  parser.parse();
  return EBOOKDocument::RESULT_OK;
}

}

EBOOKAPI EBOOKDocument::Confidence EBOOKDocument::isSupported(librevenge::RVNGInputStream *const input, Type *const type) try
{
  if (!input)
    return CONFIDENCE_NONE;

  if (type)
    *type = TYPE_UNKNOWN;

  if (input->isStructured())
  {
    if (input->existsSubStream("mimetype"))
    {
      const unique_ptr<RVNGInputStream> mimetype(input->getSubStreamByName("mimetype"));
      static const char mime[] = "application/epub+zip";
      const unsigned char *const data = readNBytes(mimetype.get(), sizeof(mime));
      if (EPubToken::MIME_epub == getEPubTokenId(char_cast(data), sizeof(mime)))
      {
        if (type)
          *type = TYPE_EPUB;
        return CONFIDENCE_EXCELLENT;
      }
    }

    if (input->existsSubStream("META-INF/container.xml"))
    {
      const unique_ptr<RVNGInputStream> container(input->getSubStreamByName("META-INF/container.xml"));
      const Type xmlType = detectXML(container.get());
      if (TYPE_EPUB == xmlType)
      {
        if (type)
          *type = TYPE_EPUB;
        return CONFIDENCE_EXCELLENT;
      }
    }

    if ((input->existsSubStream("reader/MobileLibrary.class")) && (input->existsSubStream("data")))
    {
      if (type)
        *type = TYPE_QIOO;
      return CONFIDENCE_WEAK;
    }

    const RVNGInputStreamPtr_t package(input, EBOOKDummyDeleter());

#if defined LIBE_BOOK_EXPERIMENTAL
    {
      unsigned opfId = 0;
      if (EBOOKOPFParser::findOPFStream(package, opfId))
      {
        const unique_ptr<RVNGInputStream> opf(input->getSubStreamById(opfId));
        const Type xmlType = detectXML(opf.get());
        if ((TYPE_EPUB == xmlType) || (TYPE_OPENEBOOK == xmlType))
        {
          if (type)
            *type = xmlType;
          return CONFIDENCE_EXCELLENT;
        }
      }
    }
#endif

    unsigned fb2Stream = 0;
    if (findFB2Stream(package, fb2Stream))
    {
      const unique_ptr<RVNGInputStream> fb2(package->getSubStreamById(fb2Stream));
      const Type xmlType = detectXML(fb2.get());
      if (TYPE_FICTIONBOOK2 == xmlType)
      {
        if (type)
          *type = xmlType;
        return CONFIDENCE_EXCELLENT;
      }
    }
  }

  Confidence confidence = CONFIDENCE_NONE;

  if (detectPalm(input, type, confidence))
    return confidence;

  Type xmlType = detectXML(input);
  if (TYPE_UNKNOWN != xmlType)
  {
    if (type)
      *type = xmlType;

    if ((TYPE_EPUB == xmlType) || (TYPE_OPENEBOOK == xmlType))
      return CONFIDENCE_SUPPORTED_PART;
    else
      return CONFIDENCE_EXCELLENT;
  }

#if defined LIBE_BOOK_EXPERIMENTAL
  if (detectHTML(input))
  {
    if (type)
      *type = TYPE_HTML;
    return CONFIDENCE_EXCELLENT;
  }
#endif

  seek(input, 0);

  {
    const std::shared_ptr<SoftBookHeader> header(SoftBookHeader::create(input));
    if (bool(header))
    {
      if (type)
        *type = TYPE_SOFTBOOK;
      return CONFIDENCE_EXCELLENT;
    }
  }

  seek(input, 0);

  if (BBeBParser::isSupported(input))
  {
    if (type)
      *type = TYPE_BBEB;
    return CONFIDENCE_EXCELLENT;
  }

  const RVNGInputStreamPtr_t input2(input, EBOOKDummyDeleter());

#if defined LIBE_BOOK_EXPERIMENTAL
  if (probe<RocketEBookParser>(input2, TYPE_ROCKETEBOOK, type, confidence))
    return confidence;

  if (probe<HTMLHelpStream>(input2, TYPE_HTMLHELP, type, confidence))
    return confidence;
#endif

  if (probePtr<TCRParser>(input, TYPE_TCR, type, confidence))
    return CONFIDENCE_WEAK;

  if (probePtr<ZVRParser>(input, TYPE_ZVR, type, confidence))
    return CONFIDENCE_WEAK;

  return CONFIDENCE_NONE;
}
catch (...)
{
  return CONFIDENCE_NONE;
}

EBOOKAPI EBOOKDocument::Result EBOOKDocument::parse(librevenge::RVNGInputStream *const input, librevenge::RVNGTextInterface *const document, const char *const)
{
  if (!input || !document)
    return RESULT_UNSUPPORTED_FORMAT;

  Type type;
  Confidence confidence = isSupported(input, &type);
  if (CONFIDENCE_NONE == confidence)
    return RESULT_UNSUPPORTED_FORMAT;
  else if (CONFIDENCE_SUPPORTED_PART == confidence)
    return RESULT_UNSUPPORTED_FORMAT;
  else if (CONFIDENCE_UNSUPPORTED_ENCRYPTION == confidence)
    return RESULT_UNSUPPORTED_ENCRYPTION;

  return parse(input, document, type);
}

EBOOKAPI EBOOKDocument::Result EBOOKDocument::parse(librevenge::RVNGInputStream *const input, librevenge::RVNGTextInterface *const document, const EBOOKDocument::Type type, const char *const) try
{
  if (!input || !document)
    return RESULT_UNSUPPORTED_FORMAT;

  // sanity check
  if (EBOOKDocument::TYPE_UNKNOWN == type)
    return EBOOKDocument::RESULT_UNSUPPORTED_FORMAT;
  if (EBOOKDocument::TYPE_RESERVED1 <= type)
    return EBOOKDocument::RESULT_UNSUPPORTED_FORMAT;

  const RVNGInputStreamPtr_t input_(input, EBOOKDummyDeleter());

  input_->seek(0, librevenge::RVNG_SEEK_SET);

  switch (type)
  {
#if defined LIBE_BOOK_EXPERIMENTAL
  case TYPE_EPUB :
    return doParse<EPubParser>(input_.get(), document);
#endif
  case TYPE_FICTIONBOOK2 :
  {
    RVNGInputStreamPtr_t fb2Input(input_);
    if (input_->isStructured())
    {
      unsigned id = 0;
      if (!findFB2Stream(input_, id))
        return RESULT_PACKAGE_ERROR;
      fb2Input.reset(input_->getSubStreamById(id));
    }
    FictionBook2Parser parser(fb2Input.get());
    return parser.parse(document) ? RESULT_OK : RESULT_UNKNOWN_ERROR;
  }
#if defined LIBE_BOOK_EXPERIMENTAL
  case TYPE_HTML :
  case TYPE_XHTML :
  {
    HTMLParser parser(input_.get(), document, TYPE_XHTML == type);
    parser.parse();
    return RESULT_OK;
  }
  case TYPE_HTMLHELP :
  {
    HTMLHelpStream package(input_);
    return doParse<HTMLHelpParser>(RVNGInputStreamPtr_t(&package, EBOOKDummyDeleter()), document);
  }
#endif
  case TYPE_SOFTBOOK :
    return doParse<SoftBookParser>(input_.get(), document);
  case TYPE_BBEB :
    return doParse<BBeBParser>(input_.get(), document);
#if defined LIBE_BOOK_EXPERIMENTAL
  case TYPE_OPENEBOOK :
    return doParse<OpenEBookParser>(input_.get(), document);
#endif
  case TYPE_PALMDOC :
    return doParse<PalmDocParser>(input_.get(), document);
  case TYPE_PLUCKER :
    return doParse<PluckerParser>(input_.get(), document);
  case TYPE_PEANUTPRESS :
    return doParse<PeanutPressParser>(input_.get(), document);
  case TYPE_QIOO :
    return doParse<QiOOParser>(input_, document);
#if defined LIBE_BOOK_EXPERIMENTAL
  case TYPE_ROCKETEBOOK :
    return doParse<RocketEBookParser>(input_, document);
#endif
  case TYPE_TCR :
    return doParse<TCRParser>(input_.get(), document);
  case TYPE_TEALDOC :
    return doParse<TealDocParser>(input_.get(), document);
  case TYPE_ZTXT :
    return doParse<ZTXTParser>(input_.get(), document);
  case TYPE_ZVR :
    return doParse<ZVRParser>(input_.get(), document);
  case TYPE_UNKNOWN:
#if !defined LIBE_BOOK_EXPERIMENTAL
  case TYPE_EPUB:
  case TYPE_HTML:
  case TYPE_HTMLHELP:
  case TYPE_OPENEBOOK:
  case TYPE_ROCKETEBOOK:
  case TYPE_XHTML:
#endif
  case TYPE_HIEBOOK:
  case TYPE_KINDLE8:
  case TYPE_LIT:
  case TYPE_MOBIPOCKET:
  case TYPE_TEBR:
  case TYPE_TOMERAIDER:
  case TYPE_TOMERAIDER3:
  case TYPE_RESERVED1:
  case TYPE_RESERVED2:
  case TYPE_RESERVED3:
  case TYPE_RESERVED4:
  case TYPE_RESERVED5:
  case TYPE_RESERVED6:
  case TYPE_RESERVED7:
  case TYPE_RESERVED8:
  case TYPE_RESERVED9:
  default :
    EBOOK_DEBUG_MSG(("unhandled document type %d\n", type));
    return RESULT_UNSUPPORTED_FORMAT;
  }

  return RESULT_UNKNOWN_ERROR;
}
catch (const FileAccessError &)
{
  return RESULT_FILE_ACCESS_ERROR;
}
catch (const PackageError &)
{
  return RESULT_PACKAGE_ERROR;
}
catch (const PasswordMismatch &)
{
  return RESULT_PASSWORD_MISMATCH;
}
catch (const UnsupportedEncryption &)
{
  return RESULT_UNSUPPORTED_ENCRYPTION;
}
catch (const UnsupportedFormat &)
{
  return RESULT_UNSUPPORTED_FORMAT;
}
catch (...)
{
  return RESULT_UNKNOWN_ERROR;
}

} // namespace libebook

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
