/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef FICTIONBOOK2BLOCKCONTEXT_H_INCLUDED
#define FICTIONBOOK2BLOCKCONTEXT_H_INCLUDED

#include <string>

#include <boost/optional.hpp>

#include "FictionBook2ParserContext.h"

namespace libebook
{

class FictionBook2BodyContext : public FictionBook2NodeContextBase
{
public:
  FictionBook2BodyContext(FictionBook2ParserContext *parentContext, FictionBook2Collector *collector);

private:
  FictionBook2XMLParserContext *element(const FictionBook2TokenData &name, const FictionBook2TokenData &ns) override;
  void startOfElement() override;
  void endOfElement() override;
  void attribute(const FictionBook2TokenData &name, const FictionBook2TokenData *ns, const char *value) override;

private:
  boost::optional<std::string> m_lang;
};

class FictionBook2CiteContext : public FictionBook2BlockFormatContextBase
{
public:
  FictionBook2CiteContext(FictionBook2ParserContext *parentContext, const FictionBook2BlockFormat &format);

private:
  FictionBook2XMLParserContext *element(const FictionBook2TokenData &name, const FictionBook2TokenData &ns) override;
  void endOfElement() override;
  void attribute(const FictionBook2TokenData &name, const FictionBook2TokenData *ns, const char *value) override;
};

class FictionBook2EmptyLineContext : public FictionBook2DataContextBase
{
public:
  explicit FictionBook2EmptyLineContext(FictionBook2ParserContext *parentContext);

private:
  void endOfElement() override;
  void attribute(const FictionBook2TokenData &name, const FictionBook2TokenData *ns, const char *value) override;
};

class FictionBook2EpigraphContext : public FictionBook2BlockFormatContextBase
{
public:
  FictionBook2EpigraphContext(FictionBook2ParserContext *parentContext, const FictionBook2BlockFormat &format);

private:
  FictionBook2XMLParserContext *element(const FictionBook2TokenData &name, const FictionBook2TokenData &ns) override;
  void endOfElement() override;
  void attribute(const FictionBook2TokenData &name, const FictionBook2TokenData *ns, const char *value) override;
};

class FictionBook2ImageContext : public FictionBook2NodeContextBase
{
public:
  FictionBook2ImageContext(FictionBook2ParserContext *parentContext);

private:
  FictionBook2XMLParserContext *element(const FictionBook2TokenData &name, const FictionBook2TokenData &ns) override;
  void endOfElement() override;
  void attribute(const FictionBook2TokenData &name, const FictionBook2TokenData *ns, const char *value) override;

private:
  std::string m_href;
  std::string m_altText;
  bool m_valid;
};

class FictionBook2PoemContext : public FictionBook2BlockFormatContextBase
{
public:
  FictionBook2PoemContext(FictionBook2ParserContext *parentContext, const FictionBook2BlockFormat &format);

private:
  FictionBook2XMLParserContext *element(const FictionBook2TokenData &name, const FictionBook2TokenData &ns) override;
  void startOfElement() override;
  void endOfElement() override;
  void attribute(const FictionBook2TokenData &name, const FictionBook2TokenData *ns, const char *value) override;
};

class FictionBook2SectionContext : public FictionBook2NodeContextBase
{
public:
  FictionBook2SectionContext(FictionBook2ParserContext *parentContext, const boost::optional<std::string> &lang);
  FictionBook2SectionContext(FictionBook2ParserContext *parentContext, uint8_t level, const boost::optional<std::string> &lang);

private:
  FictionBook2XMLParserContext *element(const FictionBook2TokenData &name, const FictionBook2TokenData &ns) override;
  void endOfElement() override;
  void attribute(const FictionBook2TokenData &name, const FictionBook2TokenData *ns, const char *value) override;

  FictionBook2BlockFormat makeBlockFormat() const;

private:
  // The level of the section. A section directly in body is at level 1.
  boost::optional<uint8_t> m_level;
  boost::optional<std::string> m_lang;
};

class FictionBook2StanzaContext : public FictionBook2BlockFormatContextBase
{
public:
  FictionBook2StanzaContext(FictionBook2ParserContext *parentContext, const FictionBook2BlockFormat &format);

private:
  FictionBook2XMLParserContext *element(const FictionBook2TokenData &name, const FictionBook2TokenData &ns) override;
  void endOfElement() override;
  void attribute(const FictionBook2TokenData &name, const FictionBook2TokenData *ns, const char *value) override;
};

class FictionBook2TitleContext : public FictionBook2BlockFormatContextBase
{
public:
  FictionBook2TitleContext(FictionBook2ParserContext *const parentContext, const FictionBook2BlockFormat &format);

private:
  FictionBook2XMLParserContext *element(const FictionBook2TokenData &name, const FictionBook2TokenData &ns) override;
  void endOfElement() override;
  void attribute(const FictionBook2TokenData &name, const FictionBook2TokenData *ns, const char *value) override;
};

}

#endif // FICTIONBOOK2BLOCKCONTEXT_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
