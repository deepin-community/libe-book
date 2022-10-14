/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef TEALDOCPARSER_H_INCLUDED
#define TEALDOCPARSER_H_INCLUDED

#include <memory>
#include <vector>

#include "PDBParser.h"


namespace libebook
{

class EBOOKCharsetConverter;

class TealDocTextParser;

class TealDocParser : public PDBParser
{
  // disable copying
  TealDocParser(const TealDocParser &);
  TealDocParser &operator=(const TealDocParser &);

public:
  explicit TealDocParser(librevenge::RVNGInputStream *input, librevenge::RVNGTextInterface *document = nullptr);
  ~TealDocParser() override;

  static bool checkType(unsigned type, unsigned creator);

private:
  void readAppInfoRecord(librevenge::RVNGInputStream *record) override;
  void readSortInfoRecord(librevenge::RVNGInputStream *record) override;
  void readIndexRecord(librevenge::RVNGInputStream *record) override;
  void readDataRecord(librevenge::RVNGInputStream *record, bool last) override;

  void createConverter(const std::vector<char> &text);

  void openDocument();
  void closeDocument();

private:
  bool m_compressed;
  unsigned m_textLength;
  unsigned m_recordCount;
  unsigned m_recordSize;
  unsigned m_read;

  bool m_openedDocument;

  std::unique_ptr<EBOOKCharsetConverter> m_converter;
  std::unique_ptr<TealDocTextParser> m_textParser;
};

}

#endif // TEALDOCPARSER_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
