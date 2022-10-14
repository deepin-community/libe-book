/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef ROCKETEBOOKPARSER_H_INCLUDED
#define ROCKETEBOOKPARSER_H_INCLUDED

#include <libe-book/libe-book.h>

#include "RocketEBookHeader.h"

namespace libebook
{

class RocketEBookParser
{
  // disable copying
  RocketEBookParser(const RocketEBookParser &other);
  RocketEBookParser &operator=(const RocketEBookParser &other);

public:
  explicit RocketEBookParser(const RVNGInputStreamPtr_t &input, librevenge::RVNGTextInterface *document = 0);

  void parse();

private:
  const RVNGInputStreamPtr_t m_input;
  librevenge::RVNGTextInterface *m_document;
  RocketEBookHeader m_header;
};

}

#endif // ROCKETEBOOKPARSER_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
