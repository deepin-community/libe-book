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

#include <librevenge/librevenge.h>
#include <librevenge-stream/librevenge-stream.h>

#include "libebook_utils.h"
#include "EBOOKCharsetConverter.h"
#include "EBOOKMemoryStream.h"
#include "EBOOKUTF8Stream.h"
#include "TCRParser.h"

using std::shared_ptr;

using std::string;

namespace libebook
{

namespace
{

const char TCR_SIGNATURE[] = "!!8-Bit!!";

}

TCRParser::TCRParser(librevenge::RVNGInputStream *input, librevenge::RVNGTextInterface *document)
  : m_input(input)
  , m_document(document)
{
  const size_t length = EBOOK_NUM_ELEMENTS(TCR_SIGNATURE) - 1; // without the final \0
  input->seek(0, librevenge::RVNG_SEEK_SET);
  const auto *const sig = reinterpret_cast<const char *>(readNBytes(input, length));
  if (!std::equal(sig, sig + length, TCR_SIGNATURE))
    throw UnsupportedFormat();
}

bool TCRParser::parse()
{
  readReplacementTable();

  const std::shared_ptr<librevenge::RVNGInputStream> input = uncompress();

  m_document->startDocument(librevenge::RVNGPropertyList());
  m_document->openPageSpan(getDefaultPageSpanPropList());

  if (bool(input))
    writeText(input);

  m_document->closePageSpan();
  m_document->endDocument();

  return true;
}

void TCRParser::readReplacementTable()
{
  for (size_t i = 0; i != EBOOK_NUM_ELEMENTS(m_replacementTable); ++i)
  {
    const unsigned char length = readU8(m_input);
    if (0 != length)
    {
      const unsigned char *const replacement = readNBytes(m_input, length);
      m_replacementTable[i].assign(reinterpret_cast<const char *>(replacement), length);
    }
  }
}

shared_ptr<librevenge::RVNGInputStream> TCRParser::uncompress()
{
  string text;

  while (!m_input->isEnd())
  {
    const unsigned char c = readU8(m_input);
    text.append(m_replacementTable[c]);
  }

  EBOOKCharsetConverter converter;
  const bool knownEncoding = converter.guessEncoding(text.data(), (unsigned) text.size());

  RVNGInputStreamPtr_t strm(new EBOOKMemoryStream(reinterpret_cast<const unsigned char *>(text.data()), (unsigned) text.size()));
  if (bool(strm) && knownEncoding)
  {
    RVNGInputStreamPtr_t tmpStream(strm);
    strm.reset(new EBOOKUTF8Stream(tmpStream.get(), &converter));
  }
  return strm;
}

void TCRParser::writeText(const shared_ptr<librevenge::RVNGInputStream> &input)
{
  assert(bool(input));

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
