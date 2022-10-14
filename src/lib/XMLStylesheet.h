/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef XMLSTYLESHEET_H_INCLUDED
#define XMLSTYLESHEET_H_INCLUDED

#include <deque>
#include <string>

#include <librevenge-stream/librevenge-stream.h>

#include "libebook_libcss.h"

namespace libebook
{

class XMLStylesheet
{
public:
  enum Source
  {
    SOURCE_UA,
    SOURCE_USER,
    SOURCE_AUTHOR
  };

public:
  explicit XMLStylesheet(Source source, bool quirks = false, librevenge::RVNGInputStream *input = 0);

  void append(const std::string &selector, const std::string &rule);

  bool isQuirks() const;
  Source getSource() const;

  CSSStylesheetPtr_t get() const;

private:
  std::deque<std::string> m_data;
  const Source m_source;
  const bool m_quirks;
  mutable bool m_changed;
  mutable CSSStylesheetPtr_t m_compiled;
};

typedef std::deque<XMLStylesheet> XMLStylesheets_t;

}

#endif // XMLSTYLESHEET_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
