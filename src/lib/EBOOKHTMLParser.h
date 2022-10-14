/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef EBOOKHTMLPARSER_H_INCLUDED
#define EBOOKHTMLPARSER_H_INCLUDED

#include <memory>

#include <librevenge/librevenge.h>
#include <librevenge-stream/librevenge-stream.h>

#include "XMLTreeNode.h"

namespace libebook
{

class XMLCollector;
struct EBOOKHTMLMetadata;
class EBOOKHTMLParagraphAttributes;
class EBOOKHTMLSpanAttributes;
class EBOOKHTMLTableCellAttributes;
class EBOOKOutputElements;
class XMLTreeWalker;

class EBOOKHTMLParser
{
  struct State;

public:
  enum Dialect
  {
    DIALECT_UNKNOWN,
    DIALECT_HTML,
    DIALECT_XHTML
  };

public:
  EBOOKHTMLParser(librevenge::RVNGInputStream *input, Dialect dialect, librevenge::RVNGTextInterface *document);
  virtual ~EBOOKHTMLParser();

  void parse();
  const XMLTreeNodePtr_t readTree();

  virtual librevenge::RVNGInputStream *getImage(const char *path) = 0;
  virtual librevenge::RVNGInputStream *getObject(const char *path) = 0;
  virtual librevenge::RVNGInputStream *getStylesheet(const char *path) = 0;

private:
  const XMLTreeNodePtr_t readTreeImpl(std::string &encoding, bool &encodingChange);
  void processNode(const XMLTreeWalker &node);

  void startElement(const XMLTreeWalker &node);
  void endElement(const XMLTreeWalker &node);
  void text(const XMLTreeWalker &node);

  void startExternalElement(const XMLTreeWalker &node);
  void endExternalElement(const XMLTreeWalker &node);

  void sendParagraph(const XMLTreeWalker &node);
  void sendSpan(const XMLTreeWalker &node);
  void sendTableCell(const XMLTreeWalker &node);
  void sendPreformattedText(const char *ch, int len);
  void sendHeading(const XMLTreeWalker &node, int level);

private:
  std::shared_ptr<XMLCollector> m_collector;
  std::shared_ptr<State> m_state;
  std::shared_ptr<librevenge::RVNGInputStream> m_input;
  // const Dialect m_dialect;
  std::shared_ptr<librevenge::RVNGInputStream> m_workingInput;
};

}

#endif // EBOOKHTMLPARSER_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
