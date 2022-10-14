/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef HTMLPARSER_H_INCLUDED
#define HTMLPARSER_H_INCLUDED

#include <libe-book/libe-book.h>

namespace libebook
{

class HTMLParser
{
  // disable copying
  HTMLParser(const HTMLParser &other);
  HTMLParser &operator=(const HTMLParser &other);

public:
  HTMLParser(librevenge::RVNGInputStream *input, librevenge::RVNGTextInterface *document, bool xhtml);

  void parse();

private:
  librevenge::RVNGInputStream *const m_input;
  librevenge::RVNGTextInterface *const m_document;
  const bool m_xhtml;
};

}

#endif // HTMLPARSER_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
