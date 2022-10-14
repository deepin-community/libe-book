/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef FICTIONBOOK2XMLPARSERCONTEXT_H_INCLUDED
#define FICTIONBOOK2XMLPARSERCONTEXT_H_INCLUDED

namespace libebook
{

struct FictionBook2TokenData;

class FictionBook2XMLParserContext
{
public:
  virtual ~FictionBook2XMLParserContext() = 0;

  /** Destroy this context and return parent.
    *
    * @todo It would be handier to use shared_ptr to manage the context
    * lifetimes.
    *
    * @return The parent context.
    */
  virtual FictionBook2XMLParserContext *leaveContext() const = 0;

  /** Create a context for parsing a child element.
    *
    * @return A new context or @c 0 if the child element cannot be handled.
    */
  virtual FictionBook2XMLParserContext *element(const FictionBook2TokenData &name, const FictionBook2TokenData &ns) = 0;

  /** Signalize the start of an element.
    *
    * This can be used if initialization of the context needs virtual
    * function calls.
    */
  virtual void startOfElement() = 0;

  /** Signalize the end of an element.
    */
  virtual void endOfElement() = 0;

  /** Process an attribute.
    *
    * Attributes are handled to the current element's context one by
    * one.
    */
  virtual void attribute(const FictionBook2TokenData &name, const FictionBook2TokenData *ns, const char *value) = 0;

  /** Signalize the end of attributes.
    */
  virtual void endOfAttributes() = 0;

  /** Process textual content of an element.
    *
    * This function can be called more than once if the element has
    * mixed content.
    */
  virtual void text(const char *value) = 0;
};

}

#endif // FICTIONBOOK2XMLPARSERCONTEXT_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
