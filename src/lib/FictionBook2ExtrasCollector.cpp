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
#include <string>
#include <utility>

#include "libebook_utils.h"

#include "FictionBook2ExtrasCollector.h"
#include "FictionBook2Style.h"

using std::make_pair;
using std::string;

namespace libebook
{

FictionBook2ExtrasCollector::FictionBook2ExtrasCollector(NoteMap_t &notes, BinaryMap_t &bitmaps)
  : m_notes(notes)
  , m_bitmaps(bitmaps)
  , m_currentID()
  , m_currentNote()
  , m_ignored(false)
  , m_title(false)
  , m_currentText()
{
}

FictionBook2ExtrasCollector::~FictionBook2ExtrasCollector()
{
  flushCurrentNote();
}

void FictionBook2ExtrasCollector::defineMetadataEntry(const char *, const char *)
{
}

void FictionBook2ExtrasCollector::openMetadataEntry(const char *)
{
}

void FictionBook2ExtrasCollector::closeMetadataEntry()
{
}

void FictionBook2ExtrasCollector::defineID(const char *id)
{
  flushCurrentNote();

  m_currentID = id;
}

void FictionBook2ExtrasCollector::openPageSpan()
{
}

void FictionBook2ExtrasCollector::closePageSpan()
{
}

void FictionBook2ExtrasCollector::openBlock()
{
  // TODO: implement me
}

void FictionBook2ExtrasCollector::closeBlock()
{
  // TODO: implement me
}

void FictionBook2ExtrasCollector::openParagraph(const FictionBook2BlockFormat &format)
{
  if (format.headingLevel == 1)
    m_ignored = true;
  if (format.headingLevel > 1)
    m_title = true;
  else if (format.headingLevel == 0)
    m_currentNote.m_paras.push_back(Paragraph(format));
}

void FictionBook2ExtrasCollector::closeParagraph()
{
  m_ignored = false;
  m_title = false;
}

void FictionBook2ExtrasCollector::openSpan(const FictionBook2Style &style)
{
  if (m_ignored || m_title)
    return;
  assert(!m_currentNote.m_paras.empty());
  m_currentNote.m_paras.back().m_spans.push_back(Span(style));
}

void FictionBook2ExtrasCollector::closeSpan()
{
  if (m_ignored)
    return;
  if (m_title)
  {
    m_currentNote.m_title = m_currentText;
  }
  else
  {
    assert(!m_currentNote.m_paras.empty());
    assert(!m_currentNote.m_paras.back().m_spans.empty());
    m_currentNote.m_paras.back().m_spans.back().m_text = m_currentText;
// avoid empty spans
    std::deque<Span> &spans = m_currentNote.m_paras.back().m_spans;
    if (spans.back().m_text.empty())
      spans.pop_back();
  }
  m_currentText.clear();
}

void FictionBook2ExtrasCollector::insertText(const char *text)
{
  if (m_ignored)
    return;
  m_currentText.append(text);
}

void FictionBook2ExtrasCollector::openTable(const FictionBook2BlockFormat &format)
{
  // TODO: implement me
  (void) format;
}

void FictionBook2ExtrasCollector::closeTable()
{
  // TODO: implement me
}

void FictionBook2ExtrasCollector::openTableRow(const FictionBook2BlockFormat &format)
{
  // TODO: implement me
  (void) format;
}

void FictionBook2ExtrasCollector::closeTableRow()
{
  // TODO: implement me
}

void FictionBook2ExtrasCollector::openTableCell(int rowspan, int colspan)
{
  // TODO: implement me
  (void) rowspan;
  (void) colspan;
}

void FictionBook2ExtrasCollector::closeTableCell()
{
  // TODO: implement me
}

void FictionBook2ExtrasCollector::insertCoveredTableCell()
{
  // TODO: implement me
}

void FictionBook2ExtrasCollector::insertFootnote(const char *)
{
}

void FictionBook2ExtrasCollector::insertBitmap(const char *)
{
}

void FictionBook2ExtrasCollector::insertBitmapData(const char *const contentType, const char *const base64Data)
{
  if (!m_currentID.empty())
  {
    m_bitmaps.insert(make_pair(m_currentID, Binary(contentType, base64Data)));
    m_currentID.clear();
  }
}

void FictionBook2ExtrasCollector::flushCurrentNote()
{
  if (!m_currentID.empty())
  {
    m_notes.insert(make_pair(m_currentID, m_currentNote));
    m_currentID.clear();
    m_currentNote = Note();
  }
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
