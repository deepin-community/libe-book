/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <cassert>

#include <libxml/xmlreader.h>

#include "libebook_utils.h"
#include "libebook_xml.h"

#include "FictionBook2BinaryContext.h"
#include "FictionBook2BlockContext.h"
#include "FictionBook2Collector.h"
#include "FictionBook2ContentCollector.h"
#include "FictionBook2ExtrasCollector.h"
#include "FictionBook2Parser.h"
#include "FictionBook2MetadataCollector.h"
#include "FictionBook2MetadataContext.h"
#include "FictionBook2TableContext.h"
#include "FictionBook2TextContext.h"
#include "FictionBook2Token.h"

namespace libebook
{

namespace
{

class DocumentContext : public FictionBook2ParserContext
{
  // no copying
  DocumentContext(const DocumentContext &other);
  DocumentContext &operator=(const DocumentContext &other);

public:
  DocumentContext(FictionBook2Collector::NoteMap_t &notes, FictionBook2Collector::BinaryMap_t &bitmaps, librevenge::RVNGTextInterface *document = nullptr);

private:
  FictionBook2XMLParserContext *leaveContext() const override;

  FictionBook2XMLParserContext *element(const FictionBook2TokenData &name, const FictionBook2TokenData &ns) override;
  void startOfElement() override;
  void endOfElement() override;
  void attribute(const FictionBook2TokenData &name, const FictionBook2TokenData *ns, const char *value) override;
  void endOfAttributes() override;
  void text(const char *text) override;

private:
  librevenge::RVNGTextInterface *const m_document;
  FictionBook2Collector::NoteMap_t &m_notes;
  FictionBook2Collector::BinaryMap_t &m_bitmaps;
  bool m_generating;
};

class FictionBookGeneratorContext : public FictionBook2NodeContextBase
{
  // no copying
  FictionBookGeneratorContext(const FictionBookGeneratorContext &other);
  FictionBookGeneratorContext &operator=(const FictionBookGeneratorContext &other);

public:
  FictionBookGeneratorContext(FictionBook2ParserContext *parentContext, const FictionBook2Collector::NoteMap_t &notes, const FictionBook2Collector::BinaryMap_t &bitmaps, librevenge::RVNGTextInterface *document);

private:
  FictionBook2XMLParserContext *element(const FictionBook2TokenData &name, const FictionBook2TokenData &ns) override;
  void startOfElement() override;
  void endOfElement() override;
  void attribute(const FictionBook2TokenData &name, const FictionBook2TokenData *ns, const char *value) override;

private:
  librevenge::RVNGTextInterface *const m_document;
  librevenge::RVNGPropertyList m_metadata;
  FictionBook2MetadataCollector m_metadataCollector;
  FictionBook2ContentCollector m_contentCollector;
  bool m_bodyRead;
};

class FictionBookGathererContext : public FictionBook2NodeContextBase
{
  // no copying
  FictionBookGathererContext(const FictionBookGathererContext &other);
  FictionBookGathererContext &operator=(const FictionBookGathererContext &other);

public:
  FictionBookGathererContext(FictionBook2ParserContext *parentContext, FictionBook2Collector::NoteMap_t &notes, FictionBook2Collector::BinaryMap_t &bitmaps);

private:
  FictionBook2XMLParserContext *element(const FictionBook2TokenData &name, const FictionBook2TokenData &ns) override;
  void endOfElement() override;
  void attribute(const FictionBook2TokenData &name, const FictionBook2TokenData *ns, const char *value) override;

private:
  FictionBook2Collector::NoteMap_t &m_notes;
  FictionBook2Collector::BinaryMap_t &m_bitmaps;
  FictionBook2ExtrasCollector m_collector;
  bool m_firstBody;
};

class StylesheetContext : public FictionBook2NodeContextBase
{
};

}

namespace
{

FictionBookGeneratorContext::FictionBookGeneratorContext(
  FictionBook2ParserContext *const parentContext,
  const FictionBook2Collector::NoteMap_t &notes, const FictionBook2Collector::BinaryMap_t &bitmaps,
  librevenge::RVNGTextInterface *const document)
  : FictionBook2NodeContextBase(parentContext)
  , m_document(document)
  , m_metadata()
  , m_metadataCollector(m_metadata)
  , m_contentCollector(m_document, m_metadata, notes, bitmaps)
  , m_bodyRead(false)
{
}

FictionBook2XMLParserContext *FictionBookGeneratorContext::element(const FictionBook2TokenData &name, const FictionBook2TokenData &ns)
{
  if (FictionBook2Token::NS_FICTIONBOOK == getFictionBook2TokenID(ns))
  {
    switch (getFictionBook2TokenID(name))
    {
    case FictionBook2Token::stylesheet :
      // ignore
      break;
    case FictionBook2Token::description :
      return new FictionBook2DescriptionContext(this, &m_metadataCollector);
    case FictionBook2Token::body :
    {
      if (!m_bodyRead)
      {
        m_document->startDocument(librevenge::RVNGPropertyList());
        m_document->setDocumentMetaData(m_metadata);
        m_bodyRead = true;
        return new FictionBook2BodyContext(this, &m_contentCollector);
      }
    }
    break;
    default :
      break;
    }
  }

  return new FictionBook2SkipElementContext(this);
}

void FictionBookGeneratorContext::startOfElement()
{
}

void FictionBookGeneratorContext::endOfElement()
{
  m_document->endDocument();
}

void FictionBookGeneratorContext::attribute(const FictionBook2TokenData &, const FictionBook2TokenData *, const char *)
{
}

FictionBookGathererContext::FictionBookGathererContext(FictionBook2ParserContext *const parentContext, FictionBook2Collector::NoteMap_t &notes, FictionBook2Collector::BinaryMap_t &bitmaps)
  : FictionBook2NodeContextBase(parentContext)
  , m_notes(notes)
  , m_bitmaps(bitmaps)
  , m_collector(m_notes, m_bitmaps)
  , m_firstBody(true)
{
}

FictionBook2XMLParserContext *FictionBookGathererContext::element(const FictionBook2TokenData &name, const FictionBook2TokenData &ns)
{
  if (FictionBook2Token::NS_FICTIONBOOK == getFictionBook2TokenID(ns))
  {
    switch (getFictionBook2TokenID(name))
    {
    case FictionBook2Token::body :
    {
      if (m_firstBody)
        m_firstBody = false;
      else
        return new FictionBook2BodyContext(this, &m_collector);
    }
    break;
    case FictionBook2Token::binary :
      return new FictionBook2BinaryContext(this, &m_collector);
    default:
      break;
    }
  }

  return new FictionBook2SkipElementContext(this);
}

void FictionBookGathererContext::endOfElement()
{
}

void FictionBookGathererContext::attribute(const FictionBook2TokenData &, const FictionBook2TokenData *, const char *)
{
}

DocumentContext::DocumentContext(FictionBook2Collector::NoteMap_t &notes, FictionBook2Collector::BinaryMap_t &bitmaps, librevenge::RVNGTextInterface *const document)
  : FictionBook2ParserContext(nullptr)
  , m_document(document)
  , m_notes(notes)
  , m_bitmaps(bitmaps)
  , m_generating(document != nullptr)
{
}

FictionBook2XMLParserContext *DocumentContext::element(const FictionBook2TokenData &name, const FictionBook2TokenData &ns)
{
  if ((FictionBook2Token::NS_FICTIONBOOK == getFictionBook2TokenID(ns)) && (FictionBook2Token::FictionBook == getFictionBook2TokenID(name)))
  {
    if (m_generating)
      return new FictionBookGeneratorContext(this, m_notes, m_bitmaps, m_document);
    else
      return new FictionBookGathererContext(this, m_notes, m_bitmaps);
  }

  return nullptr;
}

FictionBook2XMLParserContext *DocumentContext::leaveContext() const
{
  return nullptr;
}

void DocumentContext::startOfElement()
{
}

void DocumentContext::endOfElement()
{
}

void DocumentContext::attribute(const FictionBook2TokenData &, const FictionBook2TokenData *, const char *)
{
}

void DocumentContext::endOfAttributes()
{
}

void DocumentContext::text(const char *)
{
}

}

namespace
{

void processAttribute(FictionBook2XMLParserContext *const context, const xmlTextReaderPtr reader)
{
  const FictionBook2TokenData *const name = getFictionBook2Token(xmlTextReaderConstLocalName(reader));
  const xmlChar *const nsUri = xmlTextReaderConstNamespaceUri(reader);
  const FictionBook2TokenData *const ns = nsUri ? getFictionBook2Token(nsUri) : nullptr;
  if (name && (FictionBook2Token::NS_XMLNS != getFictionBook2TokenID(ns))) // ignore unknown attributes and namespace decls
    context->attribute(*name, ns, reinterpret_cast<const char *>(xmlTextReaderConstValue(reader)));
}

FictionBook2XMLParserContext *processNode(FictionBook2XMLParserContext *const context, const xmlTextReaderPtr reader)
{
  FictionBook2XMLParserContext *newContext = context;
  switch (xmlTextReaderNodeType(reader))
  {
  case XML_READER_TYPE_ELEMENT :
  {
    const xmlChar *name_str = xmlTextReaderConstLocalName(reader);
    const xmlChar *ns_str = xmlTextReaderConstNamespaceUri(reader);

    const FictionBook2TokenData *name = name_str ? getFictionBook2Token(name_str) : nullptr;
    const FictionBook2TokenData *ns = ns_str ? getFictionBook2Token(ns_str) : nullptr;

    if (!name || !ns)
      // TODO: unknown elements should not be skipped entirely, but
      // their content should be processed as if they did not exist.
      // Unfortunately this would be quite hard (if not impossible) to
      // do in the current parser framework
      newContext = new FictionBook2SkipElementContext(dynamic_cast<FictionBook2ParserContext *>(context));
    else
      newContext = context->element(*name, *ns);

    if (newContext)
    {
      newContext->startOfElement();
      const bool isEmpty = xmlTextReaderIsEmptyElement(reader);

      if (xmlTextReaderHasAttributes(reader))
      {
        int ret = xmlTextReaderMoveToFirstAttribute(reader);
        while (1 == ret)
        {
          processAttribute(newContext, reader);
          ret = xmlTextReaderMoveToNextAttribute(reader);
        }
        if (0 > ret) // some error while reading
        {
          delete newContext;
          newContext = nullptr;
        }
      }
      if (newContext)
        newContext->endOfAttributes();

      if (newContext && isEmpty)
      {
        newContext->endOfElement();
        newContext = newContext->leaveContext();
      }
    }

    break;
  }
  case XML_READER_TYPE_ATTRIBUTE :
    assert(false && "How did i ever got there?");
    processAttribute(context, reader);
    break;
  case XML_READER_TYPE_END_ELEMENT :
  {
    context->endOfElement();
    newContext = context->leaveContext();
    break;
  }
  case XML_READER_TYPE_TEXT :
  {
    xmlChar *const text = xmlTextReaderReadString(reader);
    context->text(reinterpret_cast<char *>(text));
    xmlFree(text);
    break;
  }
  default :
    // ignore other types of XML content
    break;
  }

  return newContext;
}

}

FictionBook2Parser::FictionBook2Parser(librevenge::RVNGInputStream *input)
  : m_input(input)
{
  assert(m_input);
}

bool FictionBook2Parser::parse(FictionBook2XMLParserContext *const context) const
{
  m_input->seek(0, librevenge::RVNG_SEEK_SET);

  const xmlTextReaderPtr reader = xmlReaderForIO(ebookXMLReadFromStream, ebookXMLCloseStream, m_input, "", nullptr, 0);
  if (!reader)
    return false;

  int ret = xmlTextReaderRead(reader);
  FictionBook2XMLParserContext *currentContext = context;
  while ((1 == ret) && currentContext)
  {
    currentContext = processNode(currentContext, reader);
    if (currentContext)
      ret = xmlTextReaderRead(reader);
  }

  xmlTextReaderClose(reader);
  xmlFreeTextReader(reader);

  // we processed all input and it was valid
  return (!currentContext || (currentContext == context)) && m_input->isEnd();
}

bool FictionBook2Parser::parse(librevenge::RVNGTextInterface *const document) const
{
  FictionBook2Collector::NoteMap_t notes;
  FictionBook2Collector::BinaryMap_t bitmaps;

  {
    // in the 1st pass we gather notes and bitmaps
    DocumentContext context(notes, bitmaps);
    if (!parse(&context))
      return false;
  }

  DocumentContext context(notes, bitmaps, document);
  return parse(&context);
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
