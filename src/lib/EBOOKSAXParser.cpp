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

#include "EBOOKSAXParser.h"

namespace libebook
{

namespace
{

void processAttribute(const xmlTextReaderPtr reader, EBOOKSAXParser &parser)
{
  const char *const name = char_cast(xmlTextReaderConstLocalName(reader));
  const char *const ns = char_cast(xmlTextReaderConstNamespaceUri(reader));
  const char *const value = char_cast(xmlTextReaderConstValue(reader));

  const int id = parser.getId(name, ns);
  if (0 != id)
    parser.attribute(id, value);
  else
    parser.attributeByName(name, ns, value);
}

void processNode(const xmlTextReaderPtr reader, EBOOKSAXParser &parser)
{
  switch (xmlTextReaderNodeType(reader))
  {
  case XML_READER_TYPE_ELEMENT :
  {
    const char *const name = char_cast(xmlTextReaderConstLocalName(reader));
    const char *const ns = char_cast(xmlTextReaderConstNamespaceUri(reader));

    const int id = parser.getId(name, ns);
    if (0 != id)
      parser.startElement(id);
    else
      parser.startElementByName(name, ns);

    const bool isEmpty = xmlTextReaderIsEmptyElement(reader);

    if (xmlTextReaderHasAttributes(reader))
    {
      int ret = xmlTextReaderMoveToFirstAttribute(reader);
      while (1 == ret)
      {
        processAttribute(reader, parser);
        ret = xmlTextReaderMoveToNextAttribute(reader);
      }
      if (0 > ret) // some error while reading
        throw ParseError();
    }

    if (isEmpty)
    {
      if (0 != id)
        parser.endElement(id);
      else
        parser.endElementByName(name, ns);
    }

    break;
  }

  case XML_READER_TYPE_ATTRIBUTE :
    assert(!"How did i ever got there?");
    processAttribute(reader, parser);
    break;

  case XML_READER_TYPE_END_ELEMENT :
  {
    const char *const name = char_cast(xmlTextReaderConstLocalName(reader));
    const char *const ns = char_cast(xmlTextReaderConstNamespaceUri(reader));

    const int id = parser.getId(name, ns);
    if (0 != id)
      parser.endElement(id);
    else
      parser.endElementByName(name, ns);
    break;
  }

  case XML_READER_TYPE_TEXT :
    parser.text(char_cast(xmlTextReaderConstValue(reader)));
    break;

  default :
    // ignore other types of XML content
    break;
  }
}

}

EBOOKSAXParser::EBOOKSAXParser(const RVNGInputStreamPtr_t &input)
  : m_input(input)
{
  assert(bool(m_input));
}

EBOOKSAXParser::~EBOOKSAXParser()
{
}

void EBOOKSAXParser::parse()
{
  m_input->seek(0, librevenge::RVNG_SEEK_SET);

  const std::shared_ptr<xmlTextReader> reader(xmlReaderForIO(ebookXMLReadFromStream, ebookXMLCloseStream, m_input.get(), "", 0, 0),
                                              xmlFreeTextReader);
  if (!reader)
    return;

  int ret = xmlTextReaderRead(reader.get());
  while (1 == ret)
  {
    processNode(reader.get(), *this);
    ret = xmlTextReaderRead(reader.get());
  }

  xmlTextReaderClose(reader.get());

  if (0 != ret)
    throw ParseError();
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
