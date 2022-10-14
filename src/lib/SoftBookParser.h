/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef SOFTBOOKPARSER_H_INCLUDED
#define SOFTBOOKPARSER_H_INCLUDED

#include "SoftBookCollector.h"
#include "SoftBookHeader.h"

namespace libebook
{

class SoftBookParser
{
  // disable copying
  SoftBookParser(const SoftBookParser &);
  SoftBookParser &operator=(const SoftBookParser &);

public:
  SoftBookParser(librevenge::RVNGInputStream *input, librevenge::RVNGTextInterface *document);

  bool parse();

private:
  void createTextStream();

  std::shared_ptr<librevenge::RVNGInputStream> getFileStream(const char *name) const;

private:
  const SoftBookHeader m_header;
  librevenge::RVNGInputStream *const m_input;
  SoftBookCollector m_collector;
  std::shared_ptr<librevenge::RVNGInputStream> m_resources;
  std::shared_ptr<librevenge::RVNGInputStream> m_text;
};

}

#endif // SOFTBOOKPARSER_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
