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
#include "EBOOKOutputElements.h"
#include "LITParser.h"
#include "LITStream.h"

using librevenge::RVNGPropertyList;

using std::string;

namespace libebook
{

namespace
{

class HTMLParserImpl : public EBOOKHTMLParser
{
public:
  HTMLParserImpl(const RVNGInputStreamPtr_t &input, const RVNGInputStreamPtr_t &package, librevenge::RVNGTextInterface *document);
  ~HTMLParserImpl();

  virtual librevenge::RVNGInputStream *getImage(const char *path);
  virtual librevenge::RVNGInputStream *getObject(const char *path);
  virtual librevenge::RVNGInputStream *getStylesheet(const char *path);

private:
  const RVNGInputStreamPtr_t getStream(const char *path);

private:
  const RVNGInputStreamPtr_t m_package;
};

HTMLParserImpl::HTMLParserImpl(const RVNGInputStreamPtr_t &input, const RVNGInputStreamPtr_t &package, librevenge::RVNGTextInterface *const document)
  : EBOOKHTMLParser(input.get(), EBOOKHTMLParser::DIALECT_HTML, document)
  , m_package(package)
{
}

HTMLParserImpl::~HTMLParserImpl()
{
}

librevenge::RVNGInputStream *HTMLParserImpl::getImage(const char *const path)
{
  // FIXME: memory leak
  return getStream(path).get();
}

librevenge::RVNGInputStream *HTMLParserImpl::getObject(const char *const path)
{
  // FIXME: memory leak
  return getStream(path).get();
}

librevenge::RVNGInputStream *HTMLParserImpl::getStylesheet(const char *const path)
{
  // FIXME: memory leak
  return getStream(path).get();
}

const RVNGInputStreamPtr_t HTMLParserImpl::getStream(const char *const path)
{
  const RVNGInputStreamPtr_t stream(m_package->getSubStreamByName(path));
  return stream;
}

}

LITParser::LITParser(const RVNGInputStreamPtr_t &input, librevenge::RVNGTextInterface *document)
  : m_input(input)
  , m_document(document)
{
  assert(m_input);
}

void LITParser::parse()
{
  if (!m_document)
    return;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
