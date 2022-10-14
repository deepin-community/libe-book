/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "FictionBook2Collector.h"
#include "FictionBook2ParserContext.h"
#include "FictionBook2TextContext.h"
#include "FictionBook2Token.h"

namespace libebook
{

FictionBook2ParserContext::FictionBook2ParserContext(FictionBook2ParserContext *const parentContext, FictionBook2Collector *const collector)
  : m_collector(collector ? collector : (parentContext ? parentContext->m_collector : nullptr))
  , m_parentContext(parentContext)
{
}

FictionBook2XMLParserContext *FictionBook2ParserContext::leaveContext() const
{
  FictionBook2ParserContext *const parentContext = m_parentContext;
  delete this;
  return parentContext;
}

FictionBook2Collector *FictionBook2ParserContext::getCollector() const
{
  return m_collector;
}

FictionBook2ParserContext *FictionBook2ParserContext::getParentContext() const
{
  return m_parentContext;
}

FictionBook2NodeContextBase::FictionBook2NodeContextBase(FictionBook2ParserContext *const parentContext, FictionBook2Collector *const collector)
  : FictionBook2ParserContext(parentContext, collector)
{
}

void FictionBook2NodeContextBase::startOfElement()
{
}

void FictionBook2NodeContextBase::endOfAttributes()
{
}

void FictionBook2NodeContextBase::text(const char *)
{
}

FictionBook2DataContextBase::FictionBook2DataContextBase(FictionBook2ParserContext *const parentContext)
  : FictionBook2ParserContext(parentContext)
{
}

FictionBook2XMLParserContext *FictionBook2DataContextBase::element(const FictionBook2TokenData &, const FictionBook2TokenData &)
{
  return nullptr;
}

void FictionBook2DataContextBase::startOfElement()
{
}

void FictionBook2DataContextBase::endOfAttributes()
{
}

void FictionBook2DataContextBase::text(const char *)
{
}

FictionBook2TextContextBase::FictionBook2TextContextBase(FictionBook2ParserContext *const parentContext)
  : FictionBook2ParserContext(parentContext)
{
}

FictionBook2XMLParserContext *FictionBook2TextContextBase::element(const FictionBook2TokenData &, const FictionBook2TokenData &)
{
  return nullptr;
}

void FictionBook2TextContextBase::startOfElement()
{
}

void FictionBook2TextContextBase::endOfElement()
{
}

void FictionBook2TextContextBase::attribute(const FictionBook2TokenData &, const FictionBook2TokenData *, const char *)
{
}

void FictionBook2TextContextBase::endOfAttributes()
{
}

FictionBook2SkipElementContext::FictionBook2SkipElementContext(FictionBook2ParserContext *const parentContext)
  : FictionBook2ParserContext(parentContext)
  , m_level(1)
{
}

FictionBook2XMLParserContext *FictionBook2SkipElementContext::leaveContext() const
{
  if (m_level)
    return const_cast<FictionBook2SkipElementContext *>(this);

  FictionBook2ParserContext *const parentContext = getParentContext();
  delete this;
  return parentContext;
}

FictionBook2XMLParserContext *FictionBook2SkipElementContext::element(const FictionBook2TokenData &, const FictionBook2TokenData &)
{
  ++m_level;
  return this;
}

void FictionBook2SkipElementContext::startOfElement()
{
}

void FictionBook2SkipElementContext::endOfElement()
{
  --m_level;
}

void FictionBook2SkipElementContext::attribute(const FictionBook2TokenData &, const FictionBook2TokenData *, const char *)
{
}

void FictionBook2SkipElementContext::endOfAttributes()
{
}

void FictionBook2SkipElementContext::text(const char *)
{
}

FictionBook2StyleContextBase::FictionBook2StyleContextBase(FictionBook2ParserContext *parentContext, const FictionBook2Style &style)
  : FictionBook2ParserContext(parentContext)
  , m_style(style)
{
}

FictionBook2Style &FictionBook2StyleContextBase::getStyle()
{
  return m_style;
}

FictionBook2TextFormat &FictionBook2StyleContextBase::getTextFormat()
{
  return m_style.getTextFormat();
}

FictionBook2XMLParserContext *FictionBook2StyleContextBase::element(const FictionBook2TokenData &name, const FictionBook2TokenData &ns)
{
  if (FictionBook2Token::NS_FICTIONBOOK == getFictionBook2TokenID(ns))
  {
    switch (getFictionBook2TokenID(name))
    {
    case FictionBook2Token::strong :
      return new FictionBook2StrongContext(this, getStyle());
    case FictionBook2Token::emphasis :
      return new FictionBook2EmphasisContext(this, getStyle());
    case FictionBook2Token::style :
      return new FictionBook2StyleContext(this, getStyle());
    case FictionBook2Token::a :
      return new FictionBook2AContext(this, getStyle());
    case FictionBook2Token::strikethrough :
      return new FictionBook2StrikethroughContext(this, getStyle());
    case FictionBook2Token::sub :
      return new FictionBook2SubContext(this, getStyle());
    case FictionBook2Token::sup :
      return new FictionBook2SupContext(this, getStyle());
    case FictionBook2Token::code :
      return new FictionBook2CodeContext(this, getStyle());
    case FictionBook2Token::image :
      return new FictionBook2InlineImageContext(this, getStyle());
    default :
      break;
    }
  }

  return new FictionBook2SkipElementContext(this);
}

void FictionBook2StyleContextBase::endOfElement()
{
}

void FictionBook2StyleContextBase::endOfAttributes()
{
}

void FictionBook2StyleContextBase::attribute(const FictionBook2TokenData &name, const FictionBook2TokenData *ns, const char *const value)
{
  if ((FictionBook2Token::NS_XML == getFictionBook2TokenID(ns)) && (FictionBook2Token::lang == getFictionBook2TokenID(name)))
    m_style.getTextFormat().lang = value;
}

void FictionBook2StyleContextBase::text(const char *value)
{
  getCollector()->openSpan(getStyle());
  getCollector()->insertText(value);
  getCollector()->closeSpan();
}

FictionBook2BlockFormatContextBase::FictionBook2BlockFormatContextBase(FictionBook2ParserContext *parentContext, const FictionBook2BlockFormat &format)
  : FictionBook2NodeContextBase(parentContext)
  , m_format(format)
{
}

FictionBook2BlockFormat &FictionBook2BlockFormatContextBase::getBlockFormat()
{
  return m_format;
}

const FictionBook2BlockFormat &FictionBook2BlockFormatContextBase::getBlockFormat() const
{
  return m_format;
}

void FictionBook2BlockFormatContextBase::setLang(const char *const lang)
{
  m_format.lang = lang;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
