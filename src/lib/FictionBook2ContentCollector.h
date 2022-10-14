/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef FICTIONBOOK2CONTENTCOLLECTOR_H_INCLUDED
#define FICTIONBOOK2CONTENTCOLLECTOR_H_INCLUDED

#include "EBOOKLanguageManager.h"
#include "FictionBook2Collector.h"

namespace libebook
{

class FictionBook2ContentCollector : public FictionBook2Collector
{
  // no copying
  FictionBook2ContentCollector(const FictionBook2ContentCollector &other);
  FictionBook2ContentCollector &operator=(const FictionBook2ContentCollector &other);

public:
  FictionBook2ContentCollector(librevenge::RVNGTextInterface *m_document, const librevenge::RVNGPropertyList &metadata, const NoteMap_t &notes, const BinaryMap_t &bitmaps);

private:
  void defineMetadataEntry(const char *name, const char *value) override;
  void openMetadataEntry(const char *name) override;
  void closeMetadataEntry() override;

  void defineID(const char *id) override;

  void openPageSpan() override;
  void closePageSpan() override;

  void openBlock() override;
  void closeBlock() override;

  void openParagraph(const FictionBook2BlockFormat &format) override;
  void closeParagraph() override;

  void openSpan(const FictionBook2Style &style) override;
  void closeSpan() override;

  void insertText(const char *text) override;

  void openTable(const FictionBook2BlockFormat &format) override;
  void closeTable() override;

  void openTableRow(const FictionBook2BlockFormat &format) override;
  void closeTableRow() override;

  void openTableCell(int rowspan, int colspan) override;
  void closeTableCell() override;
  void insertCoveredTableCell() override;

  void insertFootnote(const char *id) override;

  void insertBitmap(const char *id) override;
  void insertBitmapData(const char *contentType, const char *base64Data) override;

private:
  librevenge::RVNGTextInterface *m_document;
  const librevenge::RVNGPropertyList &m_metadata;
  const NoteMap_t &m_notes;
  const BinaryMap_t &m_bitmaps;
  EBOOKLanguageManager m_languageManager;
  int m_currentFootnote;
  int m_openPara;
};

}

#endif // FICTIONBOOK2CONTENTCOLLECTOR_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
