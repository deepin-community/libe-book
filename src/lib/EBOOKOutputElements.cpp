/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "EBOOKOutputElements.h"

namespace libebook
{

class EBOOKOutputElement
{
public:
  EBOOKOutputElement() {}
  virtual ~EBOOKOutputElement() {}
  virtual void write(librevenge::RVNGTextInterface *iface,
                     const std::map<int, std::list<EBOOKOutputElement *> > *footers,
                     const std::map<int, std::list<EBOOKOutputElement *> > *headers) const = 0;
};

namespace
{

class CloseEndnoteElement : public EBOOKOutputElement
{
public:
  CloseEndnoteElement() {}
  ~CloseEndnoteElement() override {}
  void write(librevenge::RVNGTextInterface *iface,
             const std::map<int, std::list<EBOOKOutputElement *> > *footers,
             const std::map<int, std::list<EBOOKOutputElement *> > *headers) const override;
};

class CloseFooterElement : public EBOOKOutputElement
{
public:
  CloseFooterElement() {}
  ~CloseFooterElement() override {}
  void write(librevenge::RVNGTextInterface *iface,
             const std::map<int, std::list<EBOOKOutputElement *> > *footers,
             const std::map<int, std::list<EBOOKOutputElement *> > *headers) const override;
};

class CloseFootnoteElement : public EBOOKOutputElement
{
public:
  CloseFootnoteElement() {}
  ~CloseFootnoteElement() override {}
  void write(librevenge::RVNGTextInterface *iface,
             const std::map<int, std::list<EBOOKOutputElement *> > *footers,
             const std::map<int, std::list<EBOOKOutputElement *> > *headers) const override;
};

class CloseFrameElement : public EBOOKOutputElement
{
public:
  CloseFrameElement() {}
  ~CloseFrameElement() override {}
  void write(librevenge::RVNGTextInterface *iface,
             const std::map<int, std::list<EBOOKOutputElement *> > *footers,
             const std::map<int, std::list<EBOOKOutputElement *> > *headers) const override;
};

class CloseHeaderElement : public EBOOKOutputElement
{
public:
  CloseHeaderElement() {}
  ~CloseHeaderElement() override {}
  void write(librevenge::RVNGTextInterface *iface,
             const std::map<int, std::list<EBOOKOutputElement *> > *footers,
             const std::map<int, std::list<EBOOKOutputElement *> > *headers) const override;
};

class CloseLinkElement : public EBOOKOutputElement
{
public:
  CloseLinkElement() {}
  ~CloseLinkElement() override {}
  void write(librevenge::RVNGTextInterface *iface,
             const std::map<int, std::list<EBOOKOutputElement *> > *footers,
             const std::map<int, std::list<EBOOKOutputElement *> > *headers) const override;
  EBOOKOutputElement *clone()
  {
    return new CloseLinkElement();
  }
};

class CloseListElementElement : public EBOOKOutputElement
{
public:
  CloseListElementElement() {}
  ~CloseListElementElement() override {}
  void write(librevenge::RVNGTextInterface *iface,
             const std::map<int, std::list<EBOOKOutputElement *> > *footers,
             const std::map<int, std::list<EBOOKOutputElement *> > *headers) const override;
  EBOOKOutputElement *clone()
  {
    return new CloseListElementElement();
  }
};

class CloseOrderedListLevelElement : public EBOOKOutputElement
{
public:
  CloseOrderedListLevelElement() {}
  ~CloseOrderedListLevelElement() override {}
  void write(librevenge::RVNGTextInterface *iface,
             const std::map<int, std::list<EBOOKOutputElement *> > *footers,
             const std::map<int, std::list<EBOOKOutputElement *> > *headers) const override;
  EBOOKOutputElement *clone()
  {
    return new CloseOrderedListLevelElement();
  }
};

class ClosePageSpanElement : public EBOOKOutputElement
{
public:
  ClosePageSpanElement() {}
  ~ClosePageSpanElement() override {}
  void write(librevenge::RVNGTextInterface *iface,
             const std::map<int, std::list<EBOOKOutputElement *> > *footers,
             const std::map<int, std::list<EBOOKOutputElement *> > *headers) const override;
  EBOOKOutputElement *clone()
  {
    return new ClosePageSpanElement();
  }
};

class CloseParagraphElement : public EBOOKOutputElement
{
public:
  CloseParagraphElement() {}
  ~CloseParagraphElement() override {}
  void write(librevenge::RVNGTextInterface *iface,
             const std::map<int, std::list<EBOOKOutputElement *> > *footers,
             const std::map<int, std::list<EBOOKOutputElement *> > *headers) const override;
  EBOOKOutputElement *clone()
  {
    return new CloseParagraphElement();
  }
};

class CloseSectionElement : public EBOOKOutputElement
{
public:
  CloseSectionElement() {}
  ~CloseSectionElement() override {}
  void write(librevenge::RVNGTextInterface *iface,
             const std::map<int, std::list<EBOOKOutputElement *> > *footers,
             const std::map<int, std::list<EBOOKOutputElement *> > *headers) const override;
  EBOOKOutputElement *clone()
  {
    return new CloseSectionElement();
  }
};

class CloseSpanElement : public EBOOKOutputElement
{
public:
  CloseSpanElement() {}
  ~CloseSpanElement() override {}
  void write(librevenge::RVNGTextInterface *iface,
             const std::map<int, std::list<EBOOKOutputElement *> > *footers,
             const std::map<int, std::list<EBOOKOutputElement *> > *headers) const override;
  EBOOKOutputElement *clone()
  {
    return new CloseSpanElement();
  }
};

class CloseTableElement : public EBOOKOutputElement
{
public:
  CloseTableElement() {}
  ~CloseTableElement() override {}
  void write(librevenge::RVNGTextInterface *iface,
             const std::map<int, std::list<EBOOKOutputElement *> > *footers,
             const std::map<int, std::list<EBOOKOutputElement *> > *headers) const override;
  EBOOKOutputElement *clone()
  {
    return new CloseTableElement();
  }
};

class CloseTableCellElement : public EBOOKOutputElement
{
public:
  CloseTableCellElement() {}
  ~CloseTableCellElement() override {}
  void write(librevenge::RVNGTextInterface *iface,
             const std::map<int, std::list<EBOOKOutputElement *> > *footers,
             const std::map<int, std::list<EBOOKOutputElement *> > *headers) const override;
  EBOOKOutputElement *clone()
  {
    return new CloseTableCellElement();
  }
};

class CloseTableRowElement : public EBOOKOutputElement
{
public:
  CloseTableRowElement() {}
  ~CloseTableRowElement() override {}
  void write(librevenge::RVNGTextInterface *iface,
             const std::map<int, std::list<EBOOKOutputElement *> > *footers,
             const std::map<int, std::list<EBOOKOutputElement *> > *headers) const override;
  EBOOKOutputElement *clone()
  {
    return new CloseTableRowElement();
  }
};

class CloseUnorderedListLevelElement : public EBOOKOutputElement
{
public:
  CloseUnorderedListLevelElement() {}
  ~CloseUnorderedListLevelElement() override {}
  void write(librevenge::RVNGTextInterface *iface,
             const std::map<int, std::list<EBOOKOutputElement *> > *footers,
             const std::map<int, std::list<EBOOKOutputElement *> > *headers) const override;
  EBOOKOutputElement *clone()
  {
    return new CloseUnorderedListLevelElement();
  }
};

class InsertBinaryObjectElement : public EBOOKOutputElement
{
public:
  InsertBinaryObjectElement(const librevenge::RVNGPropertyList &propList) :
    m_propList(propList) {}
  ~InsertBinaryObjectElement() override {}
  void write(librevenge::RVNGTextInterface *iface,
             const std::map<int, std::list<EBOOKOutputElement *> > *footers,
             const std::map<int, std::list<EBOOKOutputElement *> > *headers) const override;
  EBOOKOutputElement *clone()
  {
    return new InsertBinaryObjectElement(m_propList);
  }
private:
  librevenge::RVNGPropertyList m_propList;
};

class InsertCoveredTableCellElement : public EBOOKOutputElement
{
public:
  InsertCoveredTableCellElement(const librevenge::RVNGPropertyList &propList) :
    m_propList(propList) {}
  ~InsertCoveredTableCellElement() override {}
  void write(librevenge::RVNGTextInterface *iface,
             const std::map<int, std::list<EBOOKOutputElement *> > *footers,
             const std::map<int, std::list<EBOOKOutputElement *> > *headers) const override;
  EBOOKOutputElement *clone()
  {
    return new InsertCoveredTableCellElement(m_propList);
  }
private:
  librevenge::RVNGPropertyList m_propList;
};

class InsertLineBreakElement : public EBOOKOutputElement
{
public:
  InsertLineBreakElement() {}
  ~InsertLineBreakElement() override {}
  void write(librevenge::RVNGTextInterface *iface,
             const std::map<int, std::list<EBOOKOutputElement *> > *footers,
             const std::map<int, std::list<EBOOKOutputElement *> > *headers) const override;
  EBOOKOutputElement *clone()
  {
    return new InsertLineBreakElement();
  }
};

class InsertSpaceElement : public EBOOKOutputElement
{
public:
  InsertSpaceElement() {}
  ~InsertSpaceElement() override {}
  void write(librevenge::RVNGTextInterface *iface,
             const std::map<int, std::list<EBOOKOutputElement *> > *footers,
             const std::map<int, std::list<EBOOKOutputElement *> > *headers) const override;
  EBOOKOutputElement *clone()
  {
    return new InsertSpaceElement();
  }
};

class InsertTabElement : public EBOOKOutputElement
{
public:
  InsertTabElement() {}
  ~InsertTabElement() override {}
  void write(librevenge::RVNGTextInterface *iface,
             const std::map<int, std::list<EBOOKOutputElement *> > *footers,
             const std::map<int, std::list<EBOOKOutputElement *> > *headers) const override;
  EBOOKOutputElement *clone()
  {
    return new InsertTabElement();
  }
};

class InsertTextElement : public EBOOKOutputElement
{
public:
  InsertTextElement(const librevenge::RVNGString &text) :
    m_text(text) {}
  ~InsertTextElement() override {}
  void write(librevenge::RVNGTextInterface *iface,
             const std::map<int, std::list<EBOOKOutputElement *> > *footers,
             const std::map<int, std::list<EBOOKOutputElement *> > *headers) const override;
  EBOOKOutputElement *clone()
  {
    return new InsertTextElement(m_text);
  }
private:
  librevenge::RVNGString m_text;
};

class OpenEndnoteElement : public EBOOKOutputElement
{
public:
  OpenEndnoteElement(const librevenge::RVNGPropertyList &propList) :
    m_propList(propList) {}
  ~OpenEndnoteElement() override {}
  void write(librevenge::RVNGTextInterface *iface,
             const std::map<int, std::list<EBOOKOutputElement *> > *footers,
             const std::map<int, std::list<EBOOKOutputElement *> > *headers) const override;
  EBOOKOutputElement *clone()
  {
    return new OpenEndnoteElement(m_propList);
  }
private:
  librevenge::RVNGPropertyList m_propList;
};

class OpenFooterElement : public EBOOKOutputElement
{
public:
  OpenFooterElement(const librevenge::RVNGPropertyList &propList) :
    m_propList(propList) {}
  ~OpenFooterElement() override {}
  void write(librevenge::RVNGTextInterface *iface,
             const std::map<int, std::list<EBOOKOutputElement *> > *footers,
             const std::map<int, std::list<EBOOKOutputElement *> > *headers) const override;
  EBOOKOutputElement *clone()
  {
    return new OpenFooterElement(m_propList);
  }
private:
  librevenge::RVNGPropertyList m_propList;
};

class OpenFootnoteElement : public EBOOKOutputElement
{
public:
  OpenFootnoteElement(const librevenge::RVNGPropertyList &propList) :
    m_propList(propList) {}
  ~OpenFootnoteElement() override {}
  void write(librevenge::RVNGTextInterface *iface,
             const std::map<int, std::list<EBOOKOutputElement *> > *footers,
             const std::map<int, std::list<EBOOKOutputElement *> > *headers) const override;
  EBOOKOutputElement *clone()
  {
    return new OpenFootnoteElement(m_propList);
  }
private:
  librevenge::RVNGPropertyList m_propList;
};

class OpenFrameElement : public EBOOKOutputElement
{
public:
  OpenFrameElement(const librevenge::RVNGPropertyList &propList) :
    m_propList(propList) {}
  ~OpenFrameElement() override {}
  void write(librevenge::RVNGTextInterface *iface,
             const std::map<int, std::list<EBOOKOutputElement *> > *footers,
             const std::map<int, std::list<EBOOKOutputElement *> > *headers) const override;
  EBOOKOutputElement *clone()
  {
    return new OpenFrameElement(m_propList);
  }
private:
  librevenge::RVNGPropertyList m_propList;
};

class OpenHeaderElement : public EBOOKOutputElement
{
public:
  OpenHeaderElement(const librevenge::RVNGPropertyList &propList) :
    m_propList(propList) {}
  ~OpenHeaderElement() override {}
  void write(librevenge::RVNGTextInterface *iface,
             const std::map<int, std::list<EBOOKOutputElement *> > *footers,
             const std::map<int, std::list<EBOOKOutputElement *> > *headers) const override;
  EBOOKOutputElement *clone()
  {
    return new OpenHeaderElement(m_propList);
  }
private:
  librevenge::RVNGPropertyList m_propList;
};

class OpenLinkElement : public EBOOKOutputElement
{
public:
  OpenLinkElement(const librevenge::RVNGPropertyList &propList) :
    m_propList(propList) {}
  ~OpenLinkElement() override {}
  void write(librevenge::RVNGTextInterface *iface,
             const std::map<int, std::list<EBOOKOutputElement *> > *footers,
             const std::map<int, std::list<EBOOKOutputElement *> > *headers) const override;
private:
  librevenge::RVNGPropertyList m_propList;
};

class OpenListElementElement : public EBOOKOutputElement
{
public:
  OpenListElementElement(const librevenge::RVNGPropertyList &propList) :
    m_propList(propList) {}
  ~OpenListElementElement() override {}
  void write(librevenge::RVNGTextInterface *iface,
             const std::map<int, std::list<EBOOKOutputElement *> > *footers,
             const std::map<int, std::list<EBOOKOutputElement *> > *headers) const override;
private:
  librevenge::RVNGPropertyList m_propList;
};

class OpenOrderedListLevelElement : public EBOOKOutputElement
{
public:
  OpenOrderedListLevelElement(const librevenge::RVNGPropertyList &propList) :
    m_propList(propList) {}
  ~OpenOrderedListLevelElement() override {}
  void write(librevenge::RVNGTextInterface *iface,
             const std::map<int, std::list<EBOOKOutputElement *> > *footers,
             const std::map<int, std::list<EBOOKOutputElement *> > *headers) const override;
private:
  librevenge::RVNGPropertyList m_propList;
};

class OpenPageSpanElement : public EBOOKOutputElement
{
public:
  OpenPageSpanElement(const librevenge::RVNGPropertyList &propList,
                      int footer, int footerLeft, int footerFirst, int footerLast,
                      int header, int headerLeft, int headerFirst, int headerLast) :
    m_propList(propList),
    m_footer(footer),
    m_footerLeft(footerLeft),
    m_footerFirst(footerFirst),
    m_footerLast(footerLast),
    m_header(header),
    m_headerLeft(headerLeft),
    m_headerFirst(headerFirst),
    m_headerLast(headerLast) {}
  ~OpenPageSpanElement() override {}
  void write(librevenge::RVNGTextInterface *iface,
             const std::map<int, std::list<EBOOKOutputElement *> > *footers,
             const std::map<int, std::list<EBOOKOutputElement *> > *headers) const override;
private:
  void _writeElements(librevenge::RVNGTextInterface *iface, int id,
                      const std::map<int, std::list<EBOOKOutputElement *> > *elements) const;
  librevenge::RVNGPropertyList m_propList;
  int m_footer;
  int m_footerLeft;
  int m_footerFirst;
  int m_footerLast;
  int m_header;
  int m_headerLeft;
  int m_headerFirst;
  int m_headerLast;
};

class OpenParagraphElement : public EBOOKOutputElement
{
public:
  OpenParagraphElement(const librevenge::RVNGPropertyList &propList) :
    m_propList(propList) {}
  ~OpenParagraphElement() override {}
  void write(librevenge::RVNGTextInterface *iface,
             const std::map<int, std::list<EBOOKOutputElement *> > *footers,
             const std::map<int, std::list<EBOOKOutputElement *> > *headers) const override;
private:
  librevenge::RVNGPropertyList m_propList;
};

class OpenSectionElement : public EBOOKOutputElement
{
public:
  OpenSectionElement(const librevenge::RVNGPropertyList &propList) :
    m_propList(propList) {}
  ~OpenSectionElement() override {}
  void write(librevenge::RVNGTextInterface *iface,
             const std::map<int, std::list<EBOOKOutputElement *> > *footers,
             const std::map<int, std::list<EBOOKOutputElement *> > *headers) const override;
private:
  librevenge::RVNGPropertyList m_propList;
};

class OpenSpanElement : public EBOOKOutputElement
{
public:
  OpenSpanElement(const librevenge::RVNGPropertyList &propList) :
    m_propList(propList) {}
  ~OpenSpanElement() override {}
  void write(librevenge::RVNGTextInterface *iface,
             const std::map<int, std::list<EBOOKOutputElement *> > *footers,
             const std::map<int, std::list<EBOOKOutputElement *> > *headers) const override;
private:
  librevenge::RVNGPropertyList m_propList;
};

class OpenTableElement : public EBOOKOutputElement
{
public:
  OpenTableElement(const librevenge::RVNGPropertyList &propList) :
    m_propList(propList) {}
  ~OpenTableElement() override {}
  void write(librevenge::RVNGTextInterface *iface,
             const std::map<int, std::list<EBOOKOutputElement *> > *footers,
             const std::map<int, std::list<EBOOKOutputElement *> > *headers) const override;
private:
  librevenge::RVNGPropertyList m_propList;
};

class OpenTableCellElement : public EBOOKOutputElement
{
public:
  OpenTableCellElement(const librevenge::RVNGPropertyList &propList) :
    m_propList(propList) {}
  ~OpenTableCellElement() override {}
  void write(librevenge::RVNGTextInterface *iface,
             const std::map<int, std::list<EBOOKOutputElement *> > *footers,
             const std::map<int, std::list<EBOOKOutputElement *> > *headers) const override;
private:
  librevenge::RVNGPropertyList m_propList;
};

class OpenTableRowElement : public EBOOKOutputElement
{
public:
  OpenTableRowElement(const librevenge::RVNGPropertyList &propList) :
    m_propList(propList) {}
  ~OpenTableRowElement() override {}
  void write(librevenge::RVNGTextInterface *iface,
             const std::map<int, std::list<EBOOKOutputElement *> > *footers,
             const std::map<int, std::list<EBOOKOutputElement *> > *headers) const override;
private:
  librevenge::RVNGPropertyList m_propList;
};

class OpenUnorderedListLevelElement : public EBOOKOutputElement
{
public:
  OpenUnorderedListLevelElement(const librevenge::RVNGPropertyList &propList) :
    m_propList(propList) {}
  ~OpenUnorderedListLevelElement() override {}
  void write(librevenge::RVNGTextInterface *iface,
             const std::map<int, std::list<EBOOKOutputElement *> > *footers,
             const std::map<int, std::list<EBOOKOutputElement *> > *headers) const override;
private:
  librevenge::RVNGPropertyList m_propList;
};

}

void CloseEndnoteElement::write(librevenge::RVNGTextInterface *iface,
                                const std::map<int, std::list<EBOOKOutputElement *> > *,
                                const std::map<int, std::list<EBOOKOutputElement *> > *) const
{
  if (iface)
    iface->closeEndnote();
}

void CloseFooterElement::write(librevenge::RVNGTextInterface *iface,
                               const std::map<int, std::list<EBOOKOutputElement *> > *,
                               const std::map<int, std::list<EBOOKOutputElement *> > *) const
{
  if (iface)
    iface->closeFooter();
}

void CloseFootnoteElement::write(librevenge::RVNGTextInterface *iface,
                                 const std::map<int, std::list<EBOOKOutputElement *> > *,
                                 const std::map<int, std::list<EBOOKOutputElement *> > *) const
{
  if (iface)
    iface->closeFootnote();
}

void CloseFrameElement::write(librevenge::RVNGTextInterface *iface,
                              const std::map<int, std::list<EBOOKOutputElement *> > *,
                              const std::map<int, std::list<EBOOKOutputElement *> > *) const
{
  if (iface)
    iface->closeFrame();
}

void CloseHeaderElement::write(librevenge::RVNGTextInterface *iface,
                               const std::map<int, std::list<EBOOKOutputElement *> > *,
                               const std::map<int, std::list<EBOOKOutputElement *> > *) const
{
  if (iface)
    iface->closeHeader();
}

void CloseLinkElement::write(librevenge::RVNGTextInterface *iface,
                             const std::map<int, std::list<EBOOKOutputElement *> > *,
                             const std::map<int, std::list<EBOOKOutputElement *> > *) const
{
  if (iface)
    iface->closeLink();
}

void CloseListElementElement::write(librevenge::RVNGTextInterface *iface,
                                    const std::map<int, std::list<EBOOKOutputElement *> > *,
                                    const std::map<int, std::list<EBOOKOutputElement *> > *) const
{
  if (iface)
    iface->closeListElement();
}

void CloseOrderedListLevelElement::write(librevenge::RVNGTextInterface *iface,
                                         const std::map<int, std::list<EBOOKOutputElement *> > *,
                                         const std::map<int, std::list<EBOOKOutputElement *> > *) const
{
  if (iface)
    iface->closeOrderedListLevel();
}

void ClosePageSpanElement::write(librevenge::RVNGTextInterface *iface,
                                 const std::map<int, std::list<EBOOKOutputElement *> > *,
                                 const std::map<int, std::list<EBOOKOutputElement *> > *) const
{
  if (iface)
    iface->closePageSpan();
}

void CloseParagraphElement::write(librevenge::RVNGTextInterface *iface,
                                  const std::map<int, std::list<EBOOKOutputElement *> > *,
                                  const std::map<int, std::list<EBOOKOutputElement *> > *) const
{
  if (iface)
    iface->closeParagraph();
}

void CloseSectionElement::write(librevenge::RVNGTextInterface *iface,
                                const std::map<int, std::list<EBOOKOutputElement *> > *,
                                const std::map<int, std::list<EBOOKOutputElement *> > *) const
{
  if (iface)
    iface->closeSection();
}

void CloseSpanElement::write(librevenge::RVNGTextInterface *iface,
                             const std::map<int, std::list<EBOOKOutputElement *> > *,
                             const std::map<int, std::list<EBOOKOutputElement *> > *) const
{
  if (iface)
    iface->closeSpan();
}

void CloseTableElement::write(librevenge::RVNGTextInterface *iface,
                              const std::map<int, std::list<EBOOKOutputElement *> > *,
                              const std::map<int, std::list<EBOOKOutputElement *> > *) const
{
  if (iface)
    iface->closeTable();
}

void CloseTableCellElement::write(librevenge::RVNGTextInterface *iface,
                                  const std::map<int, std::list<EBOOKOutputElement *> > *,
                                  const std::map<int, std::list<EBOOKOutputElement *> > *) const
{
  if (iface)
    iface->closeTableCell();
}

void CloseTableRowElement::write(librevenge::RVNGTextInterface *iface,
                                 const std::map<int, std::list<EBOOKOutputElement *> > *,
                                 const std::map<int, std::list<EBOOKOutputElement *> > *) const
{
  if (iface)
    iface->closeTableRow();
}

void CloseUnorderedListLevelElement::write(librevenge::RVNGTextInterface *iface,
                                           const std::map<int, std::list<EBOOKOutputElement *> > *,
                                           const std::map<int, std::list<EBOOKOutputElement *> > *) const
{
  if (iface)
    iface->closeUnorderedListLevel();
}

void InsertBinaryObjectElement::write(librevenge::RVNGTextInterface *iface,
                                      const std::map<int, std::list<EBOOKOutputElement *> > *,
                                      const std::map<int, std::list<EBOOKOutputElement *> > *) const
{
  if (iface)
    iface->insertBinaryObject(m_propList);
}

void InsertCoveredTableCellElement::write(librevenge::RVNGTextInterface *iface,
                                          const std::map<int, std::list<EBOOKOutputElement *> > *,
                                          const std::map<int, std::list<EBOOKOutputElement *> > *) const
{
  if (iface)
    iface->insertCoveredTableCell(m_propList);
}

void InsertLineBreakElement::write(librevenge::RVNGTextInterface *iface,
                                   const std::map<int, std::list<EBOOKOutputElement *> > *,
                                   const std::map<int, std::list<EBOOKOutputElement *> > *) const
{
  if (iface)
    iface->insertLineBreak();
}

void InsertSpaceElement::write(librevenge::RVNGTextInterface *iface,
                               const std::map<int, std::list<EBOOKOutputElement *> > *,
                               const std::map<int, std::list<EBOOKOutputElement *> > *) const
{
  if (iface)
    iface->insertSpace();
}

void InsertTabElement::write(librevenge::RVNGTextInterface *iface,
                             const std::map<int, std::list<EBOOKOutputElement *> > *,
                             const std::map<int, std::list<EBOOKOutputElement *> > *) const
{
  if (iface)
    iface->insertTab();
}

void InsertTextElement::write(librevenge::RVNGTextInterface *iface,
                              const std::map<int, std::list<EBOOKOutputElement *> > *,
                              const std::map<int, std::list<EBOOKOutputElement *> > *) const
{
  if (iface)
    iface->insertText(m_text);
}

void OpenEndnoteElement::write(librevenge::RVNGTextInterface *iface,
                               const std::map<int, std::list<EBOOKOutputElement *> > *,
                               const std::map<int, std::list<EBOOKOutputElement *> > *) const
{
  if (iface)
    iface->openEndnote(m_propList);
}

void OpenFooterElement::write(librevenge::RVNGTextInterface *iface,
                              const std::map<int, std::list<EBOOKOutputElement *> > *,
                              const std::map<int, std::list<EBOOKOutputElement *> > *) const
{
  if (iface)
    iface->openFooter(m_propList);
}

void OpenFootnoteElement::write(librevenge::RVNGTextInterface *iface,
                                const std::map<int, std::list<EBOOKOutputElement *> > *,
                                const std::map<int, std::list<EBOOKOutputElement *> > *) const
{
  if (iface)
    iface->openFootnote(m_propList);
}

void OpenFrameElement::write(librevenge::RVNGTextInterface *iface,
                             const std::map<int, std::list<EBOOKOutputElement *> > *,
                             const std::map<int, std::list<EBOOKOutputElement *> > *) const
{
  if (iface)
    iface->openFrame(m_propList);
}

void OpenHeaderElement::write(librevenge::RVNGTextInterface *iface,
                              const std::map<int, std::list<EBOOKOutputElement *> > *,
                              const std::map<int, std::list<EBOOKOutputElement *> > *) const
{
  if (iface)
    iface->openHeader(m_propList);
}

void OpenLinkElement::write(librevenge::RVNGTextInterface *iface,
                            const std::map<int, std::list<EBOOKOutputElement *> > *,
                            const std::map<int, std::list<EBOOKOutputElement *> > *) const
{
  if (iface)
    iface->openLink(m_propList);
}

void OpenListElementElement::write(librevenge::RVNGTextInterface *iface,
                                   const std::map<int, std::list<EBOOKOutputElement *> > *,
                                   const std::map<int, std::list<EBOOKOutputElement *> > *) const
{
  if (iface)
    iface->openListElement(m_propList);
}

void OpenOrderedListLevelElement::write(librevenge::RVNGTextInterface *iface,
                                        const std::map<int, std::list<EBOOKOutputElement *> > *,
                                        const std::map<int, std::list<EBOOKOutputElement *> > *) const
{
  if (iface)
    iface->openOrderedListLevel(m_propList);
}

void OpenPageSpanElement::write(librevenge::RVNGTextInterface *iface,
                                const std::map<int, std::list<EBOOKOutputElement *> > *footers,
                                const std::map<int, std::list<EBOOKOutputElement *> > *headers) const
{
  if (iface)
  {
    // open the page span
    iface->openPageSpan(m_propList);
    // write out the footers
    _writeElements(iface, m_footer, footers);
    _writeElements(iface, m_footerLeft, footers);
    _writeElements(iface, m_footerFirst, footers);
    _writeElements(iface, m_footerLast, footers);
    // write out the headers
    _writeElements(iface, m_header, headers);
    _writeElements(iface, m_headerLeft, headers);
    _writeElements(iface, m_headerFirst, headers);
    _writeElements(iface, m_headerLast, headers);
    // and continue with writing out the other stuff
  }

}

void OpenPageSpanElement::_writeElements(librevenge::RVNGTextInterface *iface, int id,
                                         const std::map<int, std::list<EBOOKOutputElement *> > *elements) const
{
  if (!elements || id < 0 || !iface)
    return;

  auto iterMap = elements->find(id);
  if (iterMap == elements->end() || iterMap->second.empty())
    return;

  for (auto iterVec : iterMap->second)
    iterVec->write(iface, nullptr, nullptr);
}

void OpenParagraphElement::write(librevenge::RVNGTextInterface *iface,
                                 const std::map<int, std::list<EBOOKOutputElement *> > *,
                                 const std::map<int, std::list<EBOOKOutputElement *> > *) const
{
  if (iface)
    iface->openParagraph(m_propList);
}

void OpenSectionElement::write(librevenge::RVNGTextInterface *iface,
                               const std::map<int, std::list<EBOOKOutputElement *> > *,
                               const std::map<int, std::list<EBOOKOutputElement *> > *) const
{
  if (iface)
    iface->openSection(m_propList);
}

void OpenSpanElement::write(librevenge::RVNGTextInterface *iface,
                            const std::map<int, std::list<EBOOKOutputElement *> > *,
                            const std::map<int, std::list<EBOOKOutputElement *> > *) const
{
  if (iface)
    iface->openSpan(m_propList);
}

void OpenTableElement::write(librevenge::RVNGTextInterface *iface,
                             const std::map<int, std::list<EBOOKOutputElement *> > *,
                             const std::map<int, std::list<EBOOKOutputElement *> > *) const
{
  if (iface)
    iface->openTable(m_propList);
}

void OpenTableCellElement::write(librevenge::RVNGTextInterface *iface,
                                 const std::map<int, std::list<EBOOKOutputElement *> > *,
                                 const std::map<int, std::list<EBOOKOutputElement *> > *) const
{
  if (iface)
    iface->openTableCell(m_propList);
}

void OpenTableRowElement::write(librevenge::RVNGTextInterface *iface,
                                const std::map<int, std::list<EBOOKOutputElement *> > *,
                                const std::map<int, std::list<EBOOKOutputElement *> > *) const
{
  if (iface)
    iface->openTableRow(m_propList);
}

void OpenUnorderedListLevelElement::write(librevenge::RVNGTextInterface *iface,
                                          const std::map<int, std::list<EBOOKOutputElement *> > *,
                                          const std::map<int, std::list<EBOOKOutputElement *> > *) const
{
  if (iface)
    iface->openUnorderedListLevel(m_propList);
}

// EBOOKOutputElements

EBOOKOutputElements::EBOOKOutputElements()
  : m_bodyElements(), m_headerElements(), m_footerElements(), m_elements(nullptr)
{
  m_elements = &m_bodyElements;
}

EBOOKOutputElements::~EBOOKOutputElements()
{
  m_elements = nullptr;
  std::list<EBOOKOutputElement *>::iterator iterVec;
  for (iterVec = m_bodyElements.begin(); iterVec != m_bodyElements.end(); ++iterVec)
    delete (*iterVec);
  std::map<int, std::list<EBOOKOutputElement *> >::iterator iterMap;
  for (iterMap = m_headerElements.begin(); iterMap != m_headerElements.end(); ++iterMap)
  {
    for (iterVec = iterMap->second.begin(); iterVec != iterMap->second.end(); ++iterVec)
      delete (*iterVec);
  }
  for (iterMap = m_footerElements.begin(); iterMap != m_footerElements.end(); ++iterMap)
  {
    for (iterVec = iterMap->second.begin(); iterVec != iterMap->second.end(); ++iterVec)
      delete (*iterVec);
  }
}

void EBOOKOutputElements::write(librevenge::RVNGTextInterface *iface) const
{
  std::list<EBOOKOutputElement *>::const_iterator iter;
  for (iter = m_bodyElements.begin(); iter != m_bodyElements.end(); ++iter)
    (*iter)->write(iface, &m_footerElements, &m_headerElements);
}

void EBOOKOutputElements::addCloseEndnote()
{
  if (m_elements)
    m_elements->push_back(new CloseEndnoteElement());
}

void EBOOKOutputElements::addCloseFooter()
{
  if (m_elements)
    m_elements->push_back(new CloseFooterElement());
  m_elements = &m_bodyElements;
}

void EBOOKOutputElements::addCloseFootnote()
{
  if (m_elements)
    m_elements->push_back(new CloseFootnoteElement());
}

void EBOOKOutputElements::addCloseFrame()
{
  if (m_elements)
    m_elements->push_back(new CloseFrameElement());
}

void EBOOKOutputElements::addCloseHeader()
{
  if (m_elements)
    m_elements->push_back(new CloseHeaderElement());
  m_elements = &m_bodyElements;
}

void EBOOKOutputElements::addCloseLink()
{
  if (m_elements)
    m_elements->push_back(new CloseLinkElement());
}

void EBOOKOutputElements::addCloseListElement()
{
  if (m_elements)
    m_elements->push_back(new CloseListElementElement());
}

void EBOOKOutputElements::addCloseOrderedListLevel()
{
  if (m_elements)
    m_elements->push_back(new CloseOrderedListLevelElement());
}

void EBOOKOutputElements::addClosePageSpan()
{
  if (m_elements)
    m_elements->push_back(new ClosePageSpanElement());
}

void EBOOKOutputElements::addCloseParagraph()
{
  if (m_elements)
    m_elements->push_back(new CloseParagraphElement());
}

void EBOOKOutputElements::addCloseSection()
{
  if (m_elements)
    m_elements->push_back(new CloseSectionElement());
}

void EBOOKOutputElements::addCloseSpan()
{
  if (m_elements)
    m_elements->push_back(new CloseSpanElement());
}

void EBOOKOutputElements::addCloseTable()
{
  if (m_elements)
    m_elements->push_back(new CloseTableElement());
}

void EBOOKOutputElements::addCloseTableCell()
{
  if (m_elements)
    m_elements->push_back(new CloseTableCellElement());
}

void EBOOKOutputElements::addCloseTableRow()
{
  if (m_elements)
    m_elements->push_back(new CloseTableRowElement());
}

void EBOOKOutputElements::addCloseUnorderedListLevel()
{
  if (m_elements)
    m_elements->push_back(new CloseUnorderedListLevelElement());
}

void EBOOKOutputElements::addInsertBinaryObject(const librevenge::RVNGPropertyList &propList)
{
  if (m_elements)
    m_elements->push_back(new InsertBinaryObjectElement(propList));
}

void EBOOKOutputElements::addInsertCoveredTableCell(const librevenge::RVNGPropertyList &propList)
{
  if (m_elements)
    m_elements->push_back(new InsertCoveredTableCellElement(propList));
}

void EBOOKOutputElements::addInsertLineBreak()
{
  if (m_elements)
    m_elements->push_back(new InsertLineBreakElement());
}

void EBOOKOutputElements::addInsertSpace()
{
  if (m_elements)
    m_elements->push_back(new InsertSpaceElement());
}

void EBOOKOutputElements::addInsertTab()
{
  if (m_elements)
    m_elements->push_back(new InsertTabElement());
}

void EBOOKOutputElements::addInsertText(const librevenge::RVNGString &text)
{
  if (m_elements)
    m_elements->push_back(new InsertTextElement(text));
}

void EBOOKOutputElements::addOpenEndnote(const librevenge::RVNGPropertyList &propList)
{
  if (m_elements)
    m_elements->push_back(new OpenEndnoteElement(propList));
}

void EBOOKOutputElements::addOpenFooter(const librevenge::RVNGPropertyList &propList, int id)
{
  // if the corresponding element of the map does not exist, this will default-construct it.
  // In that case we will get an empty list to fill with the footer content. If the element
  // already exists, this might be a footer with different occurrence and we will add it to
  // the existing one.
  m_elements = &m_footerElements[id];
  if (m_elements)
    m_elements->push_back(new OpenFooterElement(propList));
}

void EBOOKOutputElements::addOpenFootnote(const librevenge::RVNGPropertyList &propList)
{
  if (m_elements)
    m_elements->push_back(new OpenFootnoteElement(propList));
}

void EBOOKOutputElements::addOpenFrame(const librevenge::RVNGPropertyList &propList)
{
  if (m_elements)
    m_elements->push_back(new OpenFrameElement(propList));
}

void EBOOKOutputElements::addOpenHeader(const librevenge::RVNGPropertyList &propList, int id)
{
  // Check the comment in addOpenFooter to see what happens here
  m_elements = &m_headerElements[id];
  if (m_elements)
    m_elements->push_back(new OpenHeaderElement(propList));
}

void EBOOKOutputElements::addOpenLink(const librevenge::RVNGPropertyList &propList)
{
  if (m_elements)
    m_elements->push_back(new OpenLinkElement(propList));
}

void EBOOKOutputElements::addOpenListElement(const librevenge::RVNGPropertyList &propList)
{
  if (m_elements)
    m_elements->push_back(new OpenListElementElement(propList));
}

void EBOOKOutputElements::addOpenOrderedListLevel(const librevenge::RVNGPropertyList &propList)
{
  if (m_elements)
    m_elements->push_back(new OpenOrderedListLevelElement(propList));
}

void EBOOKOutputElements::addOpenPageSpan(const librevenge::RVNGPropertyList &propList)
{
  addOpenPageSpan(propList, 0, 0, 0, 0, 0, 0, 0, 0);
}

void EBOOKOutputElements::addOpenPageSpan(const librevenge::RVNGPropertyList &propList,
                                          int footer, int footerLeft, int footerFirst, int footerLast,
                                          int header, int headerLeft, int headerFirst, int headerLast)
{
  if (m_elements)
    m_elements->push_back(new OpenPageSpanElement(propList, footer, footerLeft, footerFirst, footerLast,
                                                  header, headerLeft, headerFirst, headerLast));
}

void EBOOKOutputElements::addOpenParagraph(const librevenge::RVNGPropertyList &propList)
{
  if (m_elements)
    m_elements->push_back(new OpenParagraphElement(propList));
}

void EBOOKOutputElements::addOpenSection(const librevenge::RVNGPropertyList &propList)
{
  if (m_elements)
    m_elements->push_back(new OpenSectionElement(propList));
}

void EBOOKOutputElements::addOpenSpan(const librevenge::RVNGPropertyList &propList)
{
  if (m_elements)
    m_elements->push_back(new OpenSpanElement(propList));
}

void EBOOKOutputElements::addOpenTable(const librevenge::RVNGPropertyList &propList)
{
  if (m_elements)
    m_elements->push_back(new OpenTableElement(propList));
}

void EBOOKOutputElements::addOpenTableCell(const librevenge::RVNGPropertyList &propList)
{
  if (m_elements)
    m_elements->push_back(new OpenTableCellElement(propList));
}

void EBOOKOutputElements::addOpenTableRow(const librevenge::RVNGPropertyList &propList)
{
  if (m_elements)
    m_elements->push_back(new OpenTableRowElement(propList));
}

void EBOOKOutputElements::addOpenUnorderedListLevel(const librevenge::RVNGPropertyList &propList)
{
  if (m_elements)
    m_elements->push_back(new OpenUnorderedListLevelElement(propList));
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
