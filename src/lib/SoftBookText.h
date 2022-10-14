/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef SOFTBOOKTEXT_H_INCLUDED
#define SOFTBOOKTEXT_H_INCLUDED

#include <string>


namespace libebook
{

class SoftBookCollector;

class SoftBookText
{
  // -Weffc++
  SoftBookText(const SoftBookText &);
  SoftBookText &operator=(const SoftBookText &);

public:
  SoftBookText(librevenge::RVNGInputStream *input, SoftBookCollector *collector);

  bool parse();

private:
  void flushText();

private:
  librevenge::RVNGInputStream *const m_input;
  SoftBookCollector *const m_collector;

  std::string m_text;
  bool m_openHeader;
  bool m_openFooter;
};

}

#endif // SOFTBOOKTEXT_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
