/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef EBOOKSUBDOCUMENT_H_INCLUDED
#define EBOOKSUBDOCUMENT_H_INCLUDED

#include <librevenge/librevenge.h>

namespace libebook
{

class EBOOKSubDocument : public librevenge::RVNGTextInterface
{
  // disable copying
  EBOOKSubDocument(const EBOOKSubDocument &);
  EBOOKSubDocument &operator=(const EBOOKSubDocument &);

public:
  explicit EBOOKSubDocument(librevenge::RVNGTextInterface &document);
  ~EBOOKSubDocument() override;

  void setDocumentMetaData(const librevenge::RVNGPropertyList &propList) override;

  void startDocument(const librevenge::RVNGPropertyList &propList) override;
  void endDocument() override;

  void defineEmbeddedFont(const librevenge::RVNGPropertyList &propList) override;

  void definePageStyle(const librevenge::RVNGPropertyList &propList) override;
  void openPageSpan(const librevenge::RVNGPropertyList &propList) override;
  void closePageSpan() override;
  void openHeader(const librevenge::RVNGPropertyList &propList) override;
  void closeHeader() override;
  void openFooter(const librevenge::RVNGPropertyList &propList) override;
  void closeFooter() override;

  void defineParagraphStyle(const librevenge::RVNGPropertyList &propList) override;
  void openParagraph(const librevenge::RVNGPropertyList &propList) override;
  void closeParagraph() override;

  void defineCharacterStyle(const librevenge::RVNGPropertyList &propList) override;
  void openSpan(const librevenge::RVNGPropertyList &propList) override;
  void closeSpan() override;

  void openLink(const librevenge::RVNGPropertyList &propList) override;
  void closeLink() override;

  void defineSectionStyle(const librevenge::RVNGPropertyList &propList) override;
  void openSection(const librevenge::RVNGPropertyList &propList) override;
  void closeSection() override;

  void insertTab() override;
  void insertSpace() override;
  void insertText(const librevenge::RVNGString &text) override;
  void insertLineBreak() override;
  void insertField(const librevenge::RVNGPropertyList &propList) override;

  void openOrderedListLevel(const librevenge::RVNGPropertyList &propList) override;
  void openUnorderedListLevel(const librevenge::RVNGPropertyList &propList) override;
  void closeOrderedListLevel() override;
  void closeUnorderedListLevel() override;
  void openListElement(const librevenge::RVNGPropertyList &propList) override;
  void closeListElement() override;

  void openFootnote(const librevenge::RVNGPropertyList &propList) override;
  void closeFootnote() override;
  void openEndnote(const librevenge::RVNGPropertyList &propList) override;
  void closeEndnote() override;
  void openComment(const librevenge::RVNGPropertyList &propList) override;
  void closeComment() override;
  void openTextBox(const librevenge::RVNGPropertyList &propList) override;
  void closeTextBox() override;

  void openTable(const librevenge::RVNGPropertyList &propList) override;
  void openTableRow(const librevenge::RVNGPropertyList &propList) override;
  void closeTableRow() override;
  void openTableCell(const librevenge::RVNGPropertyList &propList) override;
  void closeTableCell() override;
  void insertCoveredTableCell(const librevenge::RVNGPropertyList &propList) override;
  void closeTable() override;

  void openFrame(const librevenge::RVNGPropertyList &propList) override;
  void closeFrame() override;

  void openGroup(const librevenge::RVNGPropertyList &propList) override;
  void closeGroup() override;

  void defineGraphicStyle(const librevenge::RVNGPropertyList &propList) override;
  void drawRectangle(const librevenge::RVNGPropertyList &propList) override;
  void drawEllipse(const librevenge::RVNGPropertyList &propList) override;
  void drawPolygon(const librevenge::RVNGPropertyList &propList) override;
  void drawPolyline(const librevenge::RVNGPropertyList &propList) override;
  void drawPath(const librevenge::RVNGPropertyList &propList) override;
  void drawConnector(const librevenge::RVNGPropertyList &propList) override;

  void insertBinaryObject(const librevenge::RVNGPropertyList &propList) override;
  void insertEquation(const librevenge::RVNGPropertyList &propList) override;

private:
  librevenge::RVNGTextInterface &m_document;
};

}

#endif // EBOOKSUBDOCUMENT_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
