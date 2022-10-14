/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef OPENEBOOKPARSER_H_INCLUDED
#define OPENEBOOKPARSER_H_INCLUDED

#include <libe-book/libe-book.h>

namespace libebook
{

class OpenEBookParser
{
  // disable copying
  OpenEBookParser(const OpenEBookParser &other);
  OpenEBookParser &operator=(const OpenEBookParser &other);

public:
  OpenEBookParser(librevenge::RVNGInputStream *input, librevenge::RVNGTextInterface *document);

  void parse();

private:
  librevenge::RVNGInputStream *const m_input;
  librevenge::RVNGTextInterface *const m_document;
};

}

#endif // OPENEBOOKPARSER_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
