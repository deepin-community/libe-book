/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef FICTIONBOOK2EXTRASCOLLECTOR_H_INCLUDED
#define FICTIONBOOK2EXTRASCOLLECTOR_H_INCLUDED

#include "FictionBook2Collector.h"

namespace libebook
{

class FictionBook2Style;

class FictionBook2ExtrasCollector : public FictionBook2Collector
{
  // no copying
  FictionBook2ExtrasCollector(const FictionBook2ExtrasCollector &other);
  FictionBook2ExtrasCollector &operator=(const FictionBook2ExtrasCollector &other);

public:
  FictionBook2ExtrasCollector(NoteMap_t &notes, BinaryMap_t &bitmaps);
  ~FictionBook2ExtrasCollector() override;

private:
  void defineMetadataEntry(const char *name, const char *value) override;
  void openMetadataEntry(const char *name) override;
  void closeMetadataEntry() override;

  void defineID(const char *) override;

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

  void flushCurrentNote();

private:
  NoteMap_t &m_notes;
  BinaryMap_t &m_bitmaps;
  std::string m_currentID;
  Note m_currentNote;
  // TODO: this is a hack, but better way would require refactoring
  bool m_ignored;
  bool m_title;
  std::string m_currentText;
};

}

#endif // FICTIONBOOK2EXTRASCOLLECTOR_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
