/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef EBOOKXMLPARSER_H_INCLUDED
#define EBOOKXMLPARSER_H_INCLUDED

#include "libebook_utils.h"

namespace libebook
{

/** Base class for simple SAX-like XML parser.
  *
  * There are two interfaces a derived class can use: the first one
  * expects that the derived class uses a tokenizer and therefore passes
  * token IDs to the callbacks (this is preferred). The other one uses
  * strings (which can of course be used in combination with token IDs,
  * if the XML format does allow foreign elements or attributes).
  *
  * Calls of startElement / endElement (or startElementByName /
  * endElementByName) are always paired. That means that endElement (or
  * endElementByName) is emitted even for empty elements.
  */
class EBOOKSAXParser
{
  // disable copying
  EBOOKSAXParser(const EBOOKSAXParser &other);
  EBOOKSAXParser &operator=(const EBOOKSAXParser &other);

public:
  explicit EBOOKSAXParser(const RVNGInputStreamPtr_t &input);
  virtual ~EBOOKSAXParser() = 0;

  void parse();

  /** Get @c id of the element/attribute.
    *
    * The value is based on the tokenizer the derived class uses.
    *
    * @returns the @c id.
    */
  virtual int getId(const char *name, const char *ns) const = 0;

  virtual void startElement(int id) = 0;
  virtual void endElement(int id) = 0;
  virtual void attribute(int id, const char *value) = 0;

  virtual void startElementByName(const char *name, const char *ns) = 0;
  virtual void endElementByName(const char *name, const char *ns) = 0;
  virtual void attributeByName(const char *name, const char *ns, const char *value) = 0;

  virtual void text(const char *value) = 0;

private:
  const RVNGInputStreamPtr_t m_input;
};

}

#endif // EBOOKXMLPARSER_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
