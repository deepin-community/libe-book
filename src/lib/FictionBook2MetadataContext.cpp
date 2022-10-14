/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <deque>

#include <librevenge/librevenge.h>

#include "FictionBook2Collector.h"
#include "FictionBook2MetadataContext.h"
#include "FictionBook2Token.h"

namespace libebook
{

class FictionBook2Authors
{
public:
  struct Data
  {
    Data();

    librevenge::RVNGString firstName;
    librevenge::RVNGString middleName;
    librevenge::RVNGString lastName;
    librevenge::RVNGString nickname;
  };

  FictionBook2Authors();

  void setFirstName(const librevenge::RVNGString &name);
  void setMiddleName(const librevenge::RVNGString &name);
  void setLastName(const librevenge::RVNGString &name);
  void setNickname(const librevenge::RVNGString &name);
  void finishAuthor();

  const std::deque<Data> &getAuthors() const;

private:
  std::deque<Data> m_authors;
  Data m_current;
};

}

namespace libebook
{

FictionBook2Authors::Data::Data()
  : firstName()
  , middleName()
  , lastName()
  , nickname()
{
}

FictionBook2Authors::FictionBook2Authors()
  : m_authors()
  , m_current()
{
}

void FictionBook2Authors::setFirstName(const librevenge::RVNGString &name)
{
  m_current.firstName = name;
}

void FictionBook2Authors::setMiddleName(const librevenge::RVNGString &name)
{
  m_current.middleName = name;
}

void FictionBook2Authors::setLastName(const librevenge::RVNGString &name)
{
  m_current.lastName = name;
}

void FictionBook2Authors::setNickname(const librevenge::RVNGString &name)
{
  m_current.nickname = name;
}

void FictionBook2Authors::finishAuthor()
{
  m_authors.push_back(m_current);
  m_current = Data();
}

const std::deque<FictionBook2Authors::Data> &FictionBook2Authors::getAuthors() const
{
  return m_authors;
}

}

namespace libebook
{

FictionBook2AnnotationContext::FictionBook2AnnotationContext(FictionBook2ParserContext *parentContext, const FictionBook2BlockFormat &format)
  : FictionBook2BlockFormatContextBase(parentContext, format)
{
}

FictionBook2XMLParserContext *FictionBook2AnnotationContext::element(const FictionBook2TokenData &name, const FictionBook2TokenData &ns)
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

void FictionBook2AnnotationContext::startOfElement()
{
  // TODO: implement me
}

void FictionBook2AnnotationContext::endOfElement()
{
  // TODO: implement me
}

void FictionBook2AnnotationContext::attribute(const FictionBook2TokenData &name, const FictionBook2TokenData *ns, const char *value)
{
  // TODO: implement me
  (void) value;
  if (FictionBook2_NO_NAMESPACE(ns))
  {
    switch (getFictionBook2TokenID(name))
    {
    default :
      break;
    }
  }
}

void FictionBook2AnnotationContext::text(const char *)
{
}

FictionBook2AuthorContext::FictionBook2AuthorContext(FictionBook2ParserContext *parentContext, FictionBook2Authors &authors)
  : FictionBook2ParserContext(parentContext)
  , m_authors(authors)
{
}

FictionBook2XMLParserContext *FictionBook2AuthorContext::element(const FictionBook2TokenData &name, const FictionBook2TokenData &ns)
{
  if (FictionBook2Token::NS_FICTIONBOOK == getFictionBook2TokenID(ns))
  {
    switch (getFictionBook2TokenID(name))
    {
    case FictionBook2Token::first_name :
      return new FictionBook2FirstNameContext(this, m_authors);
    case FictionBook2Token::middle_name :
      return new FictionBook2MiddleNameContext(this, m_authors);
    case FictionBook2Token::last_name :
      return new FictionBook2LastNameContext(this, m_authors);
    case FictionBook2Token::nickname :
      return new FictionBook2NicknameContext(this, m_authors);
    case FictionBook2Token::email :
    case FictionBook2Token::home_page :
      // ignore
      break;
    default :
      break;
    }
  }

  return new FictionBook2SkipElementContext(this);
}

void FictionBook2AuthorContext::startOfElement()
{
}

void FictionBook2AuthorContext::endOfElement()
{
  m_authors.finishAuthor();
}

void FictionBook2AuthorContext::attribute(const FictionBook2TokenData &, const FictionBook2TokenData *, const char *)
{
}

void FictionBook2AuthorContext::text(const char *)
{
}

FictionBook2BookNameContext::FictionBook2BookNameContext(FictionBook2ParserContext *parentContext)
  : FictionBook2ParserContext(parentContext)
{
}

FictionBook2XMLParserContext *FictionBook2BookNameContext::element(const FictionBook2TokenData &name, const FictionBook2TokenData &ns)
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

void FictionBook2BookNameContext::startOfElement()
{
  // TODO: implement me
}

void FictionBook2BookNameContext::endOfElement()
{
  // TODO: implement me
}

void FictionBook2BookNameContext::attribute(const FictionBook2TokenData &name, const FictionBook2TokenData *ns, const char *value)
{
  // TODO: implement me
  (void) value;
  if (FictionBook2_NO_NAMESPACE(ns))
  {
    switch (getFictionBook2TokenID(name))
    {
    default :
      break;
    }
  }
}

void FictionBook2BookNameContext::text(const char *value)
{
  // TODO: implement me
  librevenge::RVNGString str(value);
  (void) str;
}

FictionBook2BookTitleContext::FictionBook2BookTitleContext(FictionBook2ParserContext *parentContext)
  : FictionBook2ParserContext(parentContext)
{
}

FictionBook2XMLParserContext *FictionBook2BookTitleContext::element(const FictionBook2TokenData &, const FictionBook2TokenData &)
{
  return nullptr;
}

void FictionBook2BookTitleContext::startOfElement()
{
}

void FictionBook2BookTitleContext::endOfElement()
{
}

void FictionBook2BookTitleContext::attribute(const FictionBook2TokenData &, const FictionBook2TokenData *, const char *)
{
}

void FictionBook2BookTitleContext::text(const char *value)
{
  getCollector()->defineMetadataEntry("dc:subject", value);
}

FictionBook2CityContext::FictionBook2CityContext(FictionBook2ParserContext *parentContext)
  : FictionBook2ParserContext(parentContext)
{
}

FictionBook2XMLParserContext *FictionBook2CityContext::element(const FictionBook2TokenData &name, const FictionBook2TokenData &ns)
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

void FictionBook2CityContext::startOfElement()
{
  // TODO: implement me
}

void FictionBook2CityContext::endOfElement()
{
  // TODO: implement me
}

void FictionBook2CityContext::attribute(const FictionBook2TokenData &name, const FictionBook2TokenData *ns, const char *value)
{
  // TODO: implement me
  (void) value;
  if (FictionBook2_NO_NAMESPACE(ns))
  {
    switch (getFictionBook2TokenID(name))
    {
    default :
      break;
    }
  }
}

void FictionBook2CityContext::text(const char *value)
{
  // TODO: implement me
  librevenge::RVNGString str(value);
  (void) str;
}

FictionBook2CoverpageContext::FictionBook2CoverpageContext(FictionBook2ParserContext *parentContext)
  : FictionBook2ParserContext(parentContext)
{
}

FictionBook2XMLParserContext *FictionBook2CoverpageContext::element(const FictionBook2TokenData &name, const FictionBook2TokenData &ns)
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

void FictionBook2CoverpageContext::startOfElement()
{
  // TODO: implement me
}

void FictionBook2CoverpageContext::endOfElement()
{
  // TODO: implement me
}

void FictionBook2CoverpageContext::attribute(const FictionBook2TokenData &name, const FictionBook2TokenData *ns, const char *value)
{
  // TODO: implement me
  (void) value;
  if (FictionBook2_NO_NAMESPACE(ns))
  {
    switch (getFictionBook2TokenID(name))
    {
    default :
      break;
    }
  }
}

void FictionBook2CoverpageContext::text(const char *value)
{
  // TODO: implement me
  librevenge::RVNGString str(value);
  (void) str;
}

FictionBook2CustomInfoContext::FictionBook2CustomInfoContext(FictionBook2ParserContext *parentContext)
  : FictionBook2ParserContext(parentContext)
  , m_type()
  , m_text()
{
}

FictionBook2XMLParserContext *FictionBook2CustomInfoContext::element(const FictionBook2TokenData &, const FictionBook2TokenData &)
{
  return nullptr;
}

void FictionBook2CustomInfoContext::startOfElement()
{
}

void FictionBook2CustomInfoContext::endOfElement()
{
  librevenge::RVNGString info(m_type);
  info.append(": ");
  info.append(m_text);
  // TODO: there might be more than one comment. Must properties be unique?
  getCollector()->defineMetadataEntry("librevenge:comments", info.cstr());
}

void FictionBook2CustomInfoContext::attribute(const FictionBook2TokenData &name, const FictionBook2TokenData *ns, const char *value)
{
  if (FictionBook2_NO_NAMESPACE(ns))
  {
    switch (getFictionBook2TokenID(name))
    {
    case FictionBook2Token::info_type :
      m_type = value;
    default :
      break;
    }
  }
}

void FictionBook2CustomInfoContext::text(const char *value)
{
  m_text = value;
}

FictionBook2DateContext::FictionBook2DateContext(FictionBook2ParserContext *const parentContext, const char *const name)
  : FictionBook2ParserContext(parentContext)
  , m_name(name)
{
}

FictionBook2XMLParserContext *FictionBook2DateContext::element(const FictionBook2TokenData &, const FictionBook2TokenData &)
{
  return nullptr;
}

void FictionBook2DateContext::startOfElement()
{
}

void FictionBook2DateContext::endOfElement()
{
}

void FictionBook2DateContext::attribute(const FictionBook2TokenData &name, const FictionBook2TokenData *ns, const char *value)
{
  // TODO: should we use the computer-readable date from attribute, if
  // present?
  (void) value;
  if (FictionBook2_NO_NAMESPACE(ns))
  {
    switch (getFictionBook2TokenID(name))
    {
    case FictionBook2Token::value :
      break;
    default :
      break;
    }
  }
}

void FictionBook2DateContext::text(const char *value)
{
  getCollector()->defineMetadataEntry(m_name, value);
}

FictionBook2DescriptionContext::FictionBook2DescriptionContext(FictionBook2ParserContext *const parentContext, FictionBook2Collector *const collector)
  : FictionBook2ParserContext(parentContext, collector)
{
}

FictionBook2XMLParserContext *FictionBook2DescriptionContext::element(const FictionBook2TokenData &name, const FictionBook2TokenData &ns)
{
  if (FictionBook2Token::NS_FICTIONBOOK == getFictionBook2TokenID(ns))
  {
    switch (getFictionBook2TokenID(name))
    {
    case FictionBook2Token::title_info :
      return new FictionBook2TitleInfoContext(this);
    case FictionBook2Token::src_title_info :
      // not interesting -> skip
      break;
    case FictionBook2Token::document_info :
      return new FictionBook2TextInfoContext(this);
    case FictionBook2Token::publish_info :
      return new FictionBook2PublishInfoContext(this);
    case FictionBook2Token::custom_info :
      return new FictionBook2CustomInfoContext(this);
    case FictionBook2Token::output :
      return new FictionBook2OutputContext(this);
    default :
      break;
    }
  }

  return new FictionBook2SkipElementContext(this);
}

void FictionBook2DescriptionContext::startOfElement()
{
}

void FictionBook2DescriptionContext::endOfElement()
{
}

void FictionBook2DescriptionContext::attribute(const FictionBook2TokenData &, const FictionBook2TokenData *, const char *)
{
}

void FictionBook2DescriptionContext::text(const char *)
{
}

FictionBook2TextInfoContext::FictionBook2TextInfoContext(FictionBook2ParserContext *parentContext)
  : FictionBook2ParserContext(parentContext)
  , m_authors(new FictionBook2Authors())
{
}

FictionBook2TextInfoContext::~FictionBook2TextInfoContext()
{
}

FictionBook2XMLParserContext *FictionBook2TextInfoContext::element(const FictionBook2TokenData &name, const FictionBook2TokenData &ns)
{
  if (FictionBook2Token::NS_FICTIONBOOK == getFictionBook2TokenID(ns))
  {
    switch (getFictionBook2TokenID(name))
    {
    case FictionBook2Token::author :
      return new FictionBook2AuthorContext(this, *m_authors);
    case FictionBook2Token::date :
      return new FictionBook2DateContext(this, "dcterms:issued");
    case FictionBook2Token::src_url :
      return new FictionBook2SrcUrlContext(this);
    case FictionBook2Token::version :
      return new FictionBook2VersionContext(this);
    case FictionBook2Token::history :
      return new FictionBook2HistoryContext(this);
    case FictionBook2Token::id :
    case FictionBook2Token::program_used :
    // ignore
    default :
      break;
    }
  }

  return new FictionBook2SkipElementContext(this);
}

void FictionBook2TextInfoContext::startOfElement()
{
}

void FictionBook2TextInfoContext::endOfElement()
{
  // TODO: handle authors
}

void FictionBook2TextInfoContext::attribute(const FictionBook2TokenData &, const FictionBook2TokenData *, const char *)
{
}

void FictionBook2TextInfoContext::text(const char *)
{
}

FictionBook2EmailContext::FictionBook2EmailContext(FictionBook2ParserContext *parentContext)
  : FictionBook2ParserContext(parentContext)
{
}

FictionBook2XMLParserContext *FictionBook2EmailContext::element(const FictionBook2TokenData &name, const FictionBook2TokenData &ns)
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

void FictionBook2EmailContext::startOfElement()
{
  // TODO: implement me
}

void FictionBook2EmailContext::endOfElement()
{
  // TODO: implement me
}

void FictionBook2EmailContext::attribute(const FictionBook2TokenData &name, const FictionBook2TokenData *ns, const char *value)
{
  // TODO: implement me
  (void) value;
  if (FictionBook2_NO_NAMESPACE(ns))
  {
    switch (getFictionBook2TokenID(name))
    {
    default :
      break;
    }
  }
}

void FictionBook2EmailContext::text(const char *value)
{
  // TODO: implement me
  librevenge::RVNGString str(value);
  (void) str;
}

FictionBook2FirstNameContext::FictionBook2FirstNameContext(FictionBook2ParserContext *parentContext, FictionBook2Authors &authors)
  : FictionBook2ParserContext(parentContext)
  , m_authors(authors)
{
}

FictionBook2XMLParserContext *FictionBook2FirstNameContext::element(const FictionBook2TokenData &, const FictionBook2TokenData &)
{
  return nullptr;
}

void FictionBook2FirstNameContext::startOfElement()
{
}

void FictionBook2FirstNameContext::endOfElement()
{
}

void FictionBook2FirstNameContext::attribute(const FictionBook2TokenData &, const FictionBook2TokenData *, const char *)
{
}

void FictionBook2FirstNameContext::text(const char *value)
{
  const librevenge::RVNGString str(value);
  m_authors.setFirstName(str);
}

FictionBook2GenreContext::FictionBook2GenreContext(FictionBook2ParserContext *parentContext)
  : FictionBook2ParserContext(parentContext)
{
}

FictionBook2XMLParserContext *FictionBook2GenreContext::element(const FictionBook2TokenData &name, const FictionBook2TokenData &ns)
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

void FictionBook2GenreContext::startOfElement()
{
  // TODO: implement me
}

void FictionBook2GenreContext::endOfElement()
{
  // TODO: implement me
}

void FictionBook2GenreContext::attribute(const FictionBook2TokenData &name, const FictionBook2TokenData *ns, const char *value)
{
  // TODO: implement me
  (void) value;
  if (FictionBook2_NO_NAMESPACE(ns))
  {
    switch (getFictionBook2TokenID(name))
    {
    default :
      break;
    }
  }
}

void FictionBook2GenreContext::text(const char *value)
{
  // TODO: implement me
  librevenge::RVNGString str(value);
  (void) str;
}

FictionBook2HistoryContext::FictionBook2HistoryContext(FictionBook2ParserContext *parentContext)
  : FictionBook2ParserContext(parentContext)
{
}

FictionBook2XMLParserContext *FictionBook2HistoryContext::element(const FictionBook2TokenData &name, const FictionBook2TokenData &ns)
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

void FictionBook2HistoryContext::startOfElement()
{
  // TODO: implement me
}

void FictionBook2HistoryContext::endOfElement()
{
  // TODO: implement me
}

void FictionBook2HistoryContext::attribute(const FictionBook2TokenData &name, const FictionBook2TokenData *ns, const char *value)
{
  // TODO: implement me
  (void) value;
  if (FictionBook2_NO_NAMESPACE(ns))
  {
    switch (getFictionBook2TokenID(name))
    {
    default :
      break;
    }
  }
}

void FictionBook2HistoryContext::text(const char *value)
{
  // TODO: implement me
  librevenge::RVNGString str(value);
  (void) str;
}

FictionBook2HomePageContext::FictionBook2HomePageContext(FictionBook2ParserContext *parentContext)
  : FictionBook2ParserContext(parentContext)
{
}

FictionBook2XMLParserContext *FictionBook2HomePageContext::element(const FictionBook2TokenData &name, const FictionBook2TokenData &ns)
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

void FictionBook2HomePageContext::startOfElement()
{
  // TODO: implement me
}

void FictionBook2HomePageContext::endOfElement()
{
  // TODO: implement me
}

void FictionBook2HomePageContext::attribute(const FictionBook2TokenData &name, const FictionBook2TokenData *ns, const char *value)
{
  // TODO: implement me
  (void) value;
  if (FictionBook2_NO_NAMESPACE(ns))
  {
    switch (getFictionBook2TokenID(name))
    {
    default :
      break;
    }
  }
}

void FictionBook2HomePageContext::text(const char *value)
{
  // TODO: implement me
  librevenge::RVNGString str(value);
  (void) str;
}

FictionBook2IdContext::FictionBook2IdContext(FictionBook2ParserContext *parentContext)
  : FictionBook2ParserContext(parentContext)
{
}

FictionBook2XMLParserContext *FictionBook2IdContext::element(const FictionBook2TokenData &name, const FictionBook2TokenData &ns)
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

void FictionBook2IdContext::startOfElement()
{
  // TODO: implement me
}

void FictionBook2IdContext::endOfElement()
{
  // TODO: implement me
}

void FictionBook2IdContext::attribute(const FictionBook2TokenData &name, const FictionBook2TokenData *ns, const char *value)
{
  // TODO: implement me
  (void) value;
  if (FictionBook2_NO_NAMESPACE(ns))
  {
    switch (getFictionBook2TokenID(name))
    {
    default :
      break;
    }
  }
}

void FictionBook2IdContext::text(const char *value)
{
  // TODO: implement me
  librevenge::RVNGString str(value);
  (void) str;
}

FictionBook2IsbnContext::FictionBook2IsbnContext(FictionBook2ParserContext *parentContext)
  : FictionBook2ParserContext(parentContext)
{
}

FictionBook2XMLParserContext *FictionBook2IsbnContext::element(const FictionBook2TokenData &name, const FictionBook2TokenData &ns)
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

void FictionBook2IsbnContext::startOfElement()
{
  // TODO: implement me
}

void FictionBook2IsbnContext::endOfElement()
{
  // TODO: implement me
}

void FictionBook2IsbnContext::attribute(const FictionBook2TokenData &name, const FictionBook2TokenData *ns, const char *value)
{
  // TODO: implement me
  (void) value;
  if (FictionBook2_NO_NAMESPACE(ns))
  {
    switch (getFictionBook2TokenID(name))
    {
    default :
      break;
    }
  }
}

void FictionBook2IsbnContext::text(const char *value)
{
  // TODO: implement me
  librevenge::RVNGString str(value);
  (void) str;
}

FictionBook2KeywordsContext::FictionBook2KeywordsContext(FictionBook2ParserContext *parentContext)
  : FictionBook2ParserContext(parentContext)
{
}

FictionBook2XMLParserContext *FictionBook2KeywordsContext::element(const FictionBook2TokenData &, const FictionBook2TokenData &)
{
  return nullptr;
}

void FictionBook2KeywordsContext::startOfElement()
{
}

void FictionBook2KeywordsContext::endOfElement()
{
}

void FictionBook2KeywordsContext::attribute(const FictionBook2TokenData &, const FictionBook2TokenData *, const char *)
{
}

void FictionBook2KeywordsContext::text(const char *value)
{
  getCollector()->defineMetadataEntry("meta:keyword", value);
}

FictionBook2LangContext::FictionBook2LangContext(FictionBook2ParserContext *parentContext)
  : FictionBook2ParserContext(parentContext)
{
}

FictionBook2XMLParserContext *FictionBook2LangContext::element(const FictionBook2TokenData &, const FictionBook2TokenData &)
{
  return nullptr;
}

void FictionBook2LangContext::startOfElement()
{
}

void FictionBook2LangContext::endOfElement()
{
}

void FictionBook2LangContext::attribute(const FictionBook2TokenData &, const FictionBook2TokenData *, const char *)
{
}

void FictionBook2LangContext::text(const char *value)
{
  getCollector()->defineMetadataEntry("dc:language", value);
}

FictionBook2LastNameContext::FictionBook2LastNameContext(FictionBook2ParserContext *parentContext, FictionBook2Authors &authors)
  : FictionBook2ParserContext(parentContext)
  , m_authors(authors)
{
}

FictionBook2XMLParserContext *FictionBook2LastNameContext::element(const FictionBook2TokenData &, const FictionBook2TokenData &)
{
  return nullptr;
}

void FictionBook2LastNameContext::startOfElement()
{
}

void FictionBook2LastNameContext::endOfElement()
{
}

void FictionBook2LastNameContext::attribute(const FictionBook2TokenData &, const FictionBook2TokenData *, const char *)
{
}

void FictionBook2LastNameContext::text(const char *value)
{
  const librevenge::RVNGString str(value);
  m_authors.setLastName(str);
}

FictionBook2MiddleNameContext::FictionBook2MiddleNameContext(FictionBook2ParserContext *parentContext, FictionBook2Authors &authors)
  : FictionBook2ParserContext(parentContext)
  , m_authors(authors)
{
}

FictionBook2XMLParserContext *FictionBook2MiddleNameContext::element(const FictionBook2TokenData &, const FictionBook2TokenData &)
{
  return nullptr;
}

void FictionBook2MiddleNameContext::startOfElement()
{
}

void FictionBook2MiddleNameContext::endOfElement()
{
}

void FictionBook2MiddleNameContext::attribute(const FictionBook2TokenData &, const FictionBook2TokenData *, const char *)
{
}

void FictionBook2MiddleNameContext::text(const char *value)
{
  const librevenge::RVNGString str(value);
  m_authors.setMiddleName(str);
}

FictionBook2NicknameContext::FictionBook2NicknameContext(FictionBook2ParserContext *parentContext, FictionBook2Authors &authors)
  : FictionBook2ParserContext(parentContext)
  , m_authors(authors)
{
}

FictionBook2XMLParserContext *FictionBook2NicknameContext::element(const FictionBook2TokenData &, const FictionBook2TokenData &)
{
  return nullptr;
}

void FictionBook2NicknameContext::startOfElement()
{
}

void FictionBook2NicknameContext::endOfElement()
{
}

void FictionBook2NicknameContext::attribute(const FictionBook2TokenData &, const FictionBook2TokenData *, const char *)
{
}

void FictionBook2NicknameContext::text(const char *value)
{
  const librevenge::RVNGString str(value);
  m_authors.setNickname(str);
}

FictionBook2OutputContext::FictionBook2OutputContext(FictionBook2ParserContext *parentContext)
  : FictionBook2ParserContext(parentContext)
{
}

FictionBook2XMLParserContext *FictionBook2OutputContext::element(const FictionBook2TokenData &name, const FictionBook2TokenData &ns)
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

void FictionBook2OutputContext::startOfElement()
{
  // TODO: implement me
}

void FictionBook2OutputContext::endOfElement()
{
  // TODO: implement me
}

void FictionBook2OutputContext::attribute(const FictionBook2TokenData &name, const FictionBook2TokenData *ns, const char *value)
{
  // TODO: implement me
  (void) value;
  if (FictionBook2_NO_NAMESPACE(ns))
  {
    switch (getFictionBook2TokenID(name))
    {
    default :
      break;
    }
  }
}

void FictionBook2OutputContext::text(const char *value)
{
  // TODO: implement me
  librevenge::RVNGString str(value);
  (void) str;
}

FictionBook2OutputDocumentClassContext::FictionBook2OutputDocumentClassContext(FictionBook2ParserContext *parentContext)
  : FictionBook2ParserContext(parentContext)
{
}

FictionBook2XMLParserContext *FictionBook2OutputDocumentClassContext::element(const FictionBook2TokenData &name, const FictionBook2TokenData &ns)
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

void FictionBook2OutputDocumentClassContext::startOfElement()
{
  // TODO: implement me
}

void FictionBook2OutputDocumentClassContext::endOfElement()
{
  // TODO: implement me
}

void FictionBook2OutputDocumentClassContext::attribute(const FictionBook2TokenData &name, const FictionBook2TokenData *ns, const char *value)
{
  // TODO: implement me
  (void) value;
  if (FictionBook2_NO_NAMESPACE(ns))
  {
    switch (getFictionBook2TokenID(name))
    {
    default :
      break;
    }
  }
}

void FictionBook2OutputDocumentClassContext::text(const char *value)
{
  // TODO: implement me
  librevenge::RVNGString str(value);
  (void) str;
}

FictionBook2PartContext::FictionBook2PartContext(FictionBook2ParserContext *parentContext)
  : FictionBook2ParserContext(parentContext)
{
}

FictionBook2XMLParserContext *FictionBook2PartContext::element(const FictionBook2TokenData &name, const FictionBook2TokenData &ns)
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

void FictionBook2PartContext::startOfElement()
{
  // TODO: implement me
}

void FictionBook2PartContext::endOfElement()
{
  // TODO: implement me
}

void FictionBook2PartContext::attribute(const FictionBook2TokenData &name, const FictionBook2TokenData *ns, const char *value)
{
  // TODO: implement me
  (void) value;
  if (FictionBook2_NO_NAMESPACE(ns))
  {
    switch (getFictionBook2TokenID(name))
    {
    default :
      break;
    }
  }
}

void FictionBook2PartContext::text(const char *value)
{
  // TODO: implement me
  librevenge::RVNGString str(value);
  (void) str;
}

FictionBook2ProgramUsedContext::FictionBook2ProgramUsedContext(FictionBook2ParserContext *parentContext)
  : FictionBook2ParserContext(parentContext)
{
}

FictionBook2XMLParserContext *FictionBook2ProgramUsedContext::element(const FictionBook2TokenData &name, const FictionBook2TokenData &ns)
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

void FictionBook2ProgramUsedContext::startOfElement()
{
  // TODO: implement me
}

void FictionBook2ProgramUsedContext::endOfElement()
{
  // TODO: implement me
}

void FictionBook2ProgramUsedContext::attribute(const FictionBook2TokenData &name, const FictionBook2TokenData *ns, const char *value)
{
  // TODO: implement me
  (void) value;
  if (FictionBook2_NO_NAMESPACE(ns))
  {
    switch (getFictionBook2TokenID(name))
    {
    default :
      break;
    }
  }
}

void FictionBook2ProgramUsedContext::text(const char *value)
{
  // TODO: implement me
  librevenge::RVNGString str(value);
  (void) str;
}

FictionBook2PublisherContext::FictionBook2PublisherContext(FictionBook2ParserContext *parentContext)
  : FictionBook2ParserContext(parentContext)
{
}

FictionBook2XMLParserContext *FictionBook2PublisherContext::element(const FictionBook2TokenData &, const FictionBook2TokenData &)
{
  return nullptr;
}

void FictionBook2PublisherContext::startOfElement()
{
}

void FictionBook2PublisherContext::endOfElement()
{
}

void FictionBook2PublisherContext::attribute(const FictionBook2TokenData &, const FictionBook2TokenData *, const char *)
{
}

void FictionBook2PublisherContext::text(const char *value)
{
  getCollector()->defineMetadataEntry("dc:publisher", value);
}

FictionBook2PublishInfoContext::FictionBook2PublishInfoContext(FictionBook2ParserContext *parentContext)
  : FictionBook2ParserContext(parentContext)
{
}

FictionBook2XMLParserContext *FictionBook2PublishInfoContext::element(const FictionBook2TokenData &name, const FictionBook2TokenData &ns)
{
  if (FictionBook2Token::NS_FICTIONBOOK == getFictionBook2TokenID(ns))
  {
    switch (getFictionBook2TokenID(name))
    {
    case FictionBook2Token::publisher :
      return new FictionBook2PublisherContext(this);
    case FictionBook2Token::book_name :
    case FictionBook2Token::city :
    case FictionBook2Token::isbn :
    case FictionBook2Token::sequence :
    case FictionBook2Token::year :
      // ignore
      break;
    default :
      break;
    }
  }

  return new FictionBook2SkipElementContext(this);
}

void FictionBook2PublishInfoContext::startOfElement()
{
}

void FictionBook2PublishInfoContext::endOfElement()
{
}

void FictionBook2PublishInfoContext::attribute(const FictionBook2TokenData &, const FictionBook2TokenData *, const char *)
{
}

void FictionBook2PublishInfoContext::text(const char *)
{
}

FictionBook2SequenceContext::FictionBook2SequenceContext(FictionBook2ParserContext *parentContext)
  : FictionBook2ParserContext(parentContext)
{
}

FictionBook2XMLParserContext *FictionBook2SequenceContext::element(const FictionBook2TokenData &name, const FictionBook2TokenData &ns)
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

void FictionBook2SequenceContext::startOfElement()
{
  // TODO: implement me
}

void FictionBook2SequenceContext::endOfElement()
{
  // TODO: implement me
}

void FictionBook2SequenceContext::attribute(const FictionBook2TokenData &name, const FictionBook2TokenData *ns, const char *value)
{
  // TODO: implement me
  (void) value;
  if (FictionBook2_NO_NAMESPACE(ns))
  {
    switch (getFictionBook2TokenID(name))
    {
    default :
      break;
    }
  }
}

void FictionBook2SequenceContext::text(const char *value)
{
  // TODO: implement me
  librevenge::RVNGString str(value);
  (void) str;
}

FictionBook2SrcLangContext::FictionBook2SrcLangContext(FictionBook2ParserContext *parentContext)
  : FictionBook2ParserContext(parentContext)
{
}

FictionBook2XMLParserContext *FictionBook2SrcLangContext::element(const FictionBook2TokenData &name, const FictionBook2TokenData &ns)
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

void FictionBook2SrcLangContext::startOfElement()
{
  // TODO: implement me
}

void FictionBook2SrcLangContext::endOfElement()
{
  // TODO: implement me
}

void FictionBook2SrcLangContext::attribute(const FictionBook2TokenData &name, const FictionBook2TokenData *ns, const char *value)
{
  // TODO: implement me
  (void) value;
  if (FictionBook2_NO_NAMESPACE(ns))
  {
    switch (getFictionBook2TokenID(name))
    {
    default :
      break;
    }
  }
}

void FictionBook2SrcLangContext::text(const char *value)
{
  // TODO: implement me
  librevenge::RVNGString str(value);
  (void) str;
}

FictionBook2SrcOcrContext::FictionBook2SrcOcrContext(FictionBook2ParserContext *parentContext)
  : FictionBook2ParserContext(parentContext)
{
}

FictionBook2XMLParserContext *FictionBook2SrcOcrContext::element(const FictionBook2TokenData &name, const FictionBook2TokenData &ns)
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

void FictionBook2SrcOcrContext::startOfElement()
{
  // TODO: implement me
}

void FictionBook2SrcOcrContext::endOfElement()
{
  // TODO: implement me
}

void FictionBook2SrcOcrContext::attribute(const FictionBook2TokenData &name, const FictionBook2TokenData *ns, const char *value)
{
  // TODO: implement me
  (void) value;
  if (FictionBook2_NO_NAMESPACE(ns))
  {
    switch (getFictionBook2TokenID(name))
    {
    default :
      break;
    }
  }
}

void FictionBook2SrcOcrContext::text(const char *value)
{
  // TODO: implement me
  librevenge::RVNGString str(value);
  (void) str;
}

FictionBook2SrcTitleInfoContext::FictionBook2SrcTitleInfoContext(FictionBook2ParserContext *parentContext)
  : FictionBook2ParserContext(parentContext)
{
}

FictionBook2XMLParserContext *FictionBook2SrcTitleInfoContext::element(const FictionBook2TokenData &name, const FictionBook2TokenData &ns)
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

void FictionBook2SrcTitleInfoContext::startOfElement()
{
  // TODO: implement me
}

void FictionBook2SrcTitleInfoContext::endOfElement()
{
  // TODO: implement me
}

void FictionBook2SrcTitleInfoContext::attribute(const FictionBook2TokenData &name, const FictionBook2TokenData *ns, const char *value)
{
  // TODO: implement me
  (void) value;
  if (FictionBook2_NO_NAMESPACE(ns))
  {
    switch (getFictionBook2TokenID(name))
    {
    default :
      break;
    }
  }
}

void FictionBook2SrcTitleInfoContext::text(const char *value)
{
  // TODO: implement me
  librevenge::RVNGString str(value);
  (void) str;
}

FictionBook2SrcUrlContext::FictionBook2SrcUrlContext(FictionBook2ParserContext *parentContext)
  : FictionBook2ParserContext(parentContext)
{
}

FictionBook2XMLParserContext *FictionBook2SrcUrlContext::element(const FictionBook2TokenData &, const FictionBook2TokenData &)
{
  return nullptr;
}

void FictionBook2SrcUrlContext::startOfElement()
{
}

void FictionBook2SrcUrlContext::endOfElement()
{
}

void FictionBook2SrcUrlContext::attribute(const FictionBook2TokenData &, const FictionBook2TokenData *, const char *)
{
}

void FictionBook2SrcUrlContext::text(const char *value)
{
  getCollector()->defineMetadataEntry("dc:source", value);
}

FictionBook2TitleInfoContext::FictionBook2TitleInfoContext(FictionBook2ParserContext *parentContext)
  : FictionBook2ParserContext(parentContext)
  , m_authors(new FictionBook2Authors())
{
}

FictionBook2TitleInfoContext::~FictionBook2TitleInfoContext()
{
}

FictionBook2XMLParserContext *FictionBook2TitleInfoContext::element(const FictionBook2TokenData &name, const FictionBook2TokenData &ns)
{
  // TODO: implement me
  if (FictionBook2Token::NS_FICTIONBOOK == getFictionBook2TokenID(ns))
  {
    switch (getFictionBook2TokenID(name))
    {
    case FictionBook2Token::author :
      return new FictionBook2AuthorContext(this, *m_authors);
    case FictionBook2Token::book_title :
      return new FictionBook2BookTitleContext(this);
    case FictionBook2Token::keywords :
      return new FictionBook2KeywordsContext(this);
    case FictionBook2Token::date :
      return new FictionBook2DateContext(this, "meta:creation-date");
    case FictionBook2Token::coverpage :
      return new FictionBook2CoverpageContext(this);
    case FictionBook2Token::lang :
      return new FictionBook2LangContext(this);
    case FictionBook2Token::annotation :
    case FictionBook2Token::genre :
    case FictionBook2Token::sequence :
    case FictionBook2Token::src_lang :
    case FictionBook2Token::translator :
      // ignore
      break;
    default :
      break;
    }
  }

  return new FictionBook2SkipElementContext(this);
}

void FictionBook2TitleInfoContext::startOfElement()
{
}

void FictionBook2TitleInfoContext::endOfElement()
{
  typedef std::deque<FictionBook2Authors::Data> Authors_t;
  const Authors_t authors = m_authors->getAuthors();
  librevenge::RVNGString authorsStr;
  for (const auto &it : authors)
  {
    librevenge::RVNGString author;
    if (it.firstName.len() != 0)
    {
      author = it.firstName;
      if (it.middleName.len() != 0)
      {
        author.append(' ');
        author.append(it.middleName);
      }
      author.append(' ');
      author.append(it.lastName);
    }
    else
    {
      author = it.nickname;
    }

    // TODO: it would be better to push authors separately, if it's
    // possible
    if (author.len() != 0)
    {
      if (authorsStr.len() != 0)
        authorsStr.append(", ");
      authorsStr.append(author);
    }
  }

  if (authorsStr.len() != 0)
    getCollector()->defineMetadataEntry("meta:initial-creator", authorsStr.cstr());
}

void FictionBook2TitleInfoContext::attribute(const FictionBook2TokenData &, const FictionBook2TokenData *, const char *)
{
}

void FictionBook2TitleInfoContext::text(const char *)
{
}

FictionBook2TranslatorContext::FictionBook2TranslatorContext(FictionBook2ParserContext *parentContext)
  : FictionBook2ParserContext(parentContext)
{
}

FictionBook2XMLParserContext *FictionBook2TranslatorContext::element(const FictionBook2TokenData &name, const FictionBook2TokenData &ns)
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

void FictionBook2TranslatorContext::startOfElement()
{
  // TODO: implement me
}

void FictionBook2TranslatorContext::endOfElement()
{
  // TODO: implement me
}

void FictionBook2TranslatorContext::attribute(const FictionBook2TokenData &name, const FictionBook2TokenData *ns, const char *value)
{
  // TODO: implement me
  (void) value;
  if (FictionBook2_NO_NAMESPACE(ns))
  {
    switch (getFictionBook2TokenID(name))
    {
    default :
      break;
    }
  }
}

void FictionBook2TranslatorContext::text(const char *value)
{
  // TODO: implement me
  librevenge::RVNGString str(value);
  (void) str;
}

FictionBook2VersionContext::FictionBook2VersionContext(FictionBook2ParserContext *parentContext)
  : FictionBook2ParserContext(parentContext)
{
}

FictionBook2XMLParserContext *FictionBook2VersionContext::element(const FictionBook2TokenData &, const FictionBook2TokenData &)
{
  return nullptr;
}

void FictionBook2VersionContext::startOfElement()
{
}

void FictionBook2VersionContext::endOfElement()
{
}

void FictionBook2VersionContext::attribute(const FictionBook2TokenData &, const FictionBook2TokenData *, const char *)
{
}

void FictionBook2VersionContext::text(const char *value)
{
  getCollector()->defineMetadataEntry("librevenge:version-number", value);
}

FictionBook2YearContext::FictionBook2YearContext(FictionBook2ParserContext *parentContext)
  : FictionBook2ParserContext(parentContext)
{
}

FictionBook2XMLParserContext *FictionBook2YearContext::element(const FictionBook2TokenData &name, const FictionBook2TokenData &ns)
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

void FictionBook2YearContext::startOfElement()
{
  // TODO: implement me
}

void FictionBook2YearContext::endOfElement()
{
  // TODO: implement me
}

void FictionBook2YearContext::attribute(const FictionBook2TokenData &name, const FictionBook2TokenData *ns, const char *value)
{
  // TODO: implement me
  (void) value;
  if (FictionBook2_NO_NAMESPACE(ns))
  {
    switch (getFictionBook2TokenID(name))
    {
    default :
      break;
    }
  }
}

void FictionBook2YearContext::text(const char *value)
{
  // TODO: implement me
  librevenge::RVNGString str(value);
  (void) str;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
