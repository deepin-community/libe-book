/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <cassert>
#include <deque>
#include <stack>
#include <string>
#include <unordered_map>

#include <boost/optional.hpp>

#include "EBOOKHTMLParser.h"
#include "EBOOKOPFParser.h"
#include "EBOOKOPFToken.h"
#include "EBOOKSubDocument.h"
#include "EBOOKSAXParser.h"

using boost::optional;

using librevenge::RVNGBinaryData;
using librevenge::RVNGInputStream;
using librevenge::RVNGPropertyList;
using librevenge::RVNGString;

using std::stack;
using std::string;

namespace libebook
{

namespace
{

enum SpineItemType
{
  SPINE_ITEM_TYPE_HTML,
  SPINE_ITEM_TYPE_IMAGE,
  SPINE_ITEM_TYPE_OTHER
};

struct SpineItem
{
  string path;
  string mimetype;
  SpineItemType type;

  SpineItem();
};

typedef std::deque<SpineItem> Spine_t;

typedef SpineItem ManifestItem;
typedef std::unordered_map<string, ManifestItem> Manifest_t;

struct Package
{
  RVNGPropertyList metadata;
  Manifest_t manifest;
  Spine_t spine;

  Package();
};

SpineItem::SpineItem()
  : path()
  , mimetype()
  , type(SPINE_ITEM_TYPE_OTHER)
{
}

Package::Package()
  : metadata()
  , manifest()
  , spine()
{
}

}

namespace
{

class HTMLParserImpl : public EBOOKHTMLParser
{
  // disable copying
  HTMLParserImpl(const HTMLParserImpl &);
  HTMLParserImpl &operator=(const HTMLParserImpl &);

public:
  HTMLParserImpl(const RVNGInputStreamPtr_t &input, const RVNGInputStreamPtr_t &package, librevenge::RVNGTextInterface *document);
  ~HTMLParserImpl();

  virtual RVNGInputStream *getImage(const char *path);
  virtual RVNGInputStream *getObject(const char *path);
  virtual RVNGInputStream *getStylesheet(const char *path);

private:
  RVNGInputStreamPtr_t getStream(const char *path) const;

private:
  const RVNGInputStreamPtr_t m_package;
};

HTMLParserImpl::HTMLParserImpl(const RVNGInputStreamPtr_t &input, const RVNGInputStreamPtr_t &package, librevenge::RVNGTextInterface *const document)
  : EBOOKHTMLParser(input.get(), EBOOKHTMLParser::DIALECT_XHTML, document)
  , m_package(package)
{
  assert(bool(m_package));
}

HTMLParserImpl::~HTMLParserImpl()
{
}

RVNGInputStream *HTMLParserImpl::getImage(const char *const path)
{
  return getStream(path).get();
}

RVNGInputStream *HTMLParserImpl::getObject(const char *const path)
{
  return getStream(path).get();
}

RVNGInputStream *HTMLParserImpl::getStylesheet(const char *const path)
{
  return getStream(path).get();
}

RVNGInputStreamPtr_t HTMLParserImpl::getStream(const char *const path) const
{
  const RVNGInputStreamPtr_t stream(m_package->getSubStreamByName(path));
  return stream;
}

}

namespace
{

struct SavedAttributes
{
  optional<int> event;
  optional<string> href;
  optional<string> id;
  optional<string> idref;
  optional<bool> linear;
  optional<string> mediaType;
  optional<int> role;

  SavedAttributes();
};

SavedAttributes::SavedAttributes()
  : event()
  , href()
  , id()
  , idref()
  , linear()
  , mediaType()
  , role()
{
}

}

namespace
{

class OPFParserImpl : public EBOOKSAXParser
{
  enum Context
  {
    CONTEXT_XML,
    CONTEXT_PACKAGE,
    CONTEXT_METADATA,
    CONTEXT_MANIFEST,
    CONTEXT_SPINE,
    CONTEXT_DC,
  };

public:
  OPFParserImpl(const RVNGInputStreamPtr_t &input, Package &package);
  virtual ~OPFParserImpl();

  virtual int getId(const char *name, const char *ns) const;

  virtual void startElement(int id);
  virtual void endElement(int id);
  virtual void attribute(int id, const char *value);

  virtual void startElementByName(const char *name, const char *ns);
  virtual void endElementByName(const char *name, const char *ns);
  virtual void attributeByName(const char *name, const char *ns, const char *value);

  virtual void text(const char *value);

private:
  Package &m_package;
  stack<Context> m_contextStack;
  stack<int> m_elementStack;
  SavedAttributes m_currentAttrs;
};

OPFParserImpl::OPFParserImpl(const RVNGInputStreamPtr_t &input, Package &package)
  : EBOOKSAXParser(input)
  , m_package(package)
  , m_contextStack()
  , m_elementStack()
  , m_currentAttrs()
{
  m_contextStack.push(CONTEXT_XML);
}

OPFParserImpl::~OPFParserImpl()
{
  assert(!m_contextStack.empty());
  assert(CONTEXT_XML == m_contextStack.top());
  assert(m_elementStack.empty());
}

int OPFParserImpl::getId(const char *const name, const char *const ns) const
{
  return getOPFTokenId(name, ns);
}

void OPFParserImpl::startElement(const int id)
{
  assert(!m_contextStack.empty());
  if (CONTEXT_XML == m_contextStack.top())
    assert(m_elementStack.empty());

  switch (id)
  {
  case EBOOKOPFToken::NS_dc | EBOOKOPFToken::Contributor :
  case EBOOKOPFToken::NS_dc | EBOOKOPFToken::Coverage :
  case EBOOKOPFToken::NS_dc | EBOOKOPFToken::Creator :
  case EBOOKOPFToken::NS_dc | EBOOKOPFToken::Date :
  case EBOOKOPFToken::NS_dc | EBOOKOPFToken::Description :
  case EBOOKOPFToken::NS_dc | EBOOKOPFToken::Format :
  case EBOOKOPFToken::NS_dc | EBOOKOPFToken::Identifier :
  case EBOOKOPFToken::NS_dc | EBOOKOPFToken::Language :
  case EBOOKOPFToken::NS_dc | EBOOKOPFToken::Publisher :
  case EBOOKOPFToken::NS_dc | EBOOKOPFToken::Relation :
  case EBOOKOPFToken::NS_dc | EBOOKOPFToken::Rights :
  case EBOOKOPFToken::NS_dc | EBOOKOPFToken::Source :
  case EBOOKOPFToken::NS_dc | EBOOKOPFToken::Subject :
  case EBOOKOPFToken::NS_dc | EBOOKOPFToken::Title :
  case EBOOKOPFToken::NS_dc | EBOOKOPFToken::Type :
  case EBOOKOPFToken::NS_dc | EBOOKOPFToken::contributor :
  case EBOOKOPFToken::NS_dc | EBOOKOPFToken::coverage :
  case EBOOKOPFToken::NS_dc | EBOOKOPFToken::creator :
  case EBOOKOPFToken::NS_dc | EBOOKOPFToken::date :
  case EBOOKOPFToken::NS_dc | EBOOKOPFToken::description :
  case EBOOKOPFToken::NS_dc | EBOOKOPFToken::format :
  case EBOOKOPFToken::NS_dc | EBOOKOPFToken::identifier :
  case EBOOKOPFToken::NS_dc | EBOOKOPFToken::language :
  case EBOOKOPFToken::NS_dc | EBOOKOPFToken::publisher :
  case EBOOKOPFToken::NS_dc | EBOOKOPFToken::relation :
  case EBOOKOPFToken::NS_dc | EBOOKOPFToken::rights :
  case EBOOKOPFToken::NS_dc | EBOOKOPFToken::source :
  case EBOOKOPFToken::NS_dc | EBOOKOPFToken::subject :
  case EBOOKOPFToken::NS_dc | EBOOKOPFToken::title :
  case EBOOKOPFToken::NS_dc | EBOOKOPFToken::type :
    assert(CONTEXT_METADATA == m_contextStack.top());
    m_contextStack.push(CONTEXT_DC);
    break;
  case EBOOKOPFToken::dc_metadata :
  case EBOOKOPFToken::NS_opf | EBOOKOPFToken::dc_metadata :
    assert(CONTEXT_METADATA == m_contextStack.top());
    break;
  case EBOOKOPFToken::item :
  case EBOOKOPFToken::NS_opf | EBOOKOPFToken::item :
    assert(CONTEXT_MANIFEST == m_contextStack.top());
    break;
  case EBOOKOPFToken::itemref :
  case EBOOKOPFToken::NS_opf | EBOOKOPFToken::itemref :
    assert(CONTEXT_SPINE == m_contextStack.top());
    break;
  case EBOOKOPFToken::manifest :
  case EBOOKOPFToken::NS_opf | EBOOKOPFToken::manifest :
    assert(CONTEXT_PACKAGE == m_contextStack.top());
    m_contextStack.push(CONTEXT_MANIFEST);
    break;
  case EBOOKOPFToken::meta :
  case EBOOKOPFToken::NS_opf | EBOOKOPFToken::meta :
    assert(CONTEXT_METADATA == m_contextStack.top());
    break;
  case EBOOKOPFToken::metadata :
  case EBOOKOPFToken::NS_opf | EBOOKOPFToken::metadata :
    assert(CONTEXT_PACKAGE == m_contextStack.top());
    m_contextStack.push(CONTEXT_METADATA);
    break;
  case EBOOKOPFToken::package :
  case EBOOKOPFToken::NS_opf | EBOOKOPFToken::package :
    assert(CONTEXT_XML == m_contextStack.top());
    m_contextStack.push(CONTEXT_PACKAGE);
    break;
  case EBOOKOPFToken::spine :
  case EBOOKOPFToken::NS_opf | EBOOKOPFToken::spine :
    assert(CONTEXT_PACKAGE == m_contextStack.top());
    m_contextStack.push(CONTEXT_SPINE);
    break;
  case EBOOKOPFToken::x_metadata :
  case EBOOKOPFToken::NS_opf | EBOOKOPFToken::x_metadata :
    assert(CONTEXT_METADATA == m_contextStack.top());
    break;
  default :
    break;
  }

  assert(!m_elementStack.empty());
}

void OPFParserImpl::endElement(const int id)
{
  assert(!m_contextStack.empty());

  switch (id)
  {
  case EBOOKOPFToken::NS_dc | EBOOKOPFToken::Contributor :
  case EBOOKOPFToken::NS_dc | EBOOKOPFToken::Coverage :
  case EBOOKOPFToken::NS_dc | EBOOKOPFToken::Creator :
  case EBOOKOPFToken::NS_dc | EBOOKOPFToken::Date :
  case EBOOKOPFToken::NS_dc | EBOOKOPFToken::Description :
  case EBOOKOPFToken::NS_dc | EBOOKOPFToken::Format :
  case EBOOKOPFToken::NS_dc | EBOOKOPFToken::Identifier :
  case EBOOKOPFToken::NS_dc | EBOOKOPFToken::Language :
  case EBOOKOPFToken::NS_dc | EBOOKOPFToken::Publisher :
  case EBOOKOPFToken::NS_dc | EBOOKOPFToken::Relation :
  case EBOOKOPFToken::NS_dc | EBOOKOPFToken::Rights :
  case EBOOKOPFToken::NS_dc | EBOOKOPFToken::Source :
  case EBOOKOPFToken::NS_dc | EBOOKOPFToken::Subject :
  case EBOOKOPFToken::NS_dc | EBOOKOPFToken::Title :
  case EBOOKOPFToken::NS_dc | EBOOKOPFToken::Type :
  case EBOOKOPFToken::NS_dc | EBOOKOPFToken::contributor :
  case EBOOKOPFToken::NS_dc | EBOOKOPFToken::coverage :
  case EBOOKOPFToken::NS_dc | EBOOKOPFToken::creator :
  case EBOOKOPFToken::NS_dc | EBOOKOPFToken::date :
  case EBOOKOPFToken::NS_dc | EBOOKOPFToken::description :
  case EBOOKOPFToken::NS_dc | EBOOKOPFToken::format :
  case EBOOKOPFToken::NS_dc | EBOOKOPFToken::identifier :
  case EBOOKOPFToken::NS_dc | EBOOKOPFToken::language :
  case EBOOKOPFToken::NS_dc | EBOOKOPFToken::publisher :
  case EBOOKOPFToken::NS_dc | EBOOKOPFToken::relation :
  case EBOOKOPFToken::NS_dc | EBOOKOPFToken::rights :
  case EBOOKOPFToken::NS_dc | EBOOKOPFToken::source :
  case EBOOKOPFToken::NS_dc | EBOOKOPFToken::subject :
  case EBOOKOPFToken::NS_dc | EBOOKOPFToken::title :
  case EBOOKOPFToken::NS_dc | EBOOKOPFToken::type :
    assert(CONTEXT_DC == m_contextStack.top());
    m_contextStack.pop();
    break;
  case EBOOKOPFToken::item :
  case EBOOKOPFToken::NS_opf | EBOOKOPFToken::item :
    assert(CONTEXT_MANIFEST == m_contextStack.top());
    if (m_currentAttrs.id && m_currentAttrs.href && m_currentAttrs.mediaType)
    {
      ManifestItem item;
      item.path = get(m_currentAttrs.href);
      item.mimetype = get(m_currentAttrs.mediaType);

      switch (getOPFTokenId(item.mimetype.c_str(), item.mimetype.size()))
      {
      case EBOOKOPFToken::MIME_oeb :
      case EBOOKOPFToken::MIME_xhtml :
        item.type = SPINE_ITEM_TYPE_HTML;
        break;
      default :
      {
        const string::size_type slash = item.mimetype.find('/');
        if (string::npos != slash)
        {
          const string cls = item.mimetype.substr(0, slash + 1);
          if (EBOOKOPFToken::MIME_image == getOPFTokenId(cls.c_str(), cls.size()))
            item.type = SPINE_ITEM_TYPE_IMAGE;
        }
        break;
      }
      }

      m_package.manifest[get(m_currentAttrs.id)] = item;
    }
    break;
  case EBOOKOPFToken::itemref :
  case EBOOKOPFToken::NS_opf | EBOOKOPFToken::itemref :
    if (m_currentAttrs.idref && (!m_currentAttrs.linear || (get(m_currentAttrs.linear))))
    {
      const Manifest_t::const_iterator it = m_package.manifest.find(get(m_currentAttrs.idref));
      if (m_package.manifest.end() != it)
        m_package.spine.push_back(it->second);
    }
    break;
  case EBOOKOPFToken::manifest :
  case EBOOKOPFToken::NS_opf | EBOOKOPFToken::manifest :
    assert(CONTEXT_MANIFEST == m_contextStack.top());
    m_contextStack.pop();
    break;
  case EBOOKOPFToken::metadata :
  case EBOOKOPFToken::NS_opf | EBOOKOPFToken::metadata :
    assert(CONTEXT_METADATA == m_contextStack.top());
    m_contextStack.pop();
    break;
  case EBOOKOPFToken::package :
  case EBOOKOPFToken::NS_opf | EBOOKOPFToken::package :
    assert(CONTEXT_PACKAGE == m_contextStack.top());
    m_contextStack.pop();
    assert(m_contextStack.empty());
    break;
  case EBOOKOPFToken::spine :
  case EBOOKOPFToken::NS_opf | EBOOKOPFToken::spine :
    assert(CONTEXT_SPINE == m_contextStack.top());
    m_contextStack.pop();
    break;
  default :
    break;
  }

  m_currentAttrs = SavedAttributes();

  assert(!m_contextStack.empty());
  if (CONTEXT_XML == m_contextStack.top())
    assert(m_elementStack.empty());
}

void OPFParserImpl::attribute(const int id, const char *const value)
{
  assert(!m_contextStack.empty());
  assert(!m_elementStack.empty());

  switch (id)
  {
  case EBOOKOPFToken::content :
    break;
  case EBOOKOPFToken::event :
  case EBOOKOPFToken::NS_opf | EBOOKOPFToken::event :
    if (CONTEXT_METADATA == m_contextStack.top())
      m_currentAttrs.event = getOPFTokenId(value);
    break;
  case EBOOKOPFToken::fallback :
  case EBOOKOPFToken::fallback_style :
    break;
  case EBOOKOPFToken::file_as :
  case EBOOKOPFToken::NS_opf | EBOOKOPFToken::file_as :
    break;
  case EBOOKOPFToken::href :
    if (((EBOOKOPFToken::NS_opf | EBOOKOPFToken::item) == m_elementStack.top())
        || (EBOOKOPFToken::item == m_elementStack.top()))
      m_currentAttrs.href = string(value);
    break;
  case EBOOKOPFToken::id :
    if (((EBOOKOPFToken::NS_opf | EBOOKOPFToken::item) == m_elementStack.top())
        || (EBOOKOPFToken::item == m_elementStack.top()))
      m_currentAttrs.id = string(value);
    break;
  case EBOOKOPFToken::idref :
    if (((EBOOKOPFToken::NS_opf | EBOOKOPFToken::itemref) == m_elementStack.top())
        || (EBOOKOPFToken::itemref == m_elementStack.top()))
      m_currentAttrs.idref = string(value);
    break;
  case EBOOKOPFToken::NS_xml | EBOOKOPFToken::lang :
    break;
  case EBOOKOPFToken::linear :
    if (((EBOOKOPFToken::NS_opf | EBOOKOPFToken::itemref) == m_elementStack.top())
        || (EBOOKOPFToken::itemref == m_elementStack.top()))
    {
      switch (getOPFTokenId(value))
      {
      case EBOOKOPFToken::no :
        m_currentAttrs.linear = false;
        break;
      case EBOOKOPFToken::yes :
        m_currentAttrs.linear = true;
        break;
      default :
        EBOOK_DEBUG_MSG(("invalid value of itemref/@linear: %s\n", value));
      }
    }
    break;
  case EBOOKOPFToken::media_type :
    if (((EBOOKOPFToken::NS_opf | EBOOKOPFToken::item) == m_elementStack.top())
        || (EBOOKOPFToken::item == m_elementStack.top()))
      m_currentAttrs.mediaType = value;
    break;
  case EBOOKOPFToken::name :
  case EBOOKOPFToken::required_modules :
  case EBOOKOPFToken::required_namespace :
    break;
  case EBOOKOPFToken::role :
  case EBOOKOPFToken::NS_opf | EBOOKOPFToken::role :
    if (CONTEXT_METADATA == m_contextStack.top())
      m_currentAttrs.role = getOPFTokenId(value);
    break;
  case EBOOKOPFToken::scheme :
  case EBOOKOPFToken::NS_opf | EBOOKOPFToken::scheme :
  case EBOOKOPFToken::toc :
  case EBOOKOPFToken::unique_identifier :
  case EBOOKOPFToken::version :
  default :
    break;
  }
}

void OPFParserImpl::startElementByName(const char *, const char *)
{
}

void OPFParserImpl::endElementByName(const char *, const char *)
{
}

void OPFParserImpl::attributeByName(const char *, const char *, const char *)
{
}

void OPFParserImpl::text(const char *const value)
{
  assert(!m_contextStack.empty());
  assert(!m_elementStack.empty());

  if (CONTEXT_DC == m_contextStack.top())
  {
    switch (m_elementStack.top())
    {
    case EBOOKOPFToken::NS_dc | EBOOKOPFToken::Contributor :
    case EBOOKOPFToken::NS_dc | EBOOKOPFToken::contributor :
    case EBOOKOPFToken::NS_dc | EBOOKOPFToken::Coverage :
    case EBOOKOPFToken::NS_dc | EBOOKOPFToken::coverage :
      break;
    case EBOOKOPFToken::NS_dc | EBOOKOPFToken::Creator :
    case EBOOKOPFToken::NS_dc | EBOOKOPFToken::creator :
      if (bool(m_currentAttrs.role))
      {
        switch (get(m_currentAttrs.role))
        {
        case EBOOKOPFToken::aut :
          m_package.metadata.insert("meta:initial-creator", value);
          break;
        case EBOOKOPFToken::edt :
          m_package.metadata.insert("librevenge:editor", value);
          break;
        default :
          break;
        }
      }
      else
      {
        m_package.metadata.insert("meta:initial-creator", value);
      }
      break;
    case EBOOKOPFToken::NS_dc | EBOOKOPFToken::Date :
    case EBOOKOPFToken::NS_dc | EBOOKOPFToken::date :
      if (bool(m_currentAttrs.event))
      {
        switch (get(m_currentAttrs.event))
        {
        case EBOOKOPFToken::creation :
          m_package.metadata.insert("meta:creation-date", value);
          break;
        case EBOOKOPFToken::publication :
          m_package.metadata.insert("dcterms:available", value);
          break;
        case EBOOKOPFToken::modification :
          m_package.metadata.insert("dcterms:issued", value);
        default :
          break;
        }
      }
      else
      {
        m_package.metadata.insert("dcterms:issued", value);
      }
      break;
    case EBOOKOPFToken::NS_dc | EBOOKOPFToken::Description :
    case EBOOKOPFToken::NS_dc | EBOOKOPFToken::description :
    case EBOOKOPFToken::NS_dc | EBOOKOPFToken::Format :
    case EBOOKOPFToken::NS_dc | EBOOKOPFToken::format :
    case EBOOKOPFToken::NS_dc | EBOOKOPFToken::Identifier :
    case EBOOKOPFToken::NS_dc | EBOOKOPFToken::identifier :
      break;
    case EBOOKOPFToken::NS_dc | EBOOKOPFToken::Language :
    case EBOOKOPFToken::NS_dc | EBOOKOPFToken::language :
      m_package.metadata.insert("dc:language", value);
      break;
    case EBOOKOPFToken::NS_dc | EBOOKOPFToken::Publisher :
    case EBOOKOPFToken::NS_dc | EBOOKOPFToken::publisher :
      m_package.metadata.insert("dc:publisher", value);
      break;
    case EBOOKOPFToken::NS_dc | EBOOKOPFToken::Relation :
    case EBOOKOPFToken::NS_dc | EBOOKOPFToken::relation :
    case EBOOKOPFToken::NS_dc | EBOOKOPFToken::Rights :
    case EBOOKOPFToken::NS_dc | EBOOKOPFToken::rights :
      break;
    case EBOOKOPFToken::NS_dc | EBOOKOPFToken::Source :
    case EBOOKOPFToken::NS_dc | EBOOKOPFToken::source :
      m_package.metadata.insert("dc:source", value);
      break;
    case EBOOKOPFToken::NS_dc | EBOOKOPFToken::Subject :
    case EBOOKOPFToken::NS_dc | EBOOKOPFToken::subject :
      m_package.metadata.insert("dc:subject", value);
      break;
    case EBOOKOPFToken::NS_dc | EBOOKOPFToken::Title :
    case EBOOKOPFToken::NS_dc | EBOOKOPFToken::title :
      break;
    case EBOOKOPFToken::NS_dc | EBOOKOPFToken::Type :
    case EBOOKOPFToken::NS_dc | EBOOKOPFToken::type :
      m_package.metadata.insert("dc:type", value);
      break;
    default :
      break;
    }
  }
}

}

EBOOKOPFParser::EBOOKOPFParser(const RVNGInputStreamPtr_t &input, const RVNGInputStreamPtr_t &package,
                               const Type type, librevenge::RVNGTextInterface *const document)
  : m_input(input)
  , m_package(package)
  , m_type(type)
  , m_document(document)
{
  assert(bool(m_input));
  assert(bool(m_package));
  assert(bool(m_document));
}

void EBOOKOPFParser::parse()
{
  Package package;

  OPFParserImpl parser(m_input, package);
  parser.parse();

  if (!package.spine.empty())
  {
    m_document->startDocument(package.metadata);

    for (Spine_t::const_iterator it = package.spine.begin(); package.spine.end() != it; ++it)
    {
      switch (it->type)
      {
      case SPINE_ITEM_TYPE_HTML :
      {
        const RVNGInputStreamPtr_t html = getStream(it->path.c_str());

        EBOOKSubDocument subDocument(*m_document);
        HTMLParserImpl htmlParser(html, m_package, &subDocument);
        htmlParser.parse();

        break;
      }

      case SPINE_ITEM_TYPE_IMAGE :
      {
        const RVNGInputStreamPtr_t image = getStream(it->path.c_str());
        const unsigned len = getRemainingLength(image);
        const unsigned char *const data = readNBytes(image, len);

        RVNGPropertyList props;
        props.insert("librevenge:mime-type", RVNGString(it->mimetype.c_str()));
        props.insert("office:binary-data", RVNGBinaryData(data, len));

        m_document->openPageSpan(getDefaultPageSpanPropList());
        m_document->insertBinaryObject(props);
        m_document->closePageSpan();

        break;
      }

      case SPINE_ITEM_TYPE_OTHER :
      default :
        // TODO: handle fallbacks
        EBOOK_DEBUG_MSG(("unhandled spine item of MIME type %s\n", it->mimetype.c_str()));
      }
    }

    m_document->endDocument();
  }
}

bool EBOOKOPFParser::findOPFStream(const RVNGInputStreamPtr_t &package, unsigned &stream)
{
  return findSubStreamByExt(package, ".opf", stream);
}

const RVNGInputStreamPtr_t EBOOKOPFParser::getStream(const char *const name) const
{
  const RVNGInputStreamPtr_t stream(m_package->getSubStreamByName(name));
  if (!stream)
    throw PackageError();
  return stream;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
