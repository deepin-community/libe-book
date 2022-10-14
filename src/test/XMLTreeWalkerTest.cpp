/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <string>

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include "libebook_utils.h"
#include "EBOOKHTMLParser.h"
#include "EBOOKHTMLToken.h"
#include "EBOOKMemoryStream.h"
#include "XMLTreeWalker.h"

namespace HTML = libebook::EBOOKHTMLToken;

using libebook::EBOOKHTMLParser;
using libebook::EBOOKOutputElements;
using libebook::XMLTreeWalker;
using libebook::XMLTreeNodePtr_t;

using librevenge::RVNGInputStream;

using std::string;

namespace test
{

class XMLTreeWalkerTest : public CPPUNIT_NS::TestFixture
{
public:
  virtual void setUp();
  virtual void tearDown();

private:
  CPPUNIT_TEST_SUITE(XMLTreeWalkerTest);
  CPPUNIT_TEST(testBuild);
  CPPUNIT_TEST(testWalk);
  CPPUNIT_TEST_SUITE_END();

private:
  void testBuild();
  void testWalk();
};

namespace
{

class HTMLParserImpl : public EBOOKHTMLParser
{
public:
  explicit HTMLParserImpl(RVNGInputStream *input);
  ~HTMLParserImpl();

private:
  virtual librevenge::RVNGInputStream *getImage(const char *path);
  virtual librevenge::RVNGInputStream *getObject(const char *path);
  virtual librevenge::RVNGInputStream *getStylesheet(const char *path);
};

HTMLParserImpl::HTMLParserImpl(RVNGInputStream *const input)
  : EBOOKHTMLParser(input, EBOOKHTMLParser::DIALECT_HTML, 0)
{
}

HTMLParserImpl::~HTMLParserImpl()
{
}

librevenge::RVNGInputStream *HTMLParserImpl::getImage(const char *)
{
  return 0;
}

librevenge::RVNGInputStream *HTMLParserImpl::getObject(const char *)
{
  return 0;
}

librevenge::RVNGInputStream *HTMLParserImpl::getStylesheet(const char *)
{
  return 0;
}

XMLTreeNodePtr_t readTree(const unsigned char *const data, const size_t len)
{
  libebook::EBOOKMemoryStream input(data, len);
  HTMLParserImpl parser(&input);
  return parser.readTree();
}

}

void XMLTreeWalkerTest::setUp()
{
}

void XMLTreeWalkerTest::tearDown()
{
}

void XMLTreeWalkerTest::testBuild()
{
  {
    const unsigned char test[] = "<html></html>";
    CPPUNIT_ASSERT(bool(readTree(test, EBOOK_NUM_ELEMENTS(test))));
  }

  {
    const unsigned char test[] = "<html><head><title>simple</title></head><body></body></html>";
    CPPUNIT_ASSERT(readTree(test, EBOOK_NUM_ELEMENTS(test)));
  }

  {
    const unsigned char test[] = "<?xml version='1.0'?>"
                                 "<html xmlns='http://www.w3.org/1999/xhtml/1.0'>"
                                 "<head><title>XHTML</title></head>"
                                 "<body></body>"
                                 "</html>"
                                 ;
    CPPUNIT_ASSERT(readTree(test, EBOOK_NUM_ELEMENTS(test)));
  }

  {
    const unsigned char test[] = "<html>"
                                 "<head><title>hello world</title></head>"
                                 "<body><p>hello world</p></body>"
                                 "</html>"
                                 ;
    CPPUNIT_ASSERT(readTree(test, EBOOK_NUM_ELEMENTS(test)));
  }

  {
    const unsigned char test[] = "<html>"
                                 "<head><title>attributes</title></head>"
                                 "<body><p lang='en'>hello world</p></body>"
                                 "</html>"
                                 ;
    CPPUNIT_ASSERT(readTree(test, EBOOK_NUM_ELEMENTS(test)));
  }
}

void XMLTreeWalkerTest::testWalk()
{
  const unsigned char test[] = "<html>"
                               "<head><title>hello</title>"
                               "<body text='blue' bgcolor=#222222><p id=P1>hello</p></body>"
                               "</html>"
                               ;
  const string htmlNs = "http://www.w3.org/1999/xhtml";

  const XMLTreeNodePtr_t tree = readTree(test, EBOOK_NUM_ELEMENTS(test));
  const XMLTreeWalker root(tree, libebook::getHTMLTokenId);

  CPPUNIT_ASSERT(root.isDocument());
  CPPUNIT_ASSERT(!root.empty());
  CPPUNIT_ASSERT(!root.hasParent());

  CPPUNIT_ASSERT(root.begin() == root.begin());
  CPPUNIT_ASSERT(root.end() == root.end());
  CPPUNIT_ASSERT(root.begin() != root.end());

  CPPUNIT_ASSERT(1 == std::distance(root.begin(), root.end()));

  XMLTreeWalker::Iterator it = root.begin();
  const XMLTreeWalker html = *it;

  ++it;
  CPPUNIT_ASSERT(root.end() == it);

  CPPUNIT_ASSERT(html.isElement());
  CPPUNIT_ASSERT(!html.empty());
  CPPUNIT_ASSERT(!html.hasAttributes());
  CPPUNIT_ASSERT(html.hasParent());

  CPPUNIT_ASSERT((HTML::html | HTML::NS_html) == html.getId());
  CPPUNIT_ASSERT("html" == html.getName());
  CPPUNIT_ASSERT(htmlNs == html.getNamespace());

  CPPUNIT_ASSERT(2 == std::distance(html.begin(), html.end()));

  it = html.begin();
  const XMLTreeWalker head = *it;
  ++it;
  const XMLTreeWalker body = *it;

  CPPUNIT_ASSERT(head.isElement());
  CPPUNIT_ASSERT(head.hasParent());
  CPPUNIT_ASSERT(!head.hasAttributes());
  CPPUNIT_ASSERT(1 == std::distance(head.begin(), head.end()));
  CPPUNIT_ASSERT(head.begin()->isElement());
  CPPUNIT_ASSERT(!head.begin()->empty());

  CPPUNIT_ASSERT((HTML::head | HTML::NS_html) == head.getId());
  CPPUNIT_ASSERT("head" == head.getName());
  CPPUNIT_ASSERT(htmlNs == head.getNamespace());

  const XMLTreeWalker titleText = *head.begin()->begin();
  CPPUNIT_ASSERT(titleText.isText());
  CPPUNIT_ASSERT("hello" == titleText.getText());

  CPPUNIT_ASSERT(body.isElement());
  CPPUNIT_ASSERT(body.hasParent());

  CPPUNIT_ASSERT((HTML::body | HTML::NS_html) == body.getId());
  CPPUNIT_ASSERT("body" == body.getName());
  CPPUNIT_ASSERT(htmlNs == body.getNamespace());

  CPPUNIT_ASSERT(body.hasAttributes());
  CPPUNIT_ASSERT(2 == std::distance(body.beginAttributes(), body.endAttributes()));

  for (XMLTreeWalker::AttributeIterator attrIt = body.beginAttributes(); body.endAttributes() != attrIt; ++attrIt)
  {
    switch (attrIt->getId())
    {
    case HTML::bgcolor :
      CPPUNIT_ASSERT("bgcolor" == attrIt->getName());
      CPPUNIT_ASSERT("" == attrIt->getNamespace());
      CPPUNIT_ASSERT("#222222" == attrIt->getValue());
      break;
    case HTML::text :
      CPPUNIT_ASSERT("text" == attrIt->getName());
      CPPUNIT_ASSERT("" == attrIt->getNamespace());
      CPPUNIT_ASSERT("blue" == attrIt->getValue());
      CPPUNIT_ASSERT(HTML::blue == attrIt->getValueId());
      break;
    default :
      CPPUNIT_ASSERT_MESSAGE("unknown attribute", false);
    }
  }
}

CPPUNIT_TEST_SUITE_REGISTRATION(XMLTreeWalkerTest);

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
