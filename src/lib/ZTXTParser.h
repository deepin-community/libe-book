/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef ZTXTPARSER_H_INCLUDED
#define ZTXTPARSER_H_INCLUDED

#include "PDBParser.h"


namespace libebook
{

class ZTXTParser : public PDBParser
{
public:
  explicit ZTXTParser(librevenge::RVNGInputStream *input, librevenge::RVNGTextInterface *document = nullptr);

  static bool checkType(unsigned type, unsigned creator);

  void readAppInfoRecord(librevenge::RVNGInputStream *record) override;
  void readSortInfoRecord(librevenge::RVNGInputStream *record) override;
  void readIndexRecord(librevenge::RVNGInputStream *record) override;
  void readDataRecord(librevenge::RVNGInputStream *record, bool = true) override;

  void readDataRecords() override;

private:
  void openDocument();
  void closeDocument();
  void handleText(const librevenge::RVNGString &text);

private:
  unsigned m_recordCount;
  unsigned m_size;
  unsigned m_recordSize;
};

} // namespace libebook

#endif // ZTXTPARSER_H_INCLUDED
/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
