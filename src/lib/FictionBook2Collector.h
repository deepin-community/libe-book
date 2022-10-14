/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef FICTIONBOOK2COLLECTOR_H_INCLUDED
#define FICTIONBOOK2COLLECTOR_H_INCLUDED

#include <deque>
#include <string>
#include <unordered_map>

#include "FictionBook2Style.h"

namespace libebook
{

class FictionBook2Collector
{
public:
  struct Binary
  {
    Binary(const std::string &contentType, const std::string &base64Data);

    const std::string m_contentType;
    const std::string m_data;
  };

  struct Span
  {
    explicit Span(const FictionBook2Style &style);

    const FictionBook2Style m_style;
    std::string m_text;
  };

  struct Paragraph
  {
    explicit Paragraph(const FictionBook2BlockFormat &format);

    const FictionBook2BlockFormat m_format;
    std::deque<Span> m_spans;
  };

  struct Note
  {
    Note();

    std::string m_title;
    std::deque<Paragraph> m_paras;
  };

  typedef std::unordered_map<std::string, Binary> BinaryMap_t;
  typedef std::unordered_map<std::string, Note> NoteMap_t;

public:
  virtual ~FictionBook2Collector() = 0;

  virtual void defineMetadataEntry(const char *name, const char *value) = 0;
  virtual void openMetadataEntry(const char *name) = 0;
  virtual void closeMetadataEntry() = 0;

  virtual void defineID(const char *id) = 0;

  virtual void openPageSpan() = 0;
  virtual void closePageSpan() = 0;

  virtual void openBlock() = 0;
  virtual void closeBlock() = 0;

  virtual void openParagraph(const FictionBook2BlockFormat &format) = 0;
  virtual void closeParagraph() = 0;

  virtual void openSpan(const FictionBook2Style &style) = 0;
  virtual void closeSpan() = 0;

  virtual void insertText(const char *text) = 0;

  virtual void openTable(const FictionBook2BlockFormat &format) = 0;
  virtual void closeTable() = 0;

  virtual void openTableRow(const FictionBook2BlockFormat &format) = 0;
  virtual void closeTableRow() = 0;

  virtual void openTableCell(int rowspan, int colspan) = 0;
  virtual void closeTableCell() = 0;
  virtual void insertCoveredTableCell() = 0;

  virtual void insertFootnote(const char *id) = 0;

  virtual void insertBitmap(const char *id) = 0;
  virtual void insertBitmapData(const char *contentType, const char *base64Data) = 0;
};

}

#endif // FICTIONBOOK2COLLECTOR_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
