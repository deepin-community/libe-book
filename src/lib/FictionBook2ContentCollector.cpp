/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <librevenge/librevenge.h>

#include "EBOOKLanguageManager.h"
#include "FictionBook2ContentCollector.h"
#include "FictionBook2Style.h"

namespace libebook
{

using std::string;

FictionBook2ContentCollector::FictionBook2ContentCollector(
  librevenge::RVNGTextInterface *document, const librevenge::RVNGPropertyList &metadata,
  const NoteMap_t &notes, const BinaryMap_t &bitmaps)
  : m_document(document)
  , m_metadata(metadata)
  , m_notes(notes)
  , m_bitmaps(bitmaps)
  , m_languageManager()
  , m_currentFootnote(1)
  , m_openPara(false)
{
}

void FictionBook2ContentCollector::defineMetadataEntry(const char *, const char *)
{
}

void FictionBook2ContentCollector::openMetadataEntry(const char *)
{
}

void FictionBook2ContentCollector::closeMetadataEntry()
{
}

void FictionBook2ContentCollector::defineID(const char *)
{
}

void FictionBook2ContentCollector::openPageSpan()
{
  m_document->openPageSpan(getDefaultPageSpanPropList());
}

void FictionBook2ContentCollector::closePageSpan()
{
  m_document->closePageSpan();
}

void FictionBook2ContentCollector::openBlock()
{
}

void FictionBook2ContentCollector::closeBlock()
{
}

void FictionBook2ContentCollector::openParagraph(const FictionBook2BlockFormat &format)
{
  librevenge::RVNGPropertyList props(makePropertyList(format));
  if (0 < format.headingLevel)
  {
    librevenge::RVNGString name;
    name.sprintf("FictionBook2 Heading %u", unsigned(format.headingLevel));
    props.insert("style:display-name", name);
    props.insert("text:outline-level", format.headingLevel);
  }

  m_document->openParagraph(props);
  m_openPara = true;
}

void FictionBook2ContentCollector::closeParagraph()
{
  m_document->closeParagraph();
  m_openPara = false;
}

void FictionBook2ContentCollector::openSpan(const FictionBook2Style &style)
{
  librevenge::RVNGPropertyList props(makePropertyList(style));

  std::string lang;

  if (!style.getTextFormat().lang.empty())
    lang = style.getTextFormat().lang;
  else if (!style.getBlockFormat().lang.empty())
    lang = style.getBlockFormat().lang;
  else if (m_metadata["dc:language"])
    lang = m_metadata["dc:language"]->getStr().cstr();

  // TODO: improve
  string tag = m_languageManager.addTag(lang);
  if (tag.empty())
    tag = m_languageManager.addLanguage(lang);
  if (!tag.empty())
    m_languageManager.writeProperties(tag, props);

  m_document->openSpan(props);
}

void FictionBook2ContentCollector::closeSpan()
{
  m_document->closeSpan();
}

void FictionBook2ContentCollector::insertText(const char *text)
{
  m_document->insertText(librevenge::RVNGString(text));
}

void FictionBook2ContentCollector::openTable(const FictionBook2BlockFormat &format)
{
  m_document->openTable(makePropertyList(format));
}

void FictionBook2ContentCollector::closeTable()
{
  m_document->closeTable();
}

void FictionBook2ContentCollector::openTableRow(const FictionBook2BlockFormat &format)
{
  librevenge::RVNGPropertyList props;
  if (format.headerRow)
    props.insert("fo:is-header-row", true);

  m_document->openTableRow(props);
}

void FictionBook2ContentCollector::closeTableRow()
{
  m_document->closeTableRow();
}

void FictionBook2ContentCollector::openTableCell(int rowspan, int colspan)
{
  librevenge::RVNGPropertyList props;
  if (colspan > 0)
    props.insert("table:number-columns-spanned", colspan);
  if (rowspan > 0)
    props.insert("table:number-rows-spanned", rowspan);

  m_document->openTableCell(props);
}

void FictionBook2ContentCollector::closeTableCell()
{
  m_document->closeTableCell();
}

void FictionBook2ContentCollector::insertCoveredTableCell()
{
  m_document->insertCoveredTableCell(librevenge::RVNGPropertyList());
}

void FictionBook2ContentCollector::insertFootnote(const char *id)
{
  const NoteMap_t::const_iterator it = m_notes.find(id);
  if (it != m_notes.end())
  {
    librevenge::RVNGPropertyList props;
    props.insert("librevenge:number", m_currentFootnote);
    ++m_currentFootnote;
    const Note &note = it->second;
    if (!note.m_title.empty())
      props.insert("text:label", note.m_title.c_str());

    m_document->openFootnote(props);

    for (const auto &para : note.m_paras)
    {
      openParagraph(para.m_format);
      for (const auto &span : para.m_spans)
      {
        openSpan(span.m_style);
        insertText(span.m_text.c_str());
        closeSpan();
      }
      closeParagraph();
    }

    m_document->closeFootnote();
  }
}

void FictionBook2ContentCollector::insertBitmap(const char *id)
{
  const BinaryMap_t::const_iterator it = m_bitmaps.find(id);
  if (it != m_bitmaps.end())
  {
    librevenge::RVNGPropertyList props;

    if (m_openPara)
    {
      props.insert("style:horizontal-rel", "paragraph-content");
      props.insert("style:vertical-rel", "paragraph-content");
      props.insert("text:anchor-type", "character");
    }
    else
    {
      props.insert("style:horizontal-rel", "paragraph-start-margin");
      props.insert("style:vertical-rel", "paragraph-start-margin");
      props.insert("text:anchor-type", "paragraph");
    }
    props.insert("style:horizontal-pos", "left");
    props.insert("style:vertical-pos", "top");
    props.insert("style:wrap", "none");

    m_document->openFrame(props);
    insertBitmapData(it->second.m_contentType.c_str(), it->second.m_data.c_str());
    m_document->closeFrame();
  }
}

void FictionBook2ContentCollector::insertBitmapData(const char *contentType, const char *base64Data)
{
  librevenge::RVNGPropertyList props;
  props.insert("librevenge:mime-type", librevenge::RVNGString(contentType));

  const librevenge::RVNGBinaryData data(base64Data);
  props.insert("office:binary-data", data);

  m_document->insertBinaryObject(props);
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
