/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <cassert>
#include <stack>
#include <string>

#include "libebook_utils.h"
#include "EBOOKOPFParser.h"
#include "EBOOKSAXParser.h"
#include "EPubParser.h"
#include "EPubToken.h"

using librevenge::RVNGBinaryData;
using librevenge::RVNGInputStream;
using librevenge::RVNGPropertyList;
using librevenge::RVNGString;

using std::string;

namespace libebook
{

namespace
{

class ContainerParser : public EBOOKSAXParser
{
public:
  ContainerParser(const RVNGInputStreamPtr_t &input, string &opfName);
  virtual ~ContainerParser();

  virtual int getId(const char *name, const char *ns) const;

  virtual void startElement(int id);
  virtual void endElement(int id);
  virtual void attribute(int id, const char *value);

  virtual void startElementByName(const char *name, const char *ns);
  virtual void endElementByName(const char *name, const char *ns);
  virtual void attributeByName(const char *name, const char *ns, const char *value);

  virtual void text(const char *value);

private:
  string &m_opfName;
  std::stack<int> m_elementStack;
  bool m_versionOk;
  string m_rootPath;
  bool m_rootOk;
  bool m_found;
};

ContainerParser::ContainerParser(const RVNGInputStreamPtr_t &input, string &opfName)
  : EBOOKSAXParser(input)
  , m_opfName(opfName)
  , m_elementStack()
  , m_versionOk(false)
  , m_rootPath()
  , m_rootOk(false)
  , m_found(false)
{
}

ContainerParser::~ContainerParser()
{
  assert(m_elementStack.empty());
}

int ContainerParser::getId(const char *const name, const char *const ns) const
{
  return getEPubTokenId(name, ns);
}

void ContainerParser::startElement(const int id)
{
  if ((EPubToken::NS_container | EPubToken::container) == id)
    assert(m_elementStack.empty());
  else if (!m_versionOk)
    throw UnsupportedFormat();

  m_elementStack.push(id);
}

void ContainerParser::endElement(const int id)
{
  assert(!m_elementStack.empty());
  assert(m_elementStack.top() == id);

  switch (id)
  {
  case EPubToken::NS_container | EPubToken::rootfiles :
    if (!m_found)
      throw PackageError();
    break;
  case EPubToken::NS_container | EPubToken::rootfile :
    if (m_rootOk)
    {
      m_opfName = m_rootPath;
      m_rootOk = false;
      m_found = true;
    }
    break;
  default :
    break;
  }

  m_elementStack.pop();
}

void ContainerParser::attribute(const int id, const char *const value)
{
  assert(!m_elementStack.empty());

  switch (id)
  {
  case EPubToken::full_path :
    if (!m_found)
      m_rootPath = value;
    break;
  case EPubToken::media_type :
    m_rootOk = EPubToken::MIME_opf == getEPubTokenId(value);
    if (m_rootOk && m_found)
    {
      EBOOK_DEBUG_MSG(("duplicate OPF file found\n"));
      m_rootOk = false;
    }
    break;
  case EPubToken::version :
    if ((EPubToken::NS_container | EPubToken::container) == m_elementStack.top())
      m_versionOk = EPubToken::_1_0 == getEPubTokenId(value);
    break;
  default :
    break;
  }
}

void ContainerParser::startElementByName(const char *, const char *)
{
}

void ContainerParser::endElementByName(const char *, const char *)
{
}

void ContainerParser::attributeByName(const char *, const char *, const char *)
{
}

void ContainerParser::text(const char *)
{
}

}

namespace
{

const RVNGInputStreamPtr_t getOPFStream(const RVNGInputStreamPtr_t &package)
{
  const RVNGInputStreamPtr_t container(package->getSubStreamByName("META-INF/container.xml"));
  if (!container)
    throw PackageError();

  string opfName;

  ContainerParser parser(container, opfName);
  parser.parse();

  const RVNGInputStreamPtr_t opf(package->getSubStreamByName(opfName.c_str()));
  if (!opf)
    throw PackageError();

  return opf;
}

}

EPubParser::EPubParser(librevenge::RVNGInputStream *input, librevenge::RVNGTextInterface *document)
  : m_input(input)
  , m_document(document)
{
  assert(m_input);
}

void EPubParser::parse()
{
  if (m_document)
  {
    const RVNGInputStreamPtr_t package(m_input, EBOOKDummyDeleter());
    const RVNGInputStreamPtr_t opf = getOPFStream(package);

    EBOOKOPFParser parser(opf, package, EBOOKOPFParser::TYPE_EPub, m_document);
    parser.parse();
  }
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
