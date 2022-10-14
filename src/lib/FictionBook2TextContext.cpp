/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "FictionBook2Collector.h"
#include "FictionBook2TextContext.h"
#include "FictionBook2Token.h"

namespace libebook
{

FictionBook2ParaContextBase::FictionBook2ParaContextBase(FictionBook2ParserContext *parentContext, const FictionBook2BlockFormat &format)
  : FictionBook2StyleContextBase(parentContext, FictionBook2Style(format))
{
}

void FictionBook2ParaContextBase::startOfElement()
{
  getCollector()->openParagraph(getStyle().getBlockFormat());
}

void FictionBook2ParaContextBase::endOfElement()
{
  getCollector()->closeParagraph();
}

void FictionBook2ParaContextBase::attribute(const FictionBook2TokenData &name, const FictionBook2TokenData *ns, const char *value)
{
  FictionBook2StyleContextBase::attribute(name, ns, value);

  if (FictionBook2_NO_NAMESPACE(ns))
  {
    switch (getFictionBook2TokenID(name))
    {
    case FictionBook2Token::id :
      getCollector()->defineID(value);
      break;
    case FictionBook2Token::style :
      // ignore
      break;
    default :
      break;
    }
  }
}

FictionBook2AContext::FictionBook2AContext(FictionBook2ParserContext *parentContext, const FictionBook2Style &style)
  : FictionBook2StyleContextBase(parentContext, style)
  , m_href()
  , m_valid(true)
  , m_note(false)
{
}

FictionBook2XMLParserContext *FictionBook2AContext::element(const FictionBook2TokenData &name, const FictionBook2TokenData &ns)
{
  if (m_note)
    return new FictionBook2SkipElementContext(this);
  else
    return FictionBook2StyleContextBase::element(name, ns);
}

void FictionBook2AContext::startOfElement()
{
}

void FictionBook2AContext::endOfElement()
{
  if (m_note)
    getCollector()->insertFootnote(m_href.c_str());
}

void FictionBook2AContext::endOfAttributes()
{
  if (!m_valid)
    m_note = false;

  if (m_note)
  {
    if ('#' == m_href[0])
      m_href = m_href.substr(1);
    else
      m_note = false;
  }
}

void FictionBook2AContext::attribute(const FictionBook2TokenData &name, const FictionBook2TokenData *ns, const char *value)
{
  if (FictionBook2Token::NS_XLINK == getFictionBook2TokenID(ns))
  {
    switch (getFictionBook2TokenID(name))
    {
    case FictionBook2Token::href :
      m_href = value;
      break;
    case FictionBook2Token::type :
      m_valid = FictionBook2Token::simple == getFictionBook2TokenID(value);
      break;
    default :
      break;
    }
  }
  else if (FictionBook2_NO_NAMESPACE(ns) && (FictionBook2Token::type == getFictionBook2TokenID(name)))
    m_note = FictionBook2Token::note == getFictionBook2TokenID(value);
}

void FictionBook2AContext::text(const char *value)
{
  if (!m_note)
  {
    getCollector()->openSpan(getStyle());
    getCollector()->insertText(value);
    getCollector()->closeSpan();
  }
}

FictionBook2CodeContext::FictionBook2CodeContext(FictionBook2ParserContext *parentContext, FictionBook2Style &style)
  : FictionBook2StyleContextBase(parentContext, style)
{
}

void FictionBook2CodeContext::startOfElement()
{
  getTextFormat().code = true;
}

FictionBook2EmphasisContext::FictionBook2EmphasisContext(FictionBook2ParserContext *parentContext, FictionBook2Style &style)
  : FictionBook2StyleContextBase(parentContext, style)
{
}

void FictionBook2EmphasisContext::startOfElement()
{
  getTextFormat().emphasis = true;
}

FictionBook2InlineImageContext::FictionBook2InlineImageContext(FictionBook2ParserContext *parentContext, const FictionBook2Style &style)
  : FictionBook2DataContextBase(parentContext)
  , m_style(style)
  , m_href()
  , m_altText()
  , m_valid(true)
{
}

void FictionBook2InlineImageContext::startOfElement()
{
}

void FictionBook2InlineImageContext::endOfElement()
{
  if (m_valid && ('#' != m_href[0]))
    m_valid = false;

  if (m_valid)
    getCollector()->insertBitmap(m_href.substr(1).c_str());
  else
  {
    getCollector()->openSpan(m_style);
    const std::string altText("[Image: " + m_altText + "]");
    getCollector()->insertText(altText.c_str());
    getCollector()->closeSpan();
  }
}

void FictionBook2InlineImageContext::endOfAttributes()
{
}

void FictionBook2InlineImageContext::attribute(const FictionBook2TokenData &name, const FictionBook2TokenData *ns, const char *value)
{
  if (FictionBook2_NO_NAMESPACE(ns) && (FictionBook2Token::alt == getFictionBook2TokenID(name)))
    m_altText = value;
  else if (FictionBook2Token::NS_XLINK == getFictionBook2TokenID(ns))
  {
    switch (getFictionBook2TokenID(name))
    {
    case FictionBook2Token::href :
      m_href = value;
      break;
    case FictionBook2Token::type :
      m_valid = FictionBook2Token::simple == getFictionBook2TokenID(value);
      break;
    default :
      break;
    }
  }
}

FictionBook2PContext::FictionBook2PContext(FictionBook2ParserContext *const parentContext, const FictionBook2BlockFormat &format)
  : FictionBook2ParaContextBase(parentContext, makeBlockFormat(format))
{
}

FictionBook2BlockFormat FictionBook2PContext::makeBlockFormat(const FictionBook2BlockFormat &format)
{
  FictionBook2BlockFormat outFormat(format);
  outFormat.p = true;
  return outFormat;
}

FictionBook2StrikethroughContext::FictionBook2StrikethroughContext(FictionBook2ParserContext *parentContext, FictionBook2Style &style)
  : FictionBook2StyleContextBase(parentContext, style)
{
}

void FictionBook2StrikethroughContext::startOfElement()
{
  getTextFormat().strikethrough = true;
}

FictionBook2StrongContext::FictionBook2StrongContext(FictionBook2ParserContext *parentContext, FictionBook2Style &style)
  : FictionBook2StyleContextBase(parentContext, style)
{
}

void FictionBook2StrongContext::startOfElement()
{
  getTextFormat().strong = true;
}

FictionBook2StyleContext::FictionBook2StyleContext(FictionBook2ParserContext *parentContext, FictionBook2Style &style)
  : FictionBook2StyleContextBase(parentContext, style)
{
}

void FictionBook2StyleContext::startOfElement()
{
  // TODO: implement me
}

FictionBook2SubContext::FictionBook2SubContext(FictionBook2ParserContext *parentContext, FictionBook2Style &style)
  : FictionBook2StyleContextBase(parentContext, style)
{
}

void FictionBook2SubContext::startOfElement()
{
  getTextFormat().sub = true;
}

FictionBook2SubtitleContext::FictionBook2SubtitleContext(FictionBook2ParserContext *parentContext, const FictionBook2BlockFormat &format)
  : FictionBook2ParaContextBase(parentContext, makeBlockFormat(format))
{
}

FictionBook2BlockFormat FictionBook2SubtitleContext::makeBlockFormat(const FictionBook2BlockFormat &format)
{
  FictionBook2BlockFormat outFormat(format);
  outFormat.subtitle = true;
  return outFormat;
}

FictionBook2SupContext::FictionBook2SupContext(FictionBook2ParserContext *parentContext, FictionBook2Style &style)
  : FictionBook2StyleContextBase(parentContext, style)
{
}

void FictionBook2SupContext::startOfElement()
{
  getTextFormat().sup = true;
}

FictionBook2TextAuthorContext::FictionBook2TextAuthorContext(FictionBook2ParserContext *parentContext, const FictionBook2BlockFormat &format)
  : FictionBook2PContext(parentContext, makeBlockFormat(format))
{
}

FictionBook2BlockFormat FictionBook2TextAuthorContext::makeBlockFormat(const FictionBook2BlockFormat &format)
{
  FictionBook2BlockFormat outFormat(format);
  outFormat.textAuthor = true;
  return outFormat;
}

FictionBook2VContext::FictionBook2VContext(FictionBook2ParserContext *parentContext, const FictionBook2BlockFormat &format)
  : FictionBook2ParaContextBase(parentContext, makeBlockFormat(format))
{
}

FictionBook2BlockFormat FictionBook2VContext::makeBlockFormat(const FictionBook2BlockFormat &format)
{
  FictionBook2BlockFormat outFormat(format);
  outFormat.v = true;
  return outFormat;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
