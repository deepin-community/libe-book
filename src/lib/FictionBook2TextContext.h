/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef FICTIONBOOK2TEXTCONTEXT_H_INCLUDED
#define FICTIONBOOK2TEXTCONTEXT_H_INCLUDED

#include <string>

#include "FictionBook2ParserContext.h"

namespace libebook
{

class FictionBook2ParaContextBase : public FictionBook2StyleContextBase
{
public:
  FictionBook2ParaContextBase(FictionBook2ParserContext *parentContext, const FictionBook2BlockFormat &format);

private:
  void startOfElement() override;
  void endOfElement() override;
  void attribute(const FictionBook2TokenData &name, const FictionBook2TokenData *ns, const char *value) override;
};

class FictionBook2AContext : public FictionBook2StyleContextBase
{
public:
  FictionBook2AContext(FictionBook2ParserContext *parentContext, const FictionBook2Style &style);

private:
  FictionBook2XMLParserContext *element(const FictionBook2TokenData &name, const FictionBook2TokenData &ns) override;
  void startOfElement() override;
  void endOfElement() override;
  void endOfAttributes() override;
  void attribute(const FictionBook2TokenData &name, const FictionBook2TokenData *ns, const char *value) override;
  void text(const char *text) override;

private:
  std::string m_href;
  bool m_valid;
  bool m_note;
};

class FictionBook2CodeContext : public FictionBook2StyleContextBase
{
public:
  FictionBook2CodeContext(FictionBook2ParserContext *parentContext, FictionBook2Style &style);

private:
  void startOfElement() override;
};

class FictionBook2EmphasisContext : public FictionBook2StyleContextBase
{
public:
  FictionBook2EmphasisContext(FictionBook2ParserContext *parentContext, FictionBook2Style &style);

private:
  void startOfElement() override;
};

class FictionBook2InlineImageContext : public FictionBook2DataContextBase
{
public:
  FictionBook2InlineImageContext(FictionBook2ParserContext *parentContext, const FictionBook2Style &style);

private:
  void startOfElement() override;
  void endOfElement() override;
  void endOfAttributes() override;
  void attribute(const FictionBook2TokenData &name, const FictionBook2TokenData *ns, const char *value) override;

private:
  const FictionBook2Style &m_style;
  std::string m_href;
  std::string m_altText;
  bool m_valid;
};

class FictionBook2PContext : public FictionBook2ParaContextBase
{
public:
  FictionBook2PContext(FictionBook2ParserContext *parentContext, const FictionBook2BlockFormat &format);

private:
  static FictionBook2BlockFormat makeBlockFormat(const FictionBook2BlockFormat &format);
};

class FictionBook2StrikethroughContext : public FictionBook2StyleContextBase
{
public:
  FictionBook2StrikethroughContext(FictionBook2ParserContext *parentContext, FictionBook2Style &style);

private:
  void startOfElement() override;
};

class FictionBook2StrongContext : public FictionBook2StyleContextBase
{
public:
  FictionBook2StrongContext(FictionBook2ParserContext *parentContext, FictionBook2Style &style);

private:
  void startOfElement() override;
};

class FictionBook2StyleContext : public FictionBook2StyleContextBase
{
public:
  FictionBook2StyleContext(FictionBook2ParserContext *parentContext, FictionBook2Style &style);

private:
  void startOfElement() override;
};

class FictionBook2SubContext : public FictionBook2StyleContextBase
{
public:
  FictionBook2SubContext(FictionBook2ParserContext *parentContext, FictionBook2Style &style);

private:
  void startOfElement() override;
};

class FictionBook2SubtitleContext : public FictionBook2ParaContextBase
{
public:
  explicit FictionBook2SubtitleContext(FictionBook2ParserContext *parentContext, const FictionBook2BlockFormat &format);

private:
  static FictionBook2BlockFormat makeBlockFormat(const FictionBook2BlockFormat &format);
};

class FictionBook2SupContext : public FictionBook2StyleContextBase
{
public:
  FictionBook2SupContext(FictionBook2ParserContext *parentContext, FictionBook2Style &style);

private:
  void startOfElement() override;
};

class FictionBook2TextAuthorContext : public FictionBook2PContext
{
public:
  explicit FictionBook2TextAuthorContext(FictionBook2ParserContext *parentContext, const FictionBook2BlockFormat &format);

private:
  static FictionBook2BlockFormat makeBlockFormat(const FictionBook2BlockFormat &format);
};

class FictionBook2VContext : public FictionBook2ParaContextBase
{
public:
  explicit FictionBook2VContext(FictionBook2ParserContext *parentContext, const FictionBook2BlockFormat &format);

private:
  static FictionBook2BlockFormat makeBlockFormat(const FictionBook2BlockFormat &format);
};

}

#endif // FICTIONBOOK2TEXTCONTEXT_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
