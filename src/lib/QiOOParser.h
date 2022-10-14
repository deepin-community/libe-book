/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef QIOOPARSER_H_INCLUDED
#define QIOOPARSER_H_INCLUDED

#include <librevenge/librevenge.h>

#include "libebook_utils.h"

namespace libebook
{

class QiOOParser
{
  // disable copying
  QiOOParser(const QiOOParser &other);
  QiOOParser &operator=(const QiOOParser &other);

public:
  explicit QiOOParser(const RVNGInputStreamPtr_t &input, librevenge::RVNGTextInterface *document = nullptr);

  void parse();

private:
  RVNGInputStreamPtr_t m_input;
  librevenge::RVNGTextInterface *m_document;
};

}

#endif // QIOOPARSER_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
