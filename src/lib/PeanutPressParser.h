/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef PEANUTPRESSPARSER_H_INCLUDED
#define PEANUTPRESSPARSER_H_INCLUDED

#include <string>
#include <unordered_map>
#include <vector>

#include "PDBParser.h"


namespace libebook
{

struct PeanutPressHeader;

class PeanutPressParser : public PDBParser
{
public:
  typedef std::unordered_map<std::string, std::vector<unsigned char> > ImageMap_t;

public:
  explicit PeanutPressParser(librevenge::RVNGInputStream *input, librevenge::RVNGTextInterface *document = nullptr);
  ~PeanutPressParser() override;

  static bool checkType(unsigned type, unsigned creator);

  void readAppInfoRecord(librevenge::RVNGInputStream *record) override;
  void readSortInfoRecord(librevenge::RVNGInputStream *record) override;
  void readIndexRecord(librevenge::RVNGInputStream *record) override;
  void readDataRecord(librevenge::RVNGInputStream *record, bool last = false) override;

  void readDataRecords() override;

private:
  void readText();
  void readImages();
  void readImage(librevenge::RVNGInputStream *record, bool verified = false);

  void openDocument();
  void closeDocument();

private:
  std::unique_ptr<PeanutPressHeader> m_header;
  ImageMap_t m_imageMap;
};

} // namespace libebook

#endif // PEANUTPRESSPARSER_H_INCLUDED
/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
