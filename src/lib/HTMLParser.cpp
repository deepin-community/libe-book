/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <cassert>

#include "EBOOKHTMLParser.h"
#include "EBOOKHTMLTypes.h"
#include "HTMLParser.h"

using librevenge::RVNGPropertyList;

namespace libebook
{

namespace
{

class HTMLParserImpl : public EBOOKHTMLParser
{
public:
  HTMLParserImpl(librevenge::RVNGInputStream *input, Dialect dialect, librevenge::RVNGTextInterface *document);
  ~HTMLParserImpl();

  virtual librevenge::RVNGInputStream *getImage(const char *path);
  virtual librevenge::RVNGInputStream *getObject(const char *path);
  virtual librevenge::RVNGInputStream *getStylesheet(const char *path);
};

HTMLParserImpl::HTMLParserImpl(librevenge::RVNGInputStream *const input, const Dialect dialect, librevenge::RVNGTextInterface *const document)
  : EBOOKHTMLParser(input, dialect, document)
{
}

HTMLParserImpl::~HTMLParserImpl()
{
}

librevenge::RVNGInputStream *HTMLParserImpl::getImage(const char * /* path */)
{
  // nothing yet...
  return 0;
}

librevenge::RVNGInputStream *HTMLParserImpl::getObject(const char * /* path */)
{
  // nothing yet...
  return 0;
}

librevenge::RVNGInputStream *HTMLParserImpl::getStylesheet(const char * /* path */)
{
  // nothing yet...
  return 0;
}

}

HTMLParser::HTMLParser(librevenge::RVNGInputStream *input, librevenge::RVNGTextInterface *document, bool xhtml)
  : m_input(input)
  , m_document(document)
  , m_xhtml(xhtml)
{
  assert(m_input);
}

void HTMLParser::parse()
{
  if (!m_document)
    return;

  HTMLParserImpl parser(m_input, m_xhtml ? HTMLParserImpl::DIALECT_XHTML : HTMLParserImpl::DIALECT_HTML, m_document);
  parser.parse();
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
