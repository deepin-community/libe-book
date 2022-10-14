/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <algorithm>
#include <deque>
#include <stack>

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include <librevenge/librevenge.h>

#include "libebook_utils.h"
#include "XMLCollector.h"

using libebook::XMLCollector;

using librevenge::RVNGPropertyList;
using librevenge::RVNGString;

using std::size_t;

#define CALLGRAPH_CALL(L) \
    m_impl->m_callList.push_back(L);

#define CALLGRAPH_ENTER(L) \
    m_impl->m_callList.push_back(L); \
    m_impl->m_callStack.push(L);

#define CALLGRAPH_LEAVE(L, C) \
    m_impl->m_callList.push_back(L); \
	{ \
		const int lc = m_impl->m_callStack.top(); \
		if (lc != C) \
			++m_impl->m_callbackMisses; \
		m_impl->m_callStack.pop(); \
	}

namespace test
{

class XMLCollectorTest : public CPPUNIT_NS::TestFixture
{
public:
  virtual void setUp();
  virtual void tearDown();

private:
  CPPUNIT_TEST_SUITE(XMLCollectorTest);
  CPPUNIT_TEST(testNormalNesting);
  CPPUNIT_TEST(testMissingCalls);
  CPPUNIT_TEST(testMultipleNesting);
  CPPUNIT_TEST(testIncorrectNesting);
  CPPUNIT_TEST_SUITE_END();

private:
  void testNormalNesting();
  void testMissingCalls();
  void testMultipleNesting();
  void testIncorrectNesting();
};

namespace
{

enum Callback
{
  CALLBACK_OPEN_CHART,
  CALLBACK_OPEN_COMMENT,
  CALLBACK_OPEN_ENDNOTE,
  CALLBACK_OPEN_FOOTNOTE,
  CALLBACK_OPEN_FRAME,
  CALLBACK_OPEN_GRAPHIC,
  CALLBACK_OPEN_GRAPHIC_LAYER,
  CALLBACK_OPEN_GRAPHIC_PAGE,
  CALLBACK_OPEN_HEADER_FOOTER,
  CALLBACK_OPEN_LINK,
  CALLBACK_OPEN_LIST_ELEMENT,
  CALLBACK_OPEN_ORDERED_LIST_LEVEL,
  CALLBACK_OPEN_PAGE_SPAN,
  CALLBACK_OPEN_PARAGRAPH,
  CALLBACK_OPEN_SECTION,
  CALLBACK_OPEN_SHEET,
  CALLBACK_OPEN_SHEET_CELL,
  CALLBACK_OPEN_SHEET_ROW,
  CALLBACK_OPEN_SPAN,
  CALLBACK_OPEN_TABLE,
  CALLBACK_OPEN_TABLE_CELL,
  CALLBACK_OPEN_TABLE_ROW,
  CALLBACK_OPEN_TEXT_BOX,
  CALLBACK_OPEN_UNORDERED_LIST_LEVEL,
  CALLBACK_START_COMMENT,
  CALLBACK_START_DOCUMENT,
  CALLBACK_START_EMBEDDED_GRAPHICS,
  CALLBACK_START_GROUP,
  CALLBACK_START_LAYER,
  CALLBACK_START_NOTES,
  CALLBACK_START_PAGE,
  CALLBACK_START_SLIDE,
  CALLBACK_START_TEXT_OBJECT,
  CALLBACK_CLOSE_CHART,
  CALLBACK_CLOSE_COMMENT,
  CALLBACK_CLOSE_ENDNOTE,
  CALLBACK_CLOSE_FOOTNOTE,
  CALLBACK_CLOSE_FRAME,
  CALLBACK_CLOSE_GRAPHIC,
  CALLBACK_CLOSE_GRAPHIC_LAYER,
  CALLBACK_CLOSE_GRAPHIC_PAGE,
  CALLBACK_CLOSE_HEADER_FOOTER,
  CALLBACK_CLOSE_LINK,
  CALLBACK_CLOSE_LIST_ELEMENT,
  CALLBACK_CLOSE_ORDERED_LIST_LEVEL,
  CALLBACK_CLOSE_PAGE_SPAN,
  CALLBACK_CLOSE_PARAGRAPH,
  CALLBACK_CLOSE_SECTION,
  CALLBACK_CLOSE_SHEET,
  CALLBACK_CLOSE_SHEET_CELL,
  CALLBACK_CLOSE_SHEET_ROW,
  CALLBACK_CLOSE_SPAN,
  CALLBACK_CLOSE_TABLE,
  CALLBACK_CLOSE_TABLE_CELL,
  CALLBACK_CLOSE_TABLE_ROW,
  CALLBACK_CLOSE_TEXT_BOX,
  CALLBACK_CLOSE_UNORDERED_LIST_LEVEL,
  CALLBACK_END_COMMENT,
  CALLBACK_END_DOCUMENT,
  CALLBACK_END_EMBEDDED_GRAPHICS,
  CALLBACK_END_GROUP,
  CALLBACK_END_LAYER,
  CALLBACK_END_NOTES,
  CALLBACK_END_PAGE,
  CALLBACK_END_SLIDE,
  CALLBACK_END_TEXT_OBJECT,
  CALLBACK_INSERT_TAB,
  CALLBACK_INSERT_SPACE,
  CALLBACK_INSERT_TEXT,
  CALLBACK_INSERT_LINE_BREAK,
  CALLBACK_INSERT_FIELD,
  CALLBACK_INSERT_COVERED_TABLE_CELL,
  CALLBACK_INSERT_BINARY_OBJECT,
  CALLBACK_INSERT_EQUATION
};

struct RawGeneratorBase
{
  typedef std::deque<Callback> CallList_t;

  RawGeneratorBase();
  virtual ~RawGeneratorBase();

  CallList_t m_callList;
  std::stack<Callback> m_callStack;
  int m_callbackMisses;
};

RawGeneratorBase::RawGeneratorBase()
  : m_callList()
  , m_callStack()
  , m_callbackMisses(0)
{
}

RawGeneratorBase::~RawGeneratorBase()
{
}

}

namespace
{

struct RawGeneratorImpl;

class RawGenerator : public librevenge::RVNGTextInterface
{
  // disable copying
  RawGenerator(const RawGenerator &other);
  RawGenerator &operator=(const RawGenerator &other);

public:
  typedef RawGeneratorBase::CallList_t CallList_t;

public:
  RawGenerator();
  ~RawGenerator();

  bool isBalanced() const;
  const CallList_t &getCallList() const;

  void setDocumentMetaData(const RVNGPropertyList &propList);

  void startDocument(const RVNGPropertyList &propList);
  void endDocument();

  void defineEmbeddedFont(const RVNGPropertyList &propList);

  void definePageStyle(const RVNGPropertyList &propList);
  void openPageSpan(const RVNGPropertyList &propList);
  void closePageSpan();
  void openHeader(const RVNGPropertyList &propList);
  void closeHeader();
  void openFooter(const RVNGPropertyList &propList);
  void closeFooter();

  void defineParagraphStyle(const RVNGPropertyList &propList);
  void openParagraph(const RVNGPropertyList &propList);
  void closeParagraph();

  void defineCharacterStyle(const RVNGPropertyList &propList);
  void openSpan(const RVNGPropertyList &propList);
  void closeSpan();

  void openLink(const RVNGPropertyList &propList);
  void closeLink();

  void defineSectionStyle(const RVNGPropertyList &propList);
  void openSection(const RVNGPropertyList &propList);
  void closeSection();

  void insertTab();
  void insertSpace();
  void insertText(const RVNGString &text);
  void insertLineBreak();
  void insertField(const RVNGPropertyList &propList);

  void openOrderedListLevel(const RVNGPropertyList &propList);
  void openUnorderedListLevel(const RVNGPropertyList &propList);
  void closeOrderedListLevel();
  void closeUnorderedListLevel();
  void openListElement(const RVNGPropertyList &propList);
  void closeListElement();

  void openFootnote(const RVNGPropertyList &propList);
  void closeFootnote();
  void openEndnote(const RVNGPropertyList &propList);
  void closeEndnote();
  void openComment(const RVNGPropertyList &propList);
  void closeComment();
  void openTextBox(const RVNGPropertyList &propList);
  void closeTextBox();

  void openTable(const RVNGPropertyList &propList);
  void openTableRow(const RVNGPropertyList &propList);
  void closeTableRow();
  void openTableCell(const RVNGPropertyList &propList);
  void closeTableCell();
  void insertCoveredTableCell(const RVNGPropertyList &propList);
  void closeTable();

  void openFrame(const RVNGPropertyList &propList);
  void closeFrame();

  void openGroup(const RVNGPropertyList &propList);
  void closeGroup();

  void defineGraphicStyle(const RVNGPropertyList &propList);
  void drawRectangle(const RVNGPropertyList &propList);
  void drawEllipse(const RVNGPropertyList &propList);
  void drawPolygon(const RVNGPropertyList &propList);
  void drawPolyline(const RVNGPropertyList &propList);
  void drawPath(const RVNGPropertyList &propList);
  void drawConnector(const RVNGPropertyList &propList);

  void insertBinaryObject(const RVNGPropertyList &propList);
  void insertEquation(const RVNGPropertyList &propList);

private:
  std::unique_ptr<RawGeneratorImpl> m_impl;
};

struct RawGeneratorImpl : RawGeneratorBase
{
  explicit RawGeneratorImpl();
};

RawGeneratorImpl::RawGeneratorImpl()
  : RawGeneratorBase()
{
}

RawGenerator::RawGenerator() :
  m_impl(new RawGeneratorImpl())
{
}

RawGenerator::~RawGenerator()
{
}

bool RawGenerator::isBalanced() const
{
  return m_impl->m_callStack.empty() && (0 == m_impl->m_callbackMisses);
}

const RawGenerator::CallList_t &RawGenerator::getCallList() const
{
  return m_impl->m_callList;
}

void RawGenerator::setDocumentMetaData(const RVNGPropertyList &)
{
}

void RawGenerator::startDocument(const RVNGPropertyList &)
{
  CALLGRAPH_ENTER(CALLBACK_START_DOCUMENT);
}

void RawGenerator::endDocument()
{
  CALLGRAPH_LEAVE(CALLBACK_END_DOCUMENT, CALLBACK_START_DOCUMENT);
}

void RawGenerator::defineEmbeddedFont(const RVNGPropertyList &)
{
}

void RawGenerator::definePageStyle(const RVNGPropertyList &)
{
}

void RawGenerator::openPageSpan(const RVNGPropertyList &)
{
  CALLGRAPH_ENTER(CALLBACK_OPEN_PAGE_SPAN);
}

void RawGenerator::closePageSpan()
{
  CALLGRAPH_LEAVE(CALLBACK_CLOSE_PAGE_SPAN, CALLBACK_OPEN_PAGE_SPAN);
}

void RawGenerator::openHeader(const RVNGPropertyList &)
{
  CALLGRAPH_ENTER(CALLBACK_OPEN_HEADER_FOOTER);
}

void RawGenerator::closeHeader()
{
  CALLGRAPH_LEAVE(CALLBACK_CLOSE_HEADER_FOOTER, CALLBACK_OPEN_HEADER_FOOTER);
}

void RawGenerator::openFooter(const RVNGPropertyList &)
{
  CALLGRAPH_ENTER(CALLBACK_OPEN_HEADER_FOOTER);
}

void RawGenerator::closeFooter()
{
  CALLGRAPH_LEAVE(CALLBACK_CLOSE_HEADER_FOOTER, CALLBACK_OPEN_HEADER_FOOTER);
}

void RawGenerator::defineParagraphStyle(const RVNGPropertyList &)
{
}

void RawGenerator::openParagraph(const RVNGPropertyList &)
{
  CALLGRAPH_ENTER(CALLBACK_OPEN_PARAGRAPH);
}

void RawGenerator::closeParagraph()
{
  CALLGRAPH_LEAVE(CALLBACK_CLOSE_PARAGRAPH, CALLBACK_OPEN_PARAGRAPH);
}

void RawGenerator::defineCharacterStyle(const RVNGPropertyList &)
{
}

void RawGenerator::openSpan(const RVNGPropertyList &)
{
  CALLGRAPH_ENTER(CALLBACK_OPEN_SPAN);
}

void RawGenerator::closeSpan()
{
  CALLGRAPH_LEAVE(CALLBACK_CLOSE_SPAN, CALLBACK_OPEN_SPAN);
}

void RawGenerator::openLink(const RVNGPropertyList &)
{
  CALLGRAPH_ENTER(CALLBACK_OPEN_LINK);
}

void RawGenerator::closeLink()
{
  CALLGRAPH_LEAVE(CALLBACK_CLOSE_LINK, CALLBACK_OPEN_LINK);
}

void RawGenerator::defineSectionStyle(const RVNGPropertyList &)
{
}

void RawGenerator::openSection(const RVNGPropertyList &)
{
  CALLGRAPH_ENTER(CALLBACK_OPEN_SECTION);
}

void RawGenerator::closeSection()
{
  CALLGRAPH_LEAVE(CALLBACK_CLOSE_SECTION, CALLBACK_OPEN_SECTION);
}

void RawGenerator::insertTab()
{
  CALLGRAPH_CALL(CALLBACK_INSERT_TAB);
}

void RawGenerator::insertSpace()
{
  CALLGRAPH_CALL(CALLBACK_INSERT_SPACE);
}

void RawGenerator::insertText(const RVNGString &)
{
  CALLGRAPH_CALL(CALLBACK_INSERT_TEXT);
}

void RawGenerator::insertLineBreak()
{
  CALLGRAPH_CALL(CALLBACK_INSERT_LINE_BREAK);
}

void RawGenerator::insertField(const RVNGPropertyList &)
{
  CALLGRAPH_CALL(CALLBACK_INSERT_FIELD);
}

void RawGenerator::openOrderedListLevel(const RVNGPropertyList &)
{
  CALLGRAPH_ENTER(CALLBACK_OPEN_ORDERED_LIST_LEVEL);
}

void RawGenerator::openUnorderedListLevel(const RVNGPropertyList &)
{
  CALLGRAPH_ENTER(CALLBACK_OPEN_UNORDERED_LIST_LEVEL);
}

void RawGenerator::closeOrderedListLevel()
{
  CALLGRAPH_LEAVE(CALLBACK_CLOSE_ORDERED_LIST_LEVEL, CALLBACK_OPEN_ORDERED_LIST_LEVEL);
}

void RawGenerator::closeUnorderedListLevel()
{
  CALLGRAPH_LEAVE(CALLBACK_CLOSE_UNORDERED_LIST_LEVEL, CALLBACK_OPEN_UNORDERED_LIST_LEVEL);
}

void RawGenerator::openListElement(const RVNGPropertyList &)
{
  CALLGRAPH_ENTER(CALLBACK_OPEN_LIST_ELEMENT);
}

void RawGenerator::closeListElement()
{
  CALLGRAPH_LEAVE(CALLBACK_CLOSE_LIST_ELEMENT, CALLBACK_OPEN_LIST_ELEMENT);
}

void RawGenerator::openFootnote(const RVNGPropertyList &)
{
  CALLGRAPH_ENTER(CALLBACK_OPEN_FOOTNOTE);
}

void RawGenerator::closeFootnote()
{
  CALLGRAPH_LEAVE(CALLBACK_CLOSE_FOOTNOTE, CALLBACK_OPEN_FOOTNOTE);
}

void RawGenerator::openEndnote(const RVNGPropertyList &)
{
  CALLGRAPH_ENTER(CALLBACK_OPEN_ENDNOTE);
}

void RawGenerator::closeEndnote()
{
  CALLGRAPH_LEAVE(CALLBACK_CLOSE_ENDNOTE, CALLBACK_OPEN_ENDNOTE);
}

void RawGenerator::openComment(const RVNGPropertyList &)
{
  CALLGRAPH_ENTER(CALLBACK_OPEN_COMMENT);
}

void RawGenerator::closeComment()
{
  CALLGRAPH_LEAVE(CALLBACK_CLOSE_COMMENT, CALLBACK_OPEN_COMMENT);
}

void RawGenerator::openTextBox(const RVNGPropertyList &)
{
  CALLGRAPH_ENTER(CALLBACK_OPEN_TEXT_BOX);
}

void RawGenerator::closeTextBox()
{
  CALLGRAPH_LEAVE(CALLBACK_CLOSE_TEXT_BOX, CALLBACK_OPEN_TEXT_BOX);
}

void RawGenerator::openTable(const RVNGPropertyList &)
{
  CALLGRAPH_ENTER(CALLBACK_OPEN_TABLE);
}

void RawGenerator::openTableRow(const RVNGPropertyList &)
{
  CALLGRAPH_ENTER(CALLBACK_OPEN_TABLE_ROW);
}

void RawGenerator::closeTableRow()
{
  CALLGRAPH_LEAVE(CALLBACK_CLOSE_TABLE_ROW, CALLBACK_OPEN_TABLE_ROW);
}

void RawGenerator::openTableCell(const RVNGPropertyList &)
{
  CALLGRAPH_ENTER(CALLBACK_OPEN_TABLE_CELL);
}

void RawGenerator::closeTableCell()
{
  CALLGRAPH_LEAVE(CALLBACK_CLOSE_TABLE_CELL, CALLBACK_OPEN_TABLE_CELL);
}

void RawGenerator::insertCoveredTableCell(const RVNGPropertyList &)
{
  CALLGRAPH_CALL(CALLBACK_INSERT_COVERED_TABLE_CELL);
}

void RawGenerator::closeTable()
{
  CALLGRAPH_LEAVE(CALLBACK_CLOSE_TABLE, CALLBACK_OPEN_TABLE);
}

void RawGenerator::openFrame(const RVNGPropertyList &)
{
  CALLGRAPH_ENTER(CALLBACK_OPEN_FRAME);
}

void RawGenerator::closeFrame()
{
  CALLGRAPH_LEAVE(CALLBACK_CLOSE_FRAME, CALLBACK_OPEN_FRAME);
}

void RawGenerator::openGroup(const RVNGPropertyList &)
{
  CALLGRAPH_ENTER(CALLBACK_START_GROUP);
}

void RawGenerator::closeGroup()
{
  CALLGRAPH_LEAVE(CALLBACK_END_GROUP, CALLBACK_START_GROUP);
}

void RawGenerator::defineGraphicStyle(const RVNGPropertyList &)
{
}

void RawGenerator::drawRectangle(const RVNGPropertyList &)
{
}

void RawGenerator::drawEllipse(const RVNGPropertyList &)
{
}

void RawGenerator::drawPolygon(const RVNGPropertyList &)
{
}

void RawGenerator::drawPolyline(const RVNGPropertyList &)
{
}

void RawGenerator::drawPath(const RVNGPropertyList &)
{
}

void RawGenerator::drawConnector(const RVNGPropertyList &)
{
}


void RawGenerator::insertBinaryObject(const RVNGPropertyList &)
{
  CALLGRAPH_CALL(CALLBACK_INSERT_BINARY_OBJECT);
}

void RawGenerator::insertEquation(const RVNGPropertyList &)
{
  CALLGRAPH_CALL(CALLBACK_INSERT_EQUATION);
}

}

namespace
{

bool equal(const Callback *const expected, const size_t len, const RawGenerator::CallList_t &received)
{
  return (len == received.size()) && std::equal(received.begin(), received.end(), expected);
}

}

void XMLCollectorTest::setUp()
{
}

void XMLCollectorTest::tearDown()
{
}

void XMLCollectorTest::testNormalNesting()
{
  const Callback sd = CALLBACK_START_DOCUMENT;
  const Callback ed = CALLBACK_END_DOCUMENT;
  const Callback ops = CALLBACK_OPEN_PAGE_SPAN;
  const Callback cps = CALLBACK_CLOSE_PAGE_SPAN;
  const Callback op = CALLBACK_OPEN_PARAGRAPH;
  const Callback cp = CALLBACK_CLOSE_PARAGRAPH;
  const Callback ol = CALLBACK_OPEN_LINK;
  const Callback cl = CALLBACK_CLOSE_LINK;
  const Callback os = CALLBACK_OPEN_SPAN;
  const Callback cs = CALLBACK_CLOSE_SPAN;
  const Callback is = CALLBACK_INSERT_SPACE;

  {
    const Callback expected[] = {sd, ops, op, ol, os, is, cs, cl, cp, cps, ed};

    RawGenerator generator;
    XMLCollector collector(&generator);

    collector.openPageSpan(RVNGPropertyList());
    collector.openParagraph(RVNGPropertyList());
    collector.openLink(RVNGPropertyList());
    collector.openSpan(RVNGPropertyList());
    collector.collectSpace();
    collector.closeSpan();
    collector.closeLink();
    collector.closeParagraph();
    collector.closePageSpan();
    collector.finish();

    CPPUNIT_ASSERT_MESSAGE("text in para", generator.isBalanced());
    CPPUNIT_ASSERT_MESSAGE("text in para",
                           equal(expected, EBOOK_NUM_ELEMENTS(expected), generator.getCallList()));
  }

  {
    const Callback expected[] = {sd, ops, op, cp, cps, ed};

    RawGenerator generator;
    XMLCollector collector(&generator);

    collector.openPageSpan(RVNGPropertyList());
    collector.openParagraph(RVNGPropertyList());
    collector.closeParagraph();
    collector.closePageSpan();
    collector.finish();

    CPPUNIT_ASSERT_MESSAGE("empty para", generator.isBalanced());
    CPPUNIT_ASSERT_MESSAGE("empty para",
                           equal(expected, EBOOK_NUM_ELEMENTS(expected), generator.getCallList()));
  }

  const Callback oul = CALLBACK_OPEN_UNORDERED_LIST_LEVEL;
  const Callback cul = CALLBACK_CLOSE_UNORDERED_LIST_LEVEL;
  const Callback ool = CALLBACK_OPEN_ORDERED_LIST_LEVEL;
  const Callback col = CALLBACK_CLOSE_ORDERED_LIST_LEVEL;
  const Callback ole = CALLBACK_OPEN_LIST_ELEMENT;
  const Callback cle = CALLBACK_CLOSE_LIST_ELEMENT;

  {
    const Callback expected[] = {sd, ops, oul, ole, cle, cul, cps, ed};

    RawGenerator generator;
    XMLCollector collector(&generator);

    collector.openPageSpan(RVNGPropertyList());
    collector.openUnorderedList(RVNGPropertyList());
    collector.openListElement(RVNGPropertyList());
    collector.closeListElement();
    collector.closeUnorderedList();
    collector.closePageSpan();
    collector.finish();

    CPPUNIT_ASSERT_MESSAGE("empty list", generator.isBalanced());
    CPPUNIT_ASSERT_MESSAGE("empty list",
                           equal(expected, EBOOK_NUM_ELEMENTS(expected), generator.getCallList()));
  }

  {
    const Callback expected[] = {sd, ops, ool, ole, op, os, is, cs, cp, cle, col, cps, ed};

    RawGenerator generator;
    XMLCollector collector(&generator);

    collector.openPageSpan(RVNGPropertyList());
    collector.openOrderedList(RVNGPropertyList());
    collector.openListElement(RVNGPropertyList());
    collector.openParagraph(RVNGPropertyList());
    collector.openSpan(RVNGPropertyList());
    collector.collectSpace();
    collector.closeSpan();
    collector.closeParagraph();
    collector.closeListElement();
    collector.closeOrderedList();
    collector.closePageSpan();
    collector.finish();

    CPPUNIT_ASSERT_MESSAGE("list", generator.isBalanced());
    CPPUNIT_ASSERT_MESSAGE("list",
                           equal(expected, EBOOK_NUM_ELEMENTS(expected), generator.getCallList()));
  }

  {
    const Callback expected[] = {sd, ops, ool, ole, oul, ole, cle, cul, cle, col, cps, ed};

    RawGenerator generator;
    XMLCollector collector(&generator);

    collector.openPageSpan(RVNGPropertyList());
    collector.openOrderedList(RVNGPropertyList());
    collector.openListElement(RVNGPropertyList());
    collector.openUnorderedList(RVNGPropertyList());
    collector.openListElement(RVNGPropertyList());
    collector.closeListElement();
    collector.closeUnorderedList();
    collector.closeListElement();
    collector.closeOrderedList();
    collector.closePageSpan();
    collector.finish();

    CPPUNIT_ASSERT_MESSAGE("nested list", generator.isBalanced());
    CPPUNIT_ASSERT_MESSAGE("nested list",
                           equal(expected, EBOOK_NUM_ELEMENTS(expected), generator.getCallList()));
  }

  const Callback ot = CALLBACK_OPEN_TABLE;
  const Callback ct = CALLBACK_CLOSE_TABLE;
  const Callback otr = CALLBACK_OPEN_TABLE_ROW;
  const Callback ctr = CALLBACK_CLOSE_TABLE_ROW;
  const Callback otc = CALLBACK_OPEN_TABLE_CELL;
  const Callback ctc = CALLBACK_CLOSE_TABLE_CELL;
  const Callback icc = CALLBACK_INSERT_COVERED_TABLE_CELL;

  {
    const Callback expected[] = {sd, ops, ot, otr, otc, ctc, ctr, ct, cps, ed};

    RawGenerator generator;
    XMLCollector collector(&generator);

    collector.openPageSpan(RVNGPropertyList());
    collector.openTable(RVNGPropertyList());
    collector.openTableRow(RVNGPropertyList());
    collector.openTableCell(RVNGPropertyList());
    collector.closeTableCell();
    collector.closeTableRow();
    collector.closeTable();
    collector.closePageSpan();
    collector.finish();

    CPPUNIT_ASSERT_MESSAGE("empty table", generator.isBalanced());
    CPPUNIT_ASSERT_MESSAGE("empty table",
                           equal(expected, EBOOK_NUM_ELEMENTS(expected), generator.getCallList()));
  }

  {
    const Callback expected[] = {sd, ops, ot, otr, otc, op, cp, ctc, icc, ctr, otr, icc, icc, ctr, ct, cps, ed};

    RawGenerator generator;
    XMLCollector collector(&generator);

    collector.openPageSpan(RVNGPropertyList());
    collector.openTable(RVNGPropertyList());
    collector.openTableRow(RVNGPropertyList());
    collector.openTableCell(RVNGPropertyList());
    collector.openParagraph(RVNGPropertyList());
    collector.closeParagraph();
    collector.closeTableCell();
    collector.collectCoveredTableCell(RVNGPropertyList());
    collector.closeTableRow();
    collector.openTableRow(RVNGPropertyList());
    collector.collectCoveredTableCell(RVNGPropertyList());
    collector.collectCoveredTableCell(RVNGPropertyList());
    collector.closeTableRow();
    collector.closeTable();
    collector.closePageSpan();
    collector.finish();

    CPPUNIT_ASSERT_MESSAGE("table", generator.isBalanced());
    CPPUNIT_ASSERT_MESSAGE("table",
                           equal(expected, EBOOK_NUM_ELEMENTS(expected), generator.getCallList()));
  }
}

void XMLCollectorTest::testMissingCalls()
{
  const Callback sd = CALLBACK_START_DOCUMENT;
  const Callback ed = CALLBACK_END_DOCUMENT;
  const Callback ops = CALLBACK_OPEN_PAGE_SPAN;
  const Callback cps = CALLBACK_CLOSE_PAGE_SPAN;
  const Callback op = CALLBACK_OPEN_PARAGRAPH;
  const Callback cp = CALLBACK_CLOSE_PARAGRAPH;
  const Callback os = CALLBACK_OPEN_SPAN;
  const Callback cs = CALLBACK_CLOSE_SPAN;
  const Callback is = CALLBACK_INSERT_SPACE;

  {
    const Callback expected[] = {sd, ops, op, os, is, cs, cp, cps, ed};

    RawGenerator generator;
    XMLCollector collector(&generator);

    collector.openParagraph(RVNGPropertyList());
    collector.collectSpace();
    collector.closeParagraph();
    collector.finish();

    CPPUNIT_ASSERT_MESSAGE("text in para", generator.isBalanced());
    CPPUNIT_ASSERT_MESSAGE("text in para",
                           equal(expected, EBOOK_NUM_ELEMENTS(expected), generator.getCallList()));
  }

  {
    const Callback expected[] = {sd, ops, op, os, is, cs, cp, cps, ed};

    RawGenerator generator;
    XMLCollector collector(&generator);

    collector.openSpan(RVNGPropertyList());
    collector.collectSpace();
    collector.closeSpan();
    collector.finish();

    CPPUNIT_ASSERT_MESSAGE("text in span", generator.isBalanced());
    CPPUNIT_ASSERT_MESSAGE("text in span",
                           equal(expected, EBOOK_NUM_ELEMENTS(expected), generator.getCallList()));
  }

  {
    const Callback expected[] = {sd, ops, op, os, is, cs, cp, cps, ed};

    RawGenerator generator;
    XMLCollector collector(&generator);

    collector.collectSpace();
    collector.finish();

    CPPUNIT_ASSERT_MESSAGE("just text", generator.isBalanced());
    CPPUNIT_ASSERT_MESSAGE("just text",
                           equal(expected, EBOOK_NUM_ELEMENTS(expected), generator.getCallList()));
  }
}

void XMLCollectorTest::testMultipleNesting()
{
  const Callback sd = CALLBACK_START_DOCUMENT;
  const Callback ed = CALLBACK_END_DOCUMENT;
  const Callback op = CALLBACK_OPEN_PARAGRAPH;
  const Callback cp = CALLBACK_CLOSE_PARAGRAPH;
  const Callback ops = CALLBACK_OPEN_PAGE_SPAN;
  const Callback cps = CALLBACK_CLOSE_PAGE_SPAN;
  const Callback os = CALLBACK_OPEN_SPAN;
  const Callback cs = CALLBACK_CLOSE_SPAN;
  const Callback is = CALLBACK_INSERT_SPACE;
  const Callback it = CALLBACK_INSERT_TAB;
  const Callback il = CALLBACK_INSERT_LINE_BREAK;

  {
    const Callback expected[] = {sd, ops, op, os, is, cs, cp, cps, ed};

    RawGenerator generator;
    XMLCollector collector(&generator);

    collector.openParagraph(RVNGPropertyList());
    collector.openSpan(RVNGPropertyList());
    collector.openSpan(RVNGPropertyList());
    collector.collectSpace();
    collector.closeSpan();
    collector.closeSpan();
    collector.closeParagraph();
    collector.finish();

    CPPUNIT_ASSERT_MESSAGE("completely nested", generator.isBalanced());
    CPPUNIT_ASSERT_MESSAGE("completely nested",
                           equal(expected, EBOOK_NUM_ELEMENTS(expected), generator.getCallList()));
  }

  {
    const Callback expected[] = {sd, ops, op, os, it, cs, os, is, cs, cp, cps, ed};

    RawGenerator generator;
    XMLCollector collector(&generator);

    collector.openParagraph(RVNGPropertyList());
    collector.openSpan(RVNGPropertyList());
    collector.collectTab();
    collector.openSpan(RVNGPropertyList());
    collector.collectSpace();
    collector.closeSpan();
    collector.closeSpan();
    collector.closeParagraph();
    collector.finish();

    CPPUNIT_ASSERT_MESSAGE("text before", generator.isBalanced());
    CPPUNIT_ASSERT_MESSAGE("text before",
                           equal(expected, EBOOK_NUM_ELEMENTS(expected), generator.getCallList()));
  }

  {
    const Callback expected[] = {sd, ops, op, os, is, cs, os, it, cs, cp, cps, ed};

    RawGenerator generator;
    XMLCollector collector(&generator);

    collector.openParagraph(RVNGPropertyList());
    collector.openSpan(RVNGPropertyList());
    collector.openSpan(RVNGPropertyList());
    collector.collectSpace();
    collector.closeSpan();
    collector.collectTab();
    collector.closeSpan();
    collector.closeParagraph();
    collector.finish();

    CPPUNIT_ASSERT_MESSAGE("text after", generator.isBalanced());
    CPPUNIT_ASSERT_MESSAGE("text after",
                           equal(expected, EBOOK_NUM_ELEMENTS(expected), generator.getCallList()));
  }

  {
    const Callback expected[] = {sd, ops, op, os, it, cs, os, is, cs, os, il, cs, cp, cps, ed};

    RawGenerator generator;
    XMLCollector collector(&generator);

    collector.openParagraph(RVNGPropertyList());
    collector.openSpan(RVNGPropertyList());
    collector.collectTab();
    collector.openSpan(RVNGPropertyList());
    collector.collectSpace();
    collector.closeSpan();
    collector.collectLineBreak();
    collector.closeSpan();
    collector.closeParagraph();
    collector.finish();

    CPPUNIT_ASSERT_MESSAGE("text before and after", generator.isBalanced());
    CPPUNIT_ASSERT_MESSAGE("text before and after",
                           equal(expected, EBOOK_NUM_ELEMENTS(expected), generator.getCallList()));
  }
}

void XMLCollectorTest::testIncorrectNesting()
{
  const Callback sd = CALLBACK_START_DOCUMENT;
  const Callback ed = CALLBACK_END_DOCUMENT;
  const Callback op = CALLBACK_OPEN_PARAGRAPH;
  const Callback ops = CALLBACK_OPEN_PAGE_SPAN;
  const Callback cps = CALLBACK_CLOSE_PAGE_SPAN;
  const Callback cp = CALLBACK_CLOSE_PARAGRAPH;
  const Callback os = CALLBACK_OPEN_SPAN;
  const Callback cs = CALLBACK_CLOSE_SPAN;
  const Callback is = CALLBACK_INSERT_SPACE;
  const Callback it = CALLBACK_INSERT_TAB;
  const Callback il = CALLBACK_INSERT_LINE_BREAK;

  {
    const Callback expected[] = {sd, ops, op, os, is, cs, cp, cps, ed};

    RawGenerator generator;
    XMLCollector collector(&generator);

    collector.openParagraph(RVNGPropertyList());
    collector.openSpan(RVNGPropertyList());
    collector.openParagraph(RVNGPropertyList());
    collector.openSpan(RVNGPropertyList());
    collector.collectSpace();
    collector.closeSpan();
    collector.closeParagraph();
    collector.closeSpan();
    collector.closeParagraph();
    collector.finish();

    CPPUNIT_ASSERT_MESSAGE("completely nested", generator.isBalanced());
    CPPUNIT_ASSERT_MESSAGE("completely nested",
                           equal(expected, EBOOK_NUM_ELEMENTS(expected), generator.getCallList()));
  }

  {
    const Callback expected[] = {sd, ops, op, os, it, cs, cp, op, os, is, cs, cp, cps, ed};

    RawGenerator generator;
    XMLCollector collector(&generator);

    collector.openParagraph(RVNGPropertyList());
    collector.openSpan(RVNGPropertyList());
    collector.collectTab();
    collector.openParagraph(RVNGPropertyList());
    collector.openSpan(RVNGPropertyList());
    collector.collectSpace();
    collector.closeSpan();
    collector.closeParagraph();
    collector.closeSpan();
    collector.closeParagraph();
    collector.finish();

    CPPUNIT_ASSERT_MESSAGE("text before", generator.isBalanced());
    CPPUNIT_ASSERT_MESSAGE("text before",
                           equal(expected, EBOOK_NUM_ELEMENTS(expected), generator.getCallList()));
  }

  {
    const Callback expected[] = {sd, ops, op, os, is, cs, cp, op, os, it, cs, cp, cps, ed};

    RawGenerator generator;
    XMLCollector collector(&generator);

    collector.openParagraph(RVNGPropertyList());
    collector.openSpan(RVNGPropertyList());
    collector.openParagraph(RVNGPropertyList());
    collector.openSpan(RVNGPropertyList());
    collector.collectSpace();
    collector.closeSpan();
    collector.closeParagraph();
    collector.collectTab();
    collector.closeSpan();
    collector.closeParagraph();
    collector.finish();

    CPPUNIT_ASSERT_MESSAGE("text after", generator.isBalanced());
    CPPUNIT_ASSERT_MESSAGE("text after",
                           equal(expected, EBOOK_NUM_ELEMENTS(expected), generator.getCallList()));
  }

  {
    const Callback expected[] = {sd, ops, op, os, it, cs, cp, op, os, is, cs, cp, op, os, il, cs, cp, cps, ed};

    RawGenerator generator;
    XMLCollector collector(&generator);

    collector.openParagraph(RVNGPropertyList());
    collector.openSpan(RVNGPropertyList());
    collector.collectTab();
    collector.openParagraph(RVNGPropertyList());
    collector.openSpan(RVNGPropertyList());
    collector.collectSpace();
    collector.closeSpan();
    collector.closeParagraph();
    collector.collectLineBreak();
    collector.closeSpan();
    collector.closeParagraph();
    collector.finish();

    CPPUNIT_ASSERT_MESSAGE("text before and after", generator.isBalanced());
    CPPUNIT_ASSERT_MESSAGE("text before and after",
                           equal(expected, EBOOK_NUM_ELEMENTS(expected), generator.getCallList()));
  }
}

CPPUNIT_TEST_SUITE_REGISTRATION(XMLCollectorTest);

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
