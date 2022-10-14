/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "EBOOKXMLParser.h"

#include <stack>

#include <libxml/xmlreader.h>

#include "EBOOKTokenizer.h"
#include "EBOOKXMLContext.h"
#include "libebook_xml.h"

using std::shared_ptr;
using std::stack;

namespace libebook
{

namespace
{

class DiscardContext : public EBOOKXMLContext, public std::enable_shared_from_this<DiscardContext>
{
private:
  void startOfElement() override;
  void attribute(int name, const char *value) override;
  void endOfAttributes() override;
  std::shared_ptr<EBOOKXMLContext> element(int name) override;
  void text(const char *value) override;
  void endOfElement() override;
};

void DiscardContext::startOfElement()
{
}

void DiscardContext::attribute(int, const char *)
{
}

void DiscardContext::endOfAttributes()
{
}

std::shared_ptr<EBOOKXMLContext> DiscardContext::element(int)
{
  return shared_from_this();
}

void DiscardContext::text(const char *)
{
}

void DiscardContext::endOfElement()
{
}

}

namespace
{

shared_ptr<EBOOKXMLContext> createDiscardContext()
{
  return std::make_shared<DiscardContext>();
}

void processAttribute(xmlTextReaderPtr reader, shared_ptr<EBOOKXMLContext> context, const EBOOKTokenizer &tokenizer)
{
  const int id = tokenizer.getQualifiedId(char_cast(xmlTextReaderConstLocalName(reader)), char_cast(xmlTextReaderConstNamespaceUri(reader)));
  const char *value = char_cast(xmlTextReaderConstValue(reader));
  context->attribute(id, value);
}

}

EBOOKXMLParser::EBOOKXMLParser(const RunStyle runStyle)
  : m_runStyle(runStyle)
{
}

EBOOKXMLParser::~EBOOKXMLParser()
{
}

bool EBOOKXMLParser::parse(const RVNGInputStreamPtr_t &input)
{
  const shared_ptr<xmlTextReader> sharedReader(xmlReaderForIO(ebookXMLReadFromStream, ebookXMLCloseStream, input.get(), "", nullptr, 0), xmlFreeTextReader);
  if (!sharedReader)
    return false;

  xmlTextReaderPtr reader = sharedReader.get();

  const EBOOKTokenizer &tokenizer = getTokenizer();
  stack<shared_ptr<EBOOKXMLContext> > contextStack;

  bool exited = false;
  int ret = xmlTextReaderRead(reader);
  contextStack.push(createDocumentContext());

  while (1 == ret)
  {
    switch (xmlTextReaderNodeType(reader))
    {
    case XML_READER_TYPE_ELEMENT:
    {
      const int id = tokenizer.getQualifiedId(char_cast(xmlTextReaderConstLocalName(reader)), char_cast(xmlTextReaderConstNamespaceUri(reader)));

      shared_ptr<EBOOKXMLContext> newContext = contextStack.top()->element(id);

      if (!newContext)
      {
        if (m_runStyle == RunToEnd)
        {
          newContext = createDiscardContext();
        }
        else
        {
          exited = true;
          break;
        }
      }

      const bool isEmpty = xmlTextReaderIsEmptyElement(reader);

      newContext->startOfElement();
      if (xmlTextReaderHasAttributes(reader))
      {
        ret = xmlTextReaderMoveToFirstAttribute(reader);
        while (1 == ret)
        {
          processAttribute(reader, newContext, tokenizer);
          ret = xmlTextReaderMoveToNextAttribute(reader);
        }
        newContext->endOfAttributes();
      }

      if (isEmpty)
        newContext->endOfElement();
      else
        contextStack.push(newContext);

      break;
    }
    case XML_READER_TYPE_END_ELEMENT:
    {
      contextStack.top()->endOfElement();
      contextStack.pop();
      break;
    }
    case XML_READER_TYPE_TEXT :
    {
      xmlChar *const text = xmlTextReaderReadString(reader);
      contextStack.top()->text(char_cast(text));
      xmlFree(text);
      break;
    }
    default:
      break;
    }

    if (exited)
      ret = 0;
    else
      ret = xmlTextReaderRead(reader);
  }

  while (!contextStack.empty()) // finish parsing in case of broken XML
  {
    contextStack.top()->endOfElement();
    contextStack.pop();
  }
  xmlTextReaderClose(reader);

  return !exited;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
