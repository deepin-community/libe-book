/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <cassert>

#include <boost/optional.hpp>

#include "EBOOKHTMLParser.h"
#include "RocketEBookParser.h"
#include "RocketEBookStream.h"
#include "RocketEBookToken.h"

using boost::optional;

using librevenge::RVNGPropertyList;

using std::string;

namespace libebook
{

namespace
{

void parseKey(const string &key, const string &value, optional<string> &body, RVNGPropertyList &metadata)
{
  switch (getRocketEBookTokenId(key.c_str(), key.size()))
  {
  case RocketEBookToken::AUTHOR :
    metadata.insert("dc:creator", value.c_str());
    break;
  case RocketEBookToken::BODY :
    body = value;
    break;
  case RocketEBookToken::COMMENT :
    metadata.insert("librevenge:comments", value.c_str());
    break;
  case RocketEBookToken::REVISION :
    metadata.insert("librevenge:revision-number", value.c_str());
    break;
  case RocketEBookToken::TITLE :
    metadata.insert("dc:subject", value.c_str());
    break;
  case RocketEBookToken::TITLE_LANGUAGE :
    metadata.insert("dc:language", value.c_str());
    break;
  default :
    break;
  }
}

void parseInfo(const RVNGInputStreamPtr_t &input, optional<string> &body, RVNGPropertyList &metadata)
{
  string key;
  string value;
  bool inValue = false;

  while (!input->isEnd())
  {
    const unsigned char c = readU8(input);
    switch (c)
    {
    case '=' :
      inValue = true;
      break;
    case '\n' :
      if (inValue && !key.empty())
        parseKey(key, value, body, metadata);
      inValue = false;
      break;
    default :
      if (inValue)
        value += c;
      else
        key += c;
    }
  }
}

}

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

RocketEBookParser::RocketEBookParser(const RVNGInputStreamPtr_t &input, librevenge::RVNGTextInterface *document)
  : m_input(input)
  , m_document(document)
  , m_header(input)
{
  assert(m_input);
}

void RocketEBookParser::parse()
{
  if (!m_document)
    return;

  const RVNGInputStreamPtr_t package(new RocketEBookStream(m_input, m_header));

  const RVNGInputStreamPtr_t info(package->getSubStreamById(m_header.getInfoID()));
  if (!info)
    throw PackageError();

  optional<string> htmlName;
  RVNGPropertyList metadata;
  parseInfo(info, htmlName, metadata);
  if (!htmlName)
    throw PackageError();

  const RVNGInputStreamPtr_t html(package->getSubStreamByName(get(htmlName).c_str()));
  if (!html)
    throw PackageError();

  HTMLParserImpl parser(html, package, m_document);
  parser.parse();
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
