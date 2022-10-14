/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <cassert>

#include "MobiPocketParser.h"

#include "libebook_utils.h"

namespace libebook
{

namespace
{

static const unsigned MOBIPOCKET_TYPE = PDB_CODE("BOOK");
static const unsigned MOBIPOCKET_CREATOR = PDB_CODE("MobiPocket");

}

MobiPocketParser::MobiPocketParser(librevenge::RVNGInputStream *const input, librevenge::RVNGTextInterface *const document)
  : PDBParser(input, document, MOBIPOCKET_TYPE, MOBIPOCKET_CREATOR)
{
  assert(input);
  // TODO: implement me
  throw UnsupportedFormat();
}

MobiPocketParser::~MobiPocketParser()
{
}

bool MobiPocketParser::checkType(const unsigned type, const unsigned creator)
{
  return (MOBIPOCKET_TYPE == type) && (MOBIPOCKET_CREATOR == creator);
}

void MobiPocketParser::readAppInfoRecord(librevenge::RVNGInputStream *)
{
  // nothing needed
}

void MobiPocketParser::readSortInfoRecord(librevenge::RVNGInputStream *)
{
  // nothing needed
}

void MobiPocketParser::readIndexRecord(librevenge::RVNGInputStream *const record)
{
  // TODO: implement me
  (void) record;
}

void MobiPocketParser::readDataRecord(librevenge::RVNGInputStream *const record, const bool last)
{
  // TODO: implement me
  (void) record;
  (void) last;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
