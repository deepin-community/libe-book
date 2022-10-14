/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "FictionBook2BlockContext.h"
#include "FictionBook2Collector.h"
#include "FictionBook2MetadataContext.h"
#include "FictionBook2TableContext.h"
#include "FictionBook2TextContext.h"
#include "FictionBook2Token.h"

namespace libebook
{

FictionBook2BodyContext::FictionBook2BodyContext(FictionBook2ParserContext *const parentContext, FictionBook2Collector *const collector)
  : FictionBook2NodeContextBase(parentContext, collector)
  , m_lang()
{
}

FictionBook2XMLParserContext *FictionBook2BodyContext::element(const FictionBook2TokenData &name, const FictionBook2TokenData &ns)
{
  if (FictionBook2Token::NS_FICTIONBOOK == getFictionBook2TokenID(ns))
  {
    switch (getFictionBook2TokenID(name))
    {
    case FictionBook2Token::image :
      return new FictionBook2ImageContext(this);
    case FictionBook2Token::title :
    {
      FictionBook2BlockFormat format;
      format.headingLevel = 1;
      return new FictionBook2TitleContext(this, format);
    }
    case FictionBook2Token::epigraph :
      return new FictionBook2EpigraphContext(this, FictionBook2BlockFormat());
    case FictionBook2Token::section :
      return new FictionBook2SectionContext(this, 1, m_lang);
    default :
      break;
    }
  }

  return new FictionBook2SkipElementContext(this);
}

void FictionBook2BodyContext::startOfElement()
{
  getCollector()->openPageSpan();
}

void FictionBook2BodyContext::endOfElement()
{
  getCollector()->closePageSpan();
}

void FictionBook2BodyContext::attribute(const FictionBook2TokenData &name, const FictionBook2TokenData *ns, const char *const value)
{
  if ((FictionBook2Token::NS_XML == getFictionBook2TokenID(ns)) && (FictionBook2Token::lang == getFictionBook2TokenID(name)))
    m_lang = value;
}

FictionBook2CiteContext::FictionBook2CiteContext(FictionBook2ParserContext *parentContext, const FictionBook2BlockFormat &format)
  : FictionBook2BlockFormatContextBase(parentContext, format)
{
}

FictionBook2XMLParserContext *FictionBook2CiteContext::element(const FictionBook2TokenData &name, const FictionBook2TokenData &ns)
{
  if (FictionBook2Token::NS_FICTIONBOOK == getFictionBook2TokenID(ns))
  {
    switch (getFictionBook2TokenID(name))
    {
    case FictionBook2Token::p :
      return new FictionBook2PContext(this, getBlockFormat());
    case FictionBook2Token::poem :
      return new FictionBook2PoemContext(this, getBlockFormat());
    case FictionBook2Token::empty_line :
      return new FictionBook2EmptyLineContext(this);
    case FictionBook2Token::subtitle :
      return new FictionBook2SubtitleContext(this, getBlockFormat());
    case FictionBook2Token::table :
      return new FictionBook2TableContext(this, getBlockFormat());
    default :
      break;
    }
  }

  return new FictionBook2SkipElementContext(this);
}

void FictionBook2CiteContext::endOfElement()
{
}

void FictionBook2CiteContext::attribute(const FictionBook2TokenData &name, const FictionBook2TokenData *ns, const char *value)
{
  if ((FictionBook2_NO_NAMESPACE(ns)) && (FictionBook2Token::id == getFictionBook2TokenID(name)))
    getCollector()->defineID(value);
  else if ((FictionBook2Token::NS_XML == getFictionBook2TokenID(ns)) && (FictionBook2Token::lang == getFictionBook2TokenID(name)))
    setLang(value);
}

FictionBook2EmptyLineContext::FictionBook2EmptyLineContext(FictionBook2ParserContext *parentContext)
  : FictionBook2DataContextBase(parentContext)
{
}

void FictionBook2EmptyLineContext::endOfElement()
{
  getCollector()->openParagraph(FictionBook2BlockFormat());
  getCollector()->closeParagraph();
}

void FictionBook2EmptyLineContext::attribute(const FictionBook2TokenData &, const FictionBook2TokenData *, const char *)
{
}

FictionBook2EpigraphContext::FictionBook2EpigraphContext(FictionBook2ParserContext *const parentContext, const FictionBook2BlockFormat &format)
  : FictionBook2BlockFormatContextBase(parentContext, format)
{
}

FictionBook2XMLParserContext *FictionBook2EpigraphContext::element(const FictionBook2TokenData &name, const FictionBook2TokenData &ns)
{
  if (FictionBook2Token::NS_FICTIONBOOK == getFictionBook2TokenID(ns))
  {
    switch (getFictionBook2TokenID(name))
    {
    case FictionBook2Token::p :
      return new FictionBook2PContext(this, getBlockFormat());
    case FictionBook2Token::poem :
      return new FictionBook2PoemContext(this, getBlockFormat());
    case FictionBook2Token::cite :
      return new FictionBook2CiteContext(this, getBlockFormat());
    case FictionBook2Token::empty_line :
      return new FictionBook2EmptyLineContext(this);
    case FictionBook2Token::text_author :
      return new FictionBook2TextAuthorContext(this, getBlockFormat());
    default :
      break;
    }
  }

  return new FictionBook2SkipElementContext(this);
}

void FictionBook2EpigraphContext::endOfElement()
{
}

void FictionBook2EpigraphContext::attribute(const FictionBook2TokenData &name, const FictionBook2TokenData *ns, const char *value)
{
  if ((FictionBook2_NO_NAMESPACE(ns)) && (FictionBook2Token::id == getFictionBook2TokenID(name)))
    getCollector()->defineID(value);
}

FictionBook2ImageContext::FictionBook2ImageContext(FictionBook2ParserContext *parentContext)
  : FictionBook2NodeContextBase(parentContext)
  , m_href()
  , m_altText()
  , m_valid(true)
{
}

FictionBook2XMLParserContext *FictionBook2ImageContext::element(const FictionBook2TokenData &name, const FictionBook2TokenData &ns)
{
  // TODO: implement me
  if (FictionBook2Token::NS_FICTIONBOOK == getFictionBook2TokenID(ns))
  {
    switch (getFictionBook2TokenID(name))
    {
    default :
      break;
    }
  }

  return new FictionBook2SkipElementContext(this);
}

void FictionBook2ImageContext::endOfElement()
{
  if (m_valid && ('#' != m_href[0]))
    m_valid = false;

  if (m_valid)
  {
    try
    {
      getCollector()->insertBitmap(m_href.substr(1).c_str());
    }
    catch (...)
    {
      m_valid = false;
    }
  }

  if (!m_valid)
  {
    // The image is not internal or it is broken. Use alternative text.
    // TODO: maybe extend later to support external URLs?
    getCollector()->openParagraph(FictionBook2BlockFormat());
    getCollector()->openSpan(FictionBook2Style(FictionBook2BlockFormat()));
    const std::string altText = std::string("[Image") + (m_altText.empty() ? "" : ": ") + m_altText + "]";
    getCollector()->insertText(altText.c_str());
    getCollector()->closeSpan();
    getCollector()->closeParagraph();
  }
}

void FictionBook2ImageContext::attribute(const FictionBook2TokenData &name, const FictionBook2TokenData *ns, const char *value)
{
  if (FictionBook2_NO_NAMESPACE(ns))
  {
    switch (getFictionBook2TokenID(name))
    {
    case FictionBook2Token::alt :
      m_altText = value;
      break;
    case FictionBook2Token::id :
      getCollector()->defineID(value);
      break;
    case FictionBook2Token::title :
      // ignore
      break;
    default :
      break;
    }
  }
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

FictionBook2PoemContext::FictionBook2PoemContext(FictionBook2ParserContext *parentContext, const FictionBook2BlockFormat &format)
  : FictionBook2BlockFormatContextBase(parentContext, format)
{
}

FictionBook2XMLParserContext *FictionBook2PoemContext::element(const FictionBook2TokenData &name, const FictionBook2TokenData &ns)
{
  if (FictionBook2Token::NS_FICTIONBOOK == getFictionBook2TokenID(ns))
  {
    switch (getFictionBook2TokenID(name))
    {
    case FictionBook2Token::title :
      return new FictionBook2TitleContext(this, getBlockFormat());
    case FictionBook2Token::epigraph :
      return new FictionBook2EpigraphContext(this, getBlockFormat());
    case FictionBook2Token::stanza :
      return new FictionBook2StanzaContext(this, getBlockFormat());
    case FictionBook2Token::text_author :
      return new FictionBook2TextAuthorContext(this, getBlockFormat());
    case FictionBook2Token::date :
      // ignore
      break;
    default :
      break;
    }
  }

  return new FictionBook2SkipElementContext(this);
}

void FictionBook2PoemContext::startOfElement()
{
  getCollector()->openParagraph(FictionBook2BlockFormat());
  getCollector()->closeParagraph();
}

void FictionBook2PoemContext::endOfElement()
{
}

void FictionBook2PoemContext::attribute(const FictionBook2TokenData &name, const FictionBook2TokenData *ns, const char *value)
{
  if ((FictionBook2_NO_NAMESPACE(ns)) && (FictionBook2Token::id == getFictionBook2TokenID(name)))
    getCollector()->defineID(value);
  else if ((FictionBook2Token::NS_XML == getFictionBook2TokenID(ns)) && (FictionBook2Token::lang == getFictionBook2TokenID(name)))
    setLang(value);
}

FictionBook2SectionContext::FictionBook2SectionContext(FictionBook2ParserContext *const parentContext, const boost::optional<std::string> &lang)
  : FictionBook2NodeContextBase(parentContext)
  , m_level()
  , m_lang(lang)
{
}

FictionBook2SectionContext::FictionBook2SectionContext(FictionBook2ParserContext *const parentContext, const uint8_t level, const boost::optional<std::string> &lang)
  : FictionBook2NodeContextBase(parentContext)
  , m_level(level)
  , m_lang(lang)
{
}

FictionBook2XMLParserContext *FictionBook2SectionContext::element(const FictionBook2TokenData &name, const FictionBook2TokenData &ns)
{
  if (FictionBook2Token::NS_FICTIONBOOK == getFictionBook2TokenID(ns))
  {
    switch (getFictionBook2TokenID(name))
    {
    case FictionBook2Token::title :
    {
      FictionBook2BlockFormat format;
      if (bool(m_level))
        format.headingLevel = uint8_t(get(m_level) + 1);
      if (bool(m_lang))
        format.lang = get(m_lang);
      return new FictionBook2TitleContext(this, format);
    }
    case FictionBook2Token::epigraph :
      return new FictionBook2EpigraphContext(this, makeBlockFormat());
    case FictionBook2Token::image :
      return new FictionBook2ImageContext(this);
    case FictionBook2Token::annotation :
      return new FictionBook2AnnotationContext(this, makeBlockFormat());
    case FictionBook2Token::section :
      if (bool(m_level))
        return new FictionBook2SectionContext(this, uint8_t(get(m_level) + 1), m_lang);
      else
        return new FictionBook2SectionContext(this, m_lang);
    case FictionBook2Token::p :
      return new FictionBook2PContext(this, makeBlockFormat());
    case FictionBook2Token::poem :
      return new FictionBook2PoemContext(this, makeBlockFormat());
    case FictionBook2Token::subtitle :
      return new FictionBook2SubtitleContext(this, makeBlockFormat());
    case FictionBook2Token::cite :
      return new FictionBook2CiteContext(this, makeBlockFormat());
    case FictionBook2Token::empty_line :
      return new FictionBook2EmptyLineContext(this);
    case FictionBook2Token::table :
      return new FictionBook2TableContext(this, makeBlockFormat());
    default :
      break;
    }
  }

  return new FictionBook2SkipElementContext(this);
}

void FictionBook2SectionContext::endOfElement()
{
  // TODO: implement me
}

void FictionBook2SectionContext::attribute(const FictionBook2TokenData &name, const FictionBook2TokenData *ns, const char *value)
{
  if (FictionBook2_NO_NAMESPACE(ns) && (FictionBook2Token::id == getFictionBook2TokenID(name)))
    getCollector()->defineID(value);
  else if ((FictionBook2Token::NS_XML == getFictionBook2TokenID(ns)) && (FictionBook2Token::lang == getFictionBook2TokenID(name)))
    m_lang = value;
}

FictionBook2BlockFormat FictionBook2SectionContext::makeBlockFormat() const
{
  FictionBook2BlockFormat format;

  if (bool(m_lang))
    format.lang = get(m_lang);

  return format;
}

FictionBook2StanzaContext::FictionBook2StanzaContext(FictionBook2ParserContext *parentContext, const FictionBook2BlockFormat &format)
  : FictionBook2BlockFormatContextBase(parentContext, format)
{
  getBlockFormat().stanza = true;
}

FictionBook2XMLParserContext *FictionBook2StanzaContext::element(const FictionBook2TokenData &name, const FictionBook2TokenData &ns)
{
  if (FictionBook2Token::NS_FICTIONBOOK == getFictionBook2TokenID(ns))
  {
    switch (getFictionBook2TokenID(name))
    {
    case FictionBook2Token::title :
      return new FictionBook2TitleContext(this, getBlockFormat());
    case FictionBook2Token::subtitle :
      return new FictionBook2SubtitleContext(this, getBlockFormat());
    case FictionBook2Token::v :
      return new FictionBook2VContext(this, getBlockFormat());
    default :
      break;
    }
  }

  return new FictionBook2SkipElementContext(this);
}

void FictionBook2StanzaContext::endOfElement()
{
  getCollector()->openParagraph(FictionBook2BlockFormat());
  getCollector()->closeParagraph();
}

void FictionBook2StanzaContext::attribute(const FictionBook2TokenData &name, const FictionBook2TokenData *ns, const char *const value)
{
  if ((FictionBook2Token::NS_XML == getFictionBook2TokenID(ns)) && (FictionBook2Token::lang == getFictionBook2TokenID(name)))
    setLang(value);
}

FictionBook2TitleContext::FictionBook2TitleContext(FictionBook2ParserContext *const parentContext, const FictionBook2BlockFormat &format)
  : FictionBook2BlockFormatContextBase(parentContext, format)
{
  getBlockFormat().title = true;
}

FictionBook2XMLParserContext *FictionBook2TitleContext::element(const FictionBook2TokenData &name, const FictionBook2TokenData &ns)
{
  if (FictionBook2Token::NS_FICTIONBOOK == getFictionBook2TokenID(ns))
  {
    switch (getFictionBook2TokenID(name))
    {
    case FictionBook2Token::p :
      return new FictionBook2PContext(this, getBlockFormat());
    case FictionBook2Token::empty_line :
      return new FictionBook2EmptyLineContext(this);
    default :
      break;
    }
  }

  return new FictionBook2SkipElementContext(this);
}

void FictionBook2TitleContext::endOfElement()
{
  // TODO: implement me
}

void FictionBook2TitleContext::attribute(const FictionBook2TokenData &name, const FictionBook2TokenData *ns, const char *value)
{
  if (FictionBook2_NO_NAMESPACE(ns) && (FictionBook2Token::id == getFictionBook2TokenID(name)))
    getCollector()->defineID(value);
  else if ((FictionBook2Token::NS_XML == getFictionBook2TokenID(ns)) && (FictionBook2Token::lang == getFictionBook2TokenID(name)))
    setLang(value);
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
