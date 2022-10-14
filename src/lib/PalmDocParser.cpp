/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <algorithm>
#include <cassert>
#include <cstring>

#include "libebook_utils.h"
#include "EBOOKCharsetConverter.h"
#include "PalmDocParser.h"
#include "PDBLZ77Stream.h"

using std::string;
using std::vector;

namespace libebook
{

//static const unsigned PALMDOC_BLOCK_SIZE = 4096;

static const unsigned PALMDOC_TYPE = PDB_CODE("TEXt");
static const unsigned PALMDOC_CREATOR = PDB_CODE("REAd");

PalmDocParser::PalmDocParser(librevenge::RVNGInputStream *input, librevenge::RVNGTextInterface *document)
  : PDBParser(input, document, PALMDOC_TYPE, PALMDOC_CREATOR)
  , m_compressed(false)
  , m_textLength(0)
  , m_recordCount(0)
  , m_recordSize(0)
  , m_read(0)
  , m_openedParagraph(false)
  , m_openedDocument(false)
  , m_converter()
{
}

PalmDocParser::~PalmDocParser()
{
}

bool PalmDocParser::checkType(const unsigned type, const unsigned creator)
{
  return (PALMDOC_TYPE == type) && (PALMDOC_CREATOR == creator);
}

void PalmDocParser::readAppInfoRecord(librevenge::RVNGInputStream *)
{
  // there is no appInfo in PalmDoc
}

void PalmDocParser::readSortInfoRecord(librevenge::RVNGInputStream *)
{
  // there is no sortInfo in PalmDoc
}

void PalmDocParser::readIndexRecord(librevenge::RVNGInputStream *const input)
{
  const uint16_t compression = readU16(input, true);
  if ((1 != compression) && (2 != compression))
  {
    EBOOK_DEBUG_MSG(("unexpected compression type %u, the file is likely damaged\n", compression));
  }
  m_compressed = 1 != compression;
  skip(input, 2);
  m_textLength = readU32(input, true);
  m_recordCount = readU16(input, true);
  m_recordSize = readU16(input, true);

  // check consistency
  // assert(m_recordCount == getDataRecordCount());
  // assert(PalmDoc_BLOCK_SIZE == m_recordSize);
}

void PalmDocParser::readDataRecord(librevenge::RVNGInputStream *input, const bool last)
{
  vector<char> uncompressed;
  uncompressed.reserve(m_recordSize);

  std::unique_ptr<librevenge::RVNGInputStream> compressedInput;

  if (m_compressed)
  {
    compressedInput.reset(new PDBLZ77Stream(input));
    input = compressedInput.get();
  }

  const long origPos = input->tell();
  while (!input->isEnd())
    uncompressed.push_back((char) readU8(input));
  m_read += unsigned(input->tell() - origPos);

  // assert(m_read <= m_textLength);
  // if (last)
  // assert(m_read == m_textLength);

  if (!m_openedDocument && !uncompressed.empty())
  {
    createConverter(uncompressed);
    openDocument();
  }

  handleText(uncompressed);

  if (last)
  {
    if (!m_openedDocument)
    {
      std::vector<char> name(getName(), getName() + std::strlen(getName()));
      createConverter(name);
      openDocument();
    }
    closeDocument();
  }
}

void PalmDocParser::createConverter(const std::vector<char> &text)
{
  if (text.empty())
    return;

  std::unique_ptr<EBOOKCharsetConverter> converter(new EBOOKCharsetConverter());
  if (converter->guessEncoding(&text[0], (unsigned) text.size()))
    m_converter = std::move(converter);
  else
    throw GenericException();
}

void PalmDocParser::openDocument()
{
  if (m_openedDocument)
    return;

  librevenge::RVNGPropertyList metadata;

  if (*getName())
  {
    vector<char> nameUtf8;
    if (m_converter->convertBytes(getName(), (unsigned int)std::strlen(getName()), nameUtf8) && !nameUtf8.empty())
    {
      nameUtf8.push_back(0);
      metadata.insert("dc:title", librevenge::RVNGString(&nameUtf8[0]));
    }
  }

  getDocument()->startDocument(librevenge::RVNGPropertyList());
  getDocument()->setDocumentMetaData(metadata);
  getDocument()->openPageSpan(getDefaultPageSpanPropList());

  m_openedDocument = true;
}

void PalmDocParser::closeDocument()
{
  // Hey! There was no paragraph break at the end of the last record!
  if (m_openedParagraph)
    closeParagraph();
  m_openedParagraph = false;

  getDocument()->closePageSpan();
  getDocument()->endDocument();
  m_openedDocument = false;
}

void PalmDocParser::handleText(const vector<char> &text)
{
  auto first(text.begin());
  auto last(text.begin());
  const vector<char>::const_iterator end(text.end());

  while (first != end)
  {
    last = std::find(first, end, '\n');

    openParagraph();
    if (last > first)
    {
      vector<char> out;
      if (m_converter->convertBytes(&*first, static_cast<unsigned>(last - first), out) && !out.empty())
      {
        out.push_back(0);
        handleCharacters(&out[0]);
      }
    }
    closeParagraph(last == end);

    first = last;
    if (first != end)
      ++first;
  }
}

void PalmDocParser::openParagraph()
{
  if (!m_openedParagraph)
    getDocument()->openParagraph(librevenge::RVNGPropertyList());
  m_openedParagraph = true;
}

void PalmDocParser::closeParagraph(const bool continuing)
{
  assert(m_openedParagraph);
  if (!continuing)
    getDocument()->closeParagraph();
  m_openedParagraph = continuing;
}

void PalmDocParser::handleCharacters(const char *const text)
{
  if (text == nullptr)
    return;

  getDocument()->insertText(librevenge::RVNGString(text));
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
