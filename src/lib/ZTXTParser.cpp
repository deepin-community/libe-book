/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <librevenge/librevenge.h>
#include <librevenge-stream/librevenge-stream.h>

#include "libebook_utils.h"
#include "EBOOKZlibStream.h"
#include "ZTXTParser.h"

using std::unique_ptr;

namespace libebook
{

static const uint32_t ZTXT_TYPE = PDB_CODE("zTXT");
static const uint32_t ZTXT_CREATOR = PDB_CODE("GPlm");

ZTXTParser::ZTXTParser(librevenge::RVNGInputStream *const input, librevenge::RVNGTextInterface *const document)
  : PDBParser(input, document, ZTXT_TYPE, ZTXT_CREATOR)
  , m_recordCount(0)
  , m_size(0)
  , m_recordSize(0)
{
}

bool ZTXTParser::checkType(const unsigned type, const unsigned creator)
{
  return (ZTXT_TYPE == type) && (ZTXT_CREATOR == creator);
}

void ZTXTParser::readAppInfoRecord(librevenge::RVNGInputStream *)
{
}

void ZTXTParser::readSortInfoRecord(librevenge::RVNGInputStream *)
{
}

void ZTXTParser::readIndexRecord(librevenge::RVNGInputStream *const record)
{
  readU16(record, true);
  m_recordCount = readU16(record, true);
  m_size = readU32(record, true);
  m_recordSize = readU16(record, true);
  // the rest is not interesting...
}

void ZTXTParser::readDataRecord(librevenge::RVNGInputStream *const record, bool)
{
  librevenge::RVNGString text;
  while (!record->isEnd())
  {
    const uint8_t c = readU8(record);
    if ('\n' == c)
    {
      handleText(text);
      text.clear();
    }
    else
      text.append(char(c));
  }

  if (0 < text.len())
    handleText(text);
}

void ZTXTParser::readDataRecords()
{
  openDocument();
  const unique_ptr<librevenge::RVNGInputStream> block(getDataRecords(0, m_recordCount - 1));
  if (bool(block))
  {
    EBOOKZlibStream input(block.get());
    readDataRecord(&input);
  }
  closeDocument();
}

void ZTXTParser::openDocument()
{
  getDocument()->startDocument(librevenge::RVNGPropertyList());

  librevenge::RVNGPropertyList metadata;
  metadata.insert("dc:title", librevenge::RVNGString(getName()));
  getDocument()->setDocumentMetaData(metadata);

  getDocument()->openPageSpan(getDefaultPageSpanPropList());
}

void ZTXTParser::closeDocument()
{
  getDocument()->closePageSpan();
  getDocument()->endDocument();
}

void ZTXTParser::handleText(const librevenge::RVNGString &text)
{
  getDocument()->openParagraph(librevenge::RVNGPropertyList());
  getDocument()->openSpan(librevenge::RVNGPropertyList());
  getDocument()->insertText(text);
  getDocument()->closeSpan();
  getDocument()->closeParagraph();
}

} // namespace libebook

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
