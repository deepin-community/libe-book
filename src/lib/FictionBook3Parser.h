/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef FICTIONBOOK3PARSER_H_INCLUDED
#define FICTIONBOOK3PARSER_H_INCLUDED

#include <librevenge/librevenge.h>

#include "libebook_utils.h"

namespace libebook
{

class FictionBook3Parser
{
  // disable copying
  FictionBook3Parser(const FictionBook3Parser &other);
  FictionBook3Parser &operator=(const FictionBook3Parser &other);

public:
  FictionBook3Parser(const RVNGInputStreamPtr_t &input, librevenge::RVNGTextInterface *document);

  bool parse();

private:
  const RVNGInputStreamPtr_t &m_input;
  librevenge::RVNGTextInterface *const m_document;
};

}

#endif // FICTIONBOOK3PARSER_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
