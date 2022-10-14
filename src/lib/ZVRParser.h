/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef ZVRPARSER_H_INCLUDED
#define ZVRPARSER_H_INCLUDED

#include <string>

namespace libebook
{

class ZVRParser
{
  // -Weffc++
  ZVRParser(const ZVRParser &other);
  ZVRParser &operator=(const ZVRParser &other);

public:

  ZVRParser(librevenge::RVNGInputStream *input, librevenge::RVNGTextInterface *document = nullptr);

  bool parse();

private:
  void readReplacementTable();

  std::shared_ptr<librevenge::RVNGInputStream> uncompress();

  void writeText(std::shared_ptr<librevenge::RVNGInputStream> input);

private:
  librevenge::RVNGInputStream *const m_input;
  librevenge::RVNGTextInterface *const m_document;

  std::string m_replacementTable[256];
};

} // namespace libebook

#endif // ZVRPARSER_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
