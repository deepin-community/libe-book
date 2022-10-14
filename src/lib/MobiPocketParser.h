/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef MOBIPOCKETPARSER_H_INCLUDED
#define MOBIPOCKETPARSER_H_INCLUDED

#include "PDBParser.h"

namespace libebook
{

class MobiPocketParser : public PDBParser
{
  // disable copying
  MobiPocketParser(const MobiPocketParser &other);
  MobiPocketParser &operator=(const MobiPocketParser &other);

public:
  explicit MobiPocketParser(librevenge::RVNGInputStream *input, librevenge::RVNGTextInterface *document = 0);
  virtual ~MobiPocketParser();

  static bool checkType(unsigned type, unsigned creator);

private:
  virtual void readAppInfoRecord(librevenge::RVNGInputStream *record);
  virtual void readSortInfoRecord(librevenge::RVNGInputStream *record);
  virtual void readIndexRecord(librevenge::RVNGInputStream *record);
  virtual void readDataRecord(librevenge::RVNGInputStream *record, bool last = false);
};

}

#endif // MOBIPOCKETPARSER_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
