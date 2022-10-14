/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "EBOOKSubDocument.h"

namespace libebook
{

EBOOKSubDocument::EBOOKSubDocument(librevenge::RVNGTextInterface &document)
  : m_document(document)
{
}

EBOOKSubDocument::~EBOOKSubDocument()
{
}

void EBOOKSubDocument::setDocumentMetaData(const librevenge::RVNGPropertyList &)
{
}

void EBOOKSubDocument::startDocument(const librevenge::RVNGPropertyList &)
{
}

void EBOOKSubDocument::endDocument()
{
}

void EBOOKSubDocument::defineEmbeddedFont(const librevenge::RVNGPropertyList &propList)
{
  m_document.defineEmbeddedFont(propList);
}

void EBOOKSubDocument::definePageStyle(const librevenge::RVNGPropertyList &propList)
{
  m_document.definePageStyle(propList);
}

void EBOOKSubDocument::openPageSpan(const librevenge::RVNGPropertyList &propList)
{
  m_document.openPageSpan(propList);
}

void EBOOKSubDocument::closePageSpan()
{
  m_document.closePageSpan();
}

void EBOOKSubDocument::openHeader(const librevenge::RVNGPropertyList &propList)
{
  m_document.openHeader(propList);
}

void EBOOKSubDocument::closeHeader()
{
  m_document.closeHeader();
}

void EBOOKSubDocument::openFooter(const librevenge::RVNGPropertyList &propList)
{
  m_document.openFooter(propList);
}

void EBOOKSubDocument::closeFooter()
{
  m_document.closeFooter();
}

void EBOOKSubDocument::defineParagraphStyle(const librevenge::RVNGPropertyList &propList)
{
  m_document.defineParagraphStyle(propList);
}

void EBOOKSubDocument::openParagraph(const librevenge::RVNGPropertyList &propList)
{
  m_document.openParagraph(propList);
}

void EBOOKSubDocument::closeParagraph()
{
  m_document.closeParagraph();
}

void EBOOKSubDocument::defineCharacterStyle(const librevenge::RVNGPropertyList &propList)
{
  m_document.defineCharacterStyle(propList);
}

void EBOOKSubDocument::openSpan(const librevenge::RVNGPropertyList &propList)
{
  m_document.openSpan(propList);
}

void EBOOKSubDocument::closeSpan()
{
  m_document.closeSpan();
}

void EBOOKSubDocument::openLink(const librevenge::RVNGPropertyList &propList)
{
  m_document.openLink(propList);
}

void EBOOKSubDocument::closeLink()
{
  m_document.closeLink();
}

void EBOOKSubDocument::defineSectionStyle(const librevenge::RVNGPropertyList &propList)
{
  m_document.defineSectionStyle(propList);
}

void EBOOKSubDocument::openSection(const librevenge::RVNGPropertyList &propList)
{
  m_document.openSection(propList);
}

void EBOOKSubDocument::closeSection()
{
  m_document.closeSection();
}

void EBOOKSubDocument::insertTab()
{
  m_document.insertTab();
}

void EBOOKSubDocument::insertSpace()
{
  m_document.insertSpace();
}

void EBOOKSubDocument::insertText(const librevenge::RVNGString &text)
{
  m_document.insertText(text);
}

void EBOOKSubDocument::insertLineBreak()
{
  m_document.insertLineBreak();
}

void EBOOKSubDocument::insertField(const librevenge::RVNGPropertyList &propList)
{
  m_document.insertField(propList);
}

void EBOOKSubDocument::openOrderedListLevel(const librevenge::RVNGPropertyList &propList)
{
  m_document.openOrderedListLevel(propList);
}

void EBOOKSubDocument::openUnorderedListLevel(const librevenge::RVNGPropertyList &propList)
{
  m_document.openUnorderedListLevel(propList);
}

void EBOOKSubDocument::closeOrderedListLevel()
{
  m_document.closeOrderedListLevel();
}

void EBOOKSubDocument::closeUnorderedListLevel()
{
  m_document.closeUnorderedListLevel();
}

void EBOOKSubDocument::openListElement(const librevenge::RVNGPropertyList &propList)
{
  m_document.openListElement(propList);
}

void EBOOKSubDocument::closeListElement()
{
  m_document.closeListElement();
}

void EBOOKSubDocument::openFootnote(const librevenge::RVNGPropertyList &propList)
{
  m_document.openFootnote(propList);
}

void EBOOKSubDocument::closeFootnote()
{
  m_document.closeFootnote();
}

void EBOOKSubDocument::openEndnote(const librevenge::RVNGPropertyList &propList)
{
  m_document.openEndnote(propList);
}

void EBOOKSubDocument::closeEndnote()
{
  m_document.closeEndnote();
}

void EBOOKSubDocument::openComment(const librevenge::RVNGPropertyList &propList)
{
  m_document.openComment(propList);
}

void EBOOKSubDocument::closeComment()
{
  m_document.closeComment();
}

void EBOOKSubDocument::openTextBox(const librevenge::RVNGPropertyList &propList)
{
  m_document.openTextBox(propList);
}

void EBOOKSubDocument::closeTextBox()
{
  m_document.closeTextBox();
}

void EBOOKSubDocument::openTable(const librevenge::RVNGPropertyList &propList)
{
  m_document.openTable(propList);
}

void EBOOKSubDocument::openTableRow(const librevenge::RVNGPropertyList &propList)
{
  m_document.openTableRow(propList);
}

void EBOOKSubDocument::closeTableRow()
{
  m_document.closeTableRow();
}

void EBOOKSubDocument::openTableCell(const librevenge::RVNGPropertyList &propList)
{
  m_document.openTableCell(propList);
}

void EBOOKSubDocument::closeTableCell()
{
  m_document.closeTableCell();
}

void EBOOKSubDocument::insertCoveredTableCell(const librevenge::RVNGPropertyList &propList)
{
  m_document.insertCoveredTableCell(propList);
}

void EBOOKSubDocument::closeTable()
{
  m_document.closeTable();
}

void EBOOKSubDocument::openFrame(const librevenge::RVNGPropertyList &propList)
{
  m_document.openFrame(propList);
}

void EBOOKSubDocument::closeFrame()
{
  m_document.closeFrame();
}

void EBOOKSubDocument::openGroup(const librevenge::RVNGPropertyList &propList)
{
  m_document.openGroup(propList);
}

void EBOOKSubDocument::closeGroup()
{
  m_document.closeGroup();
}

void EBOOKSubDocument::defineGraphicStyle(const librevenge::RVNGPropertyList &propList)
{
  m_document.defineGraphicStyle(propList);
}

void EBOOKSubDocument::drawRectangle(const librevenge::RVNGPropertyList &propList)
{
  m_document.drawRectangle(propList);
}

void EBOOKSubDocument::drawEllipse(const librevenge::RVNGPropertyList &propList)
{
  m_document.drawEllipse(propList);
}

void EBOOKSubDocument::drawPolygon(const librevenge::RVNGPropertyList &propList)
{
  m_document.drawPolygon(propList);
}

void EBOOKSubDocument::drawPolyline(const librevenge::RVNGPropertyList &propList)
{
  m_document.drawPolyline(propList);
}

void EBOOKSubDocument::drawPath(const librevenge::RVNGPropertyList &propList)
{
  m_document.drawPath(propList);
}

void EBOOKSubDocument::drawConnector(const librevenge::RVNGPropertyList &propList)
{
  m_document.drawConnector(propList);
}

void EBOOKSubDocument::insertBinaryObject(const librevenge::RVNGPropertyList &propList)
{
  m_document.insertBinaryObject(propList);
}

void EBOOKSubDocument::insertEquation(const librevenge::RVNGPropertyList &propList)
{
  m_document.insertEquation(propList);
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
