/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <algorithm>

#include <librevenge/librevenge.h>
#include <librevenge-stream/librevenge-stream.h>

#include "libebook_utils.h"
#include "EBOOKMemoryStream.h"
#include "ZVRParser.h"

using std::shared_ptr;

using std::string;

namespace libebook
{

namespace
{

const char ZVR_SIGNATURE[] = "!!Compressed!!\n";

}

ZVRParser::ZVRParser(librevenge::RVNGInputStream *input, librevenge::RVNGTextInterface *document)
  : m_input(input)
  , m_document(document)
{
  // ignore the trailing \0
  const size_t length = EBOOK_NUM_ELEMENTS(ZVR_SIGNATURE) - 1;
  input->seek(0, librevenge::RVNG_SEEK_SET);
  const auto *const sig = reinterpret_cast<const char *>(readNBytes(input, length));
  if (!std::equal(sig, sig + length, ZVR_SIGNATURE))
    throw UnsupportedFormat();
}

bool ZVRParser::parse()
{
  readReplacementTable();

  const std::shared_ptr<librevenge::RVNGInputStream> input = uncompress();

  m_document->startDocument(librevenge::RVNGPropertyList());
  m_document->openPageSpan(getDefaultPageSpanPropList());

  writeText(input);

  m_document->closePageSpan();
  m_document->endDocument();

  return false;
}

void ZVRParser::readReplacementTable()
{
  for (size_t i = 1; i != EBOOK_NUM_ELEMENTS(m_replacementTable); ++i)
  {
    string replacement;

    while (!m_input->isEnd())
    {
      unsigned char c = readU8(m_input);
      if ('\n' == c)
      {
        if (replacement.empty())
          m_replacementTable[i] = static_cast<char>(i);
        else
        {
          m_replacementTable[i] = replacement;
          replacement.clear();
        }
        break;
      }
      else
        replacement.push_back(char(c));
    }
  }
}

shared_ptr<librevenge::RVNGInputStream> ZVRParser::uncompress()
{
  string text;

  while (!m_input->isEnd())
  {
    const unsigned char c = readU8(m_input);
    text.append(m_replacementTable[c]);
  }

  shared_ptr<librevenge::RVNGInputStream> strm(new EBOOKMemoryStream(reinterpret_cast<const unsigned char *>(text.data()), (unsigned) text.size()));
  return strm;
}

void ZVRParser::writeText(const shared_ptr<librevenge::RVNGInputStream> input)
{
  string text;

  while (!input->isEnd())
  {
    const unsigned char c = readU8(input.get());
    if ('\n' == c)
    {
      m_document->openParagraph(librevenge::RVNGPropertyList());
      if (!text.empty())
      {
        m_document->openSpan(librevenge::RVNGPropertyList());
        m_document->insertText(librevenge::RVNGString(text.c_str()));
        m_document->closeSpan();

        text.clear();
      }
      m_document->closeParagraph();
    }
    else
      text.push_back((char) c);
  }
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
