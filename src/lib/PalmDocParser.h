/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef PALMDOCPARSER_H_INCLUDED
#define PALMDOCPARSER_H_INCLUDED

#include <memory>
#include <vector>

#include <librevenge/librevenge.h>

#include "PDBParser.h"


namespace libebook
{

class EBOOKCharsetConverter;

class PalmDocParser : public PDBParser
{
  // disable copying
  PalmDocParser(const PalmDocParser &);
  PalmDocParser &operator=(const PalmDocParser &);

public:
  explicit PalmDocParser(librevenge::RVNGInputStream *input, librevenge::RVNGTextInterface *document = nullptr);
  ~PalmDocParser() override;

  static bool checkType(unsigned type, unsigned creator);

private:
  void readAppInfoRecord(librevenge::RVNGInputStream *record) override;
  void readSortInfoRecord(librevenge::RVNGInputStream *record) override;
  void readIndexRecord(librevenge::RVNGInputStream *record) override;
  void readDataRecord(librevenge::RVNGInputStream *record, bool last) override;

  void createConverter(const std::vector<char> &text);

  void openDocument();
  void closeDocument();
  void handleText(const std::vector<char> &text);
  void openParagraph();
  void closeParagraph(bool continuing = false);
  void handleCharacters(const char *text);

private:
  bool m_compressed;
  unsigned m_textLength;
  unsigned m_recordCount;
  unsigned m_recordSize;
  unsigned m_read;

  /** Signalize that a paragraph is split at record break.
    *
    * We do not want to open a new paragraph in this case.
    *
    * NOTE: I do not know if this is actually done. It is quite possible
    * that a record always ends at a paragraph break. But AFAICS there
    * is no prescribed special handling for '\n', so this is at least
    * possible.
    */
  bool m_openedParagraph;
  bool m_openedDocument;

  std::unique_ptr<EBOOKCharsetConverter> m_converter;
};

}

#endif // PALMDOCPARSER_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
