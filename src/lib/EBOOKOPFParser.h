/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef EBOOKOPFPARSER_H_INCLUDED
#define EBOOKOPFPARSER_H_INCLUDED

#include <librevenge/librevenge.h>

#include "libebook_utils.h"

namespace libebook
{

class EBOOKOPFParser
{
  // disable copying
  EBOOKOPFParser(const EBOOKOPFParser &other);
  EBOOKOPFParser &operator=(const EBOOKOPFParser &other);

public:
  enum Type
  {
    TYPE_OEB,
    TYPE_EPub,
    TYPE_EPub3
  };

public:
  EBOOKOPFParser(const RVNGInputStreamPtr_t &input, const RVNGInputStreamPtr_t &package, Type type, librevenge::RVNGTextInterface *document);

  void parse();

  static bool findOPFStream(const RVNGInputStreamPtr_t &package, unsigned &stream);

private:
  const RVNGInputStreamPtr_t getStream(const char *name) const;

private:
  const RVNGInputStreamPtr_t m_input;
  const RVNGInputStreamPtr_t m_package;
  const Type m_type;
  librevenge::RVNGTextInterface *const m_document;
};

}

#endif // EBOOKOPFPARSER_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
