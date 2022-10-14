/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef EPUBPARSER_H_INCLUDED
#define EPUBPARSER_H_INCLUDED

#include <libe-book/libe-book.h>

namespace libebook
{

class EPubParser
{
  // disable copying
  EPubParser(const EPubParser &other);
  EPubParser &operator=(const EPubParser &other);

public:
  EPubParser(librevenge::RVNGInputStream *input, librevenge::RVNGTextInterface *document);

  void parse();

private:
  librevenge::RVNGInputStream *const m_input;
  librevenge::RVNGTextInterface *const m_document;
};

}

#endif // EPUBPARSER_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
