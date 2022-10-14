/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "QiOOParser.h"

#include <cassert>

namespace libebook
{

namespace
{

void parseText(librevenge::RVNGInputStream *const input, librevenge::RVNGTextInterface *const document)
{
  librevenge::RVNGPropertyList dummy;

  document->startDocument(dummy);

  if (!input->isEnd())
  {
    document->openPageSpan(dummy);

    // output paragraphs
    librevenge::RVNGString text;
    bool ignoreNextLineBreak = false;
    while (!input->isEnd())
    {
      const uint8_t c = readU8(input);
      if (('\n' == c) || ('\r' == c))
      {
        if (ignoreNextLineBreak)
          ignoreNextLineBreak = false;
        else
        {
          document->openParagraph(dummy);
          document->openSpan(dummy);
          document->insertText(text);
          document->closeSpan();
          document->closeParagraph();
          text.clear();
          ignoreNextLineBreak = true;
        }
      }
      else
      {
        text.append((char) c);
        ignoreNextLineBreak = false;
      }
    }

    document->closePageSpan();
  }

  document->endDocument();
}

}

QiOOParser::QiOOParser(const RVNGInputStreamPtr_t &input, librevenge::RVNGTextInterface *const document)
  : m_input()
  , m_document(document)
{
  assert(bool(input));

  m_input.reset(input->getSubStreamByName("data"));

  if (!m_input)
    throw PackageError();
}

void QiOOParser::parse()
{
  parseText(m_input.get(), m_document);
}

} // namespace libebook

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
