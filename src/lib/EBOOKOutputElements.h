/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef EBOOKOUTPUTELEMENTS_H
#define EBOOKOUTPUTELEMENTS_H

#include <list>
#include <map>

#include <librevenge/librevenge.h>

namespace libebook
{

class EBOOKOutputElement;

class EBOOKOutputElements
{
public:
  EBOOKOutputElements();
  virtual ~EBOOKOutputElements();
  void append(const EBOOKOutputElements &elements);
  void write(librevenge::RVNGTextInterface *iface) const;
  void addCloseEndnote();
  void addCloseFooter();
  void addCloseFootnote();
  void addCloseFrame();
  void addCloseHeader();
  void addCloseLink();
  void addCloseListElement();
  void addCloseOrderedListLevel();
  void addClosePageSpan();
  void addCloseParagraph();
  void addCloseSection();
  void addCloseSpan();
  void addCloseTable();
  void addCloseTableCell();
  void addCloseTableRow();
  void addCloseUnorderedListLevel();
  void addInsertBinaryObject(const librevenge::RVNGPropertyList &propList);
  void addInsertCoveredTableCell(const librevenge::RVNGPropertyList &propList);
  void addInsertLineBreak();
  void addInsertSpace();
  void addInsertTab();
  void addInsertText(const librevenge::RVNGString &text);
  void addOpenEndnote(const librevenge::RVNGPropertyList &propList);
  void addOpenFooter(const librevenge::RVNGPropertyList &propList, int id);
  void addOpenFootnote(const librevenge::RVNGPropertyList &propList);
  void addOpenFrame(const librevenge::RVNGPropertyList &propList);
  void addOpenHeader(const librevenge::RVNGPropertyList &propList, int id);
  void addOpenLink(const librevenge::RVNGPropertyList &propList);
  void addOpenListElement(const librevenge::RVNGPropertyList &propList);
  void addOpenOrderedListLevel(const librevenge::RVNGPropertyList &propList);
  void addOpenPageSpan(const librevenge::RVNGPropertyList &propList);
  void addOpenPageSpan(const librevenge::RVNGPropertyList &propList,
                       int footer, int footerLeft, int footerFirst, int footerLast,
                       int header, int headerLeft, int headerFirst, int headerLast);
  void addOpenParagraph(const librevenge::RVNGPropertyList &propList);
  void addOpenSection(const librevenge::RVNGPropertyList &propList);
  void addOpenSpan(const librevenge::RVNGPropertyList &propList);
  void addOpenTable(const librevenge::RVNGPropertyList &propList);
  void addOpenTableCell(const librevenge::RVNGPropertyList &propList);
  void addOpenTableRow(const librevenge::RVNGPropertyList &propList);
  void addOpenUnorderedListLevel(const librevenge::RVNGPropertyList &propList);
  void addStartDocument(const librevenge::RVNGPropertyList &propList);
  bool empty() const
  {
    return m_bodyElements.empty();
  }
private:
  EBOOKOutputElements(const EBOOKOutputElements &);
  EBOOKOutputElements &operator=(const EBOOKOutputElements &);
  std::list<EBOOKOutputElement *> m_bodyElements;
  std::map<int, std::list<EBOOKOutputElement *> > m_headerElements;
  std::map<int, std::list<EBOOKOutputElement *> > m_footerElements;
  std::list<EBOOKOutputElement *> *m_elements;
};

}

#endif /* EBOOKOUTPUTELEMENTS_H */

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
