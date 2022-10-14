/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef PLUCKERPARSER_H_INCLUDED
#define PLUCKERPARSER_H_INCLUDED

#include <map>
#include <memory>
#include <vector>

#include "PDBParser.h"

namespace libebook
{

typedef std::map<unsigned, std::vector<unsigned char> > PluckerImageMap_t;

struct PluckerHeader;
struct PluckerParserState;
struct PluckerRecordHeader;

class PluckerParser : public PDBParser
{
  // -Weffc++
  PluckerParser(const PluckerParser &other);
  PluckerParser &operator=(const PluckerParser &other);

public:
  explicit PluckerParser(librevenge::RVNGInputStream *input, librevenge::RVNGTextInterface *document = nullptr);

  static bool checkType(unsigned type, unsigned creator);

private:
  void readAppInfoRecord(librevenge::RVNGInputStream *record) override;
  void readSortInfoRecord(librevenge::RVNGInputStream *record) override;
  void readIndexRecord(librevenge::RVNGInputStream *record) override;
  void readDataRecord(librevenge::RVNGInputStream *record, bool last = false) override;

  void readDataRecords() override;

  void readMetadata(librevenge::RVNGInputStream *input, const PluckerRecordHeader &header);
  void readImage(librevenge::RVNGInputStream *input, const PluckerRecordHeader &header);
  void readText(librevenge::RVNGInputStream *input, const PluckerRecordHeader &header, const std::vector<unsigned> &paragraphLengths);

  std::shared_ptr<librevenge::RVNGInputStream> getUncompressedStream(librevenge::RVNGInputStream *input) const;

private:
  std::shared_ptr<PluckerHeader> m_header;
  const std::shared_ptr<PluckerParserState> m_state;
};

} // namespace libebook

#endif // PLUCKERPARSER_H_INCLUDED
/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
