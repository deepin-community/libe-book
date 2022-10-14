/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef FICTIONBOOK2PARSERCONTEXT_H_INCLUDED
#define FICTIONBOOK2PARSERCONTEXT_H_INCLUDED

#include "FictionBook2XMLParserContext.h"
#include "FictionBook2Style.h"

namespace libebook
{

class FictionBook2Collector;

/** Base class for all contexts.
  */
class FictionBook2ParserContext : public FictionBook2XMLParserContext
{
  // no copying
  FictionBook2ParserContext(const FictionBook2ParserContext &other);
  FictionBook2ParserContext &operator=(const FictionBook2ParserContext &other);

public:
  /** Create a new context with @c parentContext as parent.
    *
    * @param[in] parentContext A parent context.
    * @param[in] collector A collector for the context's content.
    */
  explicit FictionBook2ParserContext(FictionBook2ParserContext *parentContext, FictionBook2Collector *collector = nullptr);

  FictionBook2XMLParserContext *leaveContext() const override;

  /** Get the collector.
    *
    * @return The collector.
    */
  FictionBook2Collector *getCollector() const;

  /** Get the parent context.
    *
    * @return The parent context.
    */
  FictionBook2ParserContext *getParentContext() const;

private:
  FictionBook2Collector *const m_collector;
  FictionBook2ParserContext *const m_parentContext;
};

/** Base class for non-leaf element contexts.
  *
  * It provides empty implementations for functions that are typically
  * not needed for processing non-leaf elements.
  */
class FictionBook2NodeContextBase : public FictionBook2ParserContext
{
public:
  explicit FictionBook2NodeContextBase(FictionBook2ParserContext *parentContext, FictionBook2Collector *collector = nullptr);

private:
  void startOfElement() override;
  void endOfAttributes() override;
  void text(const char *value) override;
};

/** Base class for leaf data element contexts.
  *
  * It provides empty implementations for functions that are typically
  * not needed for processing leaf data elements (i.e., empty elements
  * that only have attributes).
  *
  * @note The parser inserts an artificial endOfElement() call for empty
  * elements.
  */
class FictionBook2DataContextBase : public FictionBook2ParserContext
{
public:
  explicit FictionBook2DataContextBase(FictionBook2ParserContext *parentContext);

private:
  FictionBook2XMLParserContext *element(const FictionBook2TokenData &name, const FictionBook2TokenData &ns) override;
  void startOfElement() override;
  void endOfAttributes() override;
  void text(const char *value) override;
};

/** Base class for leaf textual element contexts.
  *
  * It provides empty implementations for functions that are typically
  * not needed for processing leaf textual elements.
  */
class FictionBook2TextContextBase : public FictionBook2ParserContext
{
public:
  explicit FictionBook2TextContextBase(FictionBook2ParserContext *parentContext);

private:
  FictionBook2XMLParserContext *element(const FictionBook2TokenData &name, const FictionBook2TokenData &ns) override;
  void startOfElement() override;
  void endOfElement() override;
  void attribute(const FictionBook2TokenData &name, const FictionBook2TokenData *ns, const char *value) override;
  void endOfAttributes() override;
};

/** Context for skipping element hierarchies.
  */
class FictionBook2SkipElementContext : public FictionBook2ParserContext
{
public:
  explicit FictionBook2SkipElementContext(FictionBook2ParserContext *parentContext);

private:
  FictionBook2XMLParserContext *leaveContext() const override;

  FictionBook2XMLParserContext *element(const FictionBook2TokenData &name, const FictionBook2TokenData &ns) override;
  void startOfElement() override;
  void endOfElement() override;
  void attribute(const FictionBook2TokenData &name, const FictionBook2TokenData *ns, const char *value) override;
  void endOfAttributes() override;
  void text(const char *value) override;

private:
  unsigned m_level;
};

class FictionBook2StyleContextBase : public FictionBook2ParserContext
{
public:
  FictionBook2StyleContextBase(FictionBook2ParserContext *parentContext, const FictionBook2Style &style);

protected:
  FictionBook2Style &getStyle();
  FictionBook2TextFormat &getTextFormat();

  FictionBook2XMLParserContext *element(const FictionBook2TokenData &name, const FictionBook2TokenData &ns) override;
  void endOfElement() override;
  void attribute(const FictionBook2TokenData &name, const FictionBook2TokenData *ns, const char *value) override;
  void endOfAttributes() override;
  void text(const char *value) override;

private:
  FictionBook2Style m_style;
};

class FictionBook2BlockFormatContextBase : public FictionBook2NodeContextBase
{
public:
  FictionBook2BlockFormatContextBase(FictionBook2ParserContext *parentContext, const FictionBook2BlockFormat &format);

  FictionBook2BlockFormat &getBlockFormat();
  const FictionBook2BlockFormat &getBlockFormat() const;

  void setLang(const char *lang);

private:
  FictionBook2BlockFormat m_format;
};

}

#endif // FICTIONBOOK2PARSERCONTEXT_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
