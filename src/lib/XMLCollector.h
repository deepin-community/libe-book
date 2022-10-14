/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef XMLCOLLECTOR_H_INCLUDED
#define XMLCOLLECTOR_H_INCLUDED

#include <string>

#include <librevenge/librevenge.h>

#include "XMLTreeWalker.h"

namespace libebook
{

class XMLCollector
{
  // disable copying
  XMLCollector(const XMLCollector &);
  XMLCollector &operator=(const XMLCollector &);

public:
  struct State;

public:
  explicit XMLCollector(librevenge::RVNGTextInterface *document);

  void collectMetadata(const librevenge::RVNGPropertyList &metadata);

  void collectText(const std::string &text);
  void collectSpace();
  void collectTab();
  void collectLineBreak();

  void collectImage(const librevenge::RVNGPropertyList &props);

  void collectCoveredTableCell(const librevenge::RVNGPropertyList &props);

  void openPageSpan(const librevenge::RVNGPropertyList &props);
  void closePageSpan();
  void openHeader(const librevenge::RVNGPropertyList &props);
  void closeHeader();
  void openFooter(const librevenge::RVNGPropertyList &props);
  void closeFooter();
  void openFootnote(const librevenge::RVNGPropertyList &props);
  void closeFootnote();

  void openParagraph(const librevenge::RVNGPropertyList &props);
  void closeParagraph();
  void openSpan(const librevenge::RVNGPropertyList &props);
  void closeSpan();
  void openLink(const librevenge::RVNGPropertyList &props);
  void closeLink();

  void openTable(const librevenge::RVNGPropertyList &props);
  void closeTable();
  void openTableRow(const librevenge::RVNGPropertyList &props);
  void closeTableRow();
  void openTableCell(const librevenge::RVNGPropertyList &props);
  void closeTableCell();

  void openOrderedList(const librevenge::RVNGPropertyList &props);
  void closeOrderedList();
  void openUnorderedList(const librevenge::RVNGPropertyList &props);
  void closeUnorderedList();

  void openListElement(const librevenge::RVNGPropertyList &props);
  void closeListElement();

  void openObject(const librevenge::RVNGPropertyList &props);
  void closeObject();

  /** Signalize an end of parsing from the parser.
    */
  void finish();

private:
  librevenge::RVNGTextInterface *const m_document;
  std::shared_ptr<State> m_state;
};

}

#endif // XMLCOLLECTOR_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
