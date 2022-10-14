/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <cassert>

#include <librevenge-stream/librevenge-stream.h>

#include "libebook_utils.h"
#include "SoftBookCollector.h"
#include "SoftBookText.h"

namespace libebook
{

namespace
{

static const char *const replacementTable[] =
{
  /* 0 */ nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 1 */ nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 2 */ nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 3 */ nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 4 */ nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 5 */ nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 6 */ nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 7 */ nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* 8 */ nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, "\xc3\x89", nullptr,
  /* 9 */ nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* a */ "\xc2\xa0", nullptr, nullptr, nullptr, nullptr, "\xe2\x80\xa2", nullptr, nullptr, "\xc2\xae", "\xc2\xa9", "\xe2\x84\xa2", nullptr, nullptr, nullptr, "\xc3\x86", nullptr,
  /* b */ nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* c */ nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, "\xc2\xab", "\xc2\xbb", "\xe2\x80\xa6", nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* d */ "\xe2\x80\x93", "\xe2\x80\x94", "\xe2\x80\x9c", "\xe2\x80\x9d", "\xe2\x80\x98", "\xe2\x80\x99", nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* e */ nullptr, "\xc2\xb7", nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  /* f */ nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr
};

}

SoftBookText::SoftBookText(librevenge::RVNGInputStream *const input, SoftBookCollector *const collector)
  : m_input(input)
  , m_collector(collector)
  , m_text()
  , m_openHeader()
  , m_openFooter()
{
}

bool SoftBookText::parse()
{
  while (!m_input->isEnd())
  {
    const unsigned char c = readU8(m_input);

    switch (c)
    {
    // control characters
    case 0xa :
      m_collector->closeParagraph();
      break;
    case 0xb :
      m_collector->openParagraph();
      break;
    case 0xd :
      m_collector->collectLineBreak();
      break;
    case 0xe :
      m_collector->openTable();
      break;
    case 0xf :
      m_collector->collectImage();
      break;
    case 0x13 :
      m_collector->collectTableCell();
      break;
    case 0x14 :
      // ignore
      break;
    case 0x15 :
      if (m_openHeader)
        m_collector->closePageHeader();
      else
        m_collector->openPageHeader();
      m_openHeader = !m_openHeader;
      break;
    case 0x16 :
      if (m_openFooter)
        m_collector->closePageFooter();
      else
        m_collector->openPageFooter();
      m_openFooter = !m_openFooter;
      break;

    // substitute characters
    case 0x8e :
    case 0xa0 :
    case 0xa5 :
    case 0xa8 :
    case 0xa9 :
    case 0xaa :
    case 0xae :
    case 0xc7 :
    case 0xc8 :
    case 0xc9 :
    case 0xd0 :
    case 0xd1 :
    case 0xd2 :
    case 0xd3 :
    case 0xd4 :
    case 0xd5 :
    case 0xe1 :
    {
      assert(replacementTable[c]);
      m_text.append(replacementTable[c]);
      break;
    }
    default :
      m_text.push_back(char(c));
      break;
    }
  }

  return true;
}

void SoftBookText::flushText()
{
  m_collector->collectText(m_text);
  m_text.clear();
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
