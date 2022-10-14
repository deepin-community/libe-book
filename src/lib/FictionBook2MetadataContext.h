/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef FICTIONBOOK2METADATACONTEXT_H_INCLUDED
#define FICTIONBOOK2METADATACONTEXT_H_INCLUDED

#include <memory>

#include "FictionBook2ParserContext.h"

namespace libebook
{

class FictionBook2Authors;

class FictionBook2AnnotationContext : public FictionBook2BlockFormatContextBase
{
public:
  FictionBook2AnnotationContext(FictionBook2ParserContext *parentContext, const FictionBook2BlockFormat &format);

private:
  FictionBook2XMLParserContext *element(const FictionBook2TokenData &name, const FictionBook2TokenData &ns) override;
  void startOfElement() override;
  void endOfElement() override;
  void attribute(const FictionBook2TokenData &name, const FictionBook2TokenData *ns, const char *value) override;
  void text(const char *value) override;
};

class FictionBook2AuthorContext : public FictionBook2ParserContext
{
public:
  FictionBook2AuthorContext(FictionBook2ParserContext *parentContext, FictionBook2Authors &authors);

private:
  FictionBook2XMLParserContext *element(const FictionBook2TokenData &name, const FictionBook2TokenData &ns) override;
  void startOfElement() override;
  void endOfElement() override;
  void endOfAttributes() override {}
  void attribute(const FictionBook2TokenData &name, const FictionBook2TokenData *ns, const char *value) override;
  void text(const char *value) override;

private:
  FictionBook2Authors &m_authors;
};

class FictionBook2BookNameContext : public FictionBook2ParserContext
{
public:
  explicit FictionBook2BookNameContext(FictionBook2ParserContext *parentContext);

private:
  FictionBook2XMLParserContext *element(const FictionBook2TokenData &name, const FictionBook2TokenData &ns) override;
  void startOfElement() override;
  void endOfElement() override;
  void endOfAttributes() override {}
  void attribute(const FictionBook2TokenData &name, const FictionBook2TokenData *ns, const char *value) override;
  void text(const char *value) override;
};

class FictionBook2BookTitleContext : public FictionBook2ParserContext
{
public:
  explicit FictionBook2BookTitleContext(FictionBook2ParserContext *parentContext);

private:
  FictionBook2XMLParserContext *element(const FictionBook2TokenData &name, const FictionBook2TokenData &ns) override;
  void startOfElement() override;
  void endOfElement() override;
  void endOfAttributes() override {}
  void attribute(const FictionBook2TokenData &name, const FictionBook2TokenData *ns, const char *value) override;
  void text(const char *value) override;
};

class FictionBook2CityContext : public FictionBook2ParserContext
{
public:
  explicit FictionBook2CityContext(FictionBook2ParserContext *parentContext);

private:
  FictionBook2XMLParserContext *element(const FictionBook2TokenData &name, const FictionBook2TokenData &ns) override;
  void startOfElement() override;
  void endOfElement() override;
  void endOfAttributes() override {}
  void attribute(const FictionBook2TokenData &name, const FictionBook2TokenData *ns, const char *value) override;
  void text(const char *value) override;
};

class FictionBook2CoverpageContext : public FictionBook2ParserContext
{
public:
  explicit FictionBook2CoverpageContext(FictionBook2ParserContext *parentContext);

private:
  FictionBook2XMLParserContext *element(const FictionBook2TokenData &name, const FictionBook2TokenData &ns) override;
  void startOfElement() override;
  void endOfElement() override;
  void endOfAttributes() override {}
  void attribute(const FictionBook2TokenData &name, const FictionBook2TokenData *ns, const char *value) override;
  void text(const char *value) override;
};

class FictionBook2CustomInfoContext : public FictionBook2ParserContext
{
public:
  explicit FictionBook2CustomInfoContext(FictionBook2ParserContext *parentContext);

private:
  FictionBook2XMLParserContext *element(const FictionBook2TokenData &name, const FictionBook2TokenData &ns) override;
  void startOfElement() override;
  void endOfElement() override;
  void endOfAttributes() override {}
  void attribute(const FictionBook2TokenData &name, const FictionBook2TokenData *ns, const char *value) override;
  void text(const char *value) override;

private:
  librevenge::RVNGString m_type;
  librevenge::RVNGString m_text;
};

class FictionBook2DateContext : public FictionBook2ParserContext
{
  // no copying
  FictionBook2DateContext(const FictionBook2DateContext &);
  FictionBook2DateContext &operator=(const FictionBook2DateContext &);

public:
  FictionBook2DateContext(FictionBook2ParserContext *parentContext, const char *name);

private:
  FictionBook2XMLParserContext *element(const FictionBook2TokenData &name, const FictionBook2TokenData &ns) override;
  void startOfElement() override;
  void endOfElement() override;
  void endOfAttributes() override {}
  void attribute(const FictionBook2TokenData &name, const FictionBook2TokenData *ns, const char *value) override;
  void text(const char *value) override;

private:
  const char *const m_name;
};

class FictionBook2DescriptionContext : public FictionBook2ParserContext
{
public:
  FictionBook2DescriptionContext(FictionBook2ParserContext *parentContext, FictionBook2Collector *collector);

private:
  FictionBook2XMLParserContext *element(const FictionBook2TokenData &name, const FictionBook2TokenData &ns) override;
  void startOfElement() override;
  void endOfElement() override;
  void endOfAttributes() override {}
  void attribute(const FictionBook2TokenData &name, const FictionBook2TokenData *ns, const char *value) override;
  void text(const char *value) override;
};

class FictionBook2TextInfoContext : public FictionBook2ParserContext
{
  // no copying
  FictionBook2TextInfoContext(const FictionBook2TextInfoContext &other);
  FictionBook2TextInfoContext &operator=(const FictionBook2TextInfoContext &other);

public:
  FictionBook2TextInfoContext(FictionBook2ParserContext *parentContext);

private:
  ~FictionBook2TextInfoContext() override;

  FictionBook2XMLParserContext *element(const FictionBook2TokenData &name, const FictionBook2TokenData &ns) override;
  void startOfElement() override;
  void endOfElement() override;
  void endOfAttributes() override {}
  void attribute(const FictionBook2TokenData &name, const FictionBook2TokenData *ns, const char *value) override;
  void text(const char *value) override;

private:
  std::unique_ptr<FictionBook2Authors> m_authors;
};

class FictionBook2EmailContext : public FictionBook2ParserContext
{
public:
  explicit FictionBook2EmailContext(FictionBook2ParserContext *parentContext);

private:
  FictionBook2XMLParserContext *element(const FictionBook2TokenData &name, const FictionBook2TokenData &ns) override;
  void startOfElement() override;
  void endOfElement() override;
  void endOfAttributes() override {}
  void attribute(const FictionBook2TokenData &name, const FictionBook2TokenData *ns, const char *value) override;
  void text(const char *value) override;
};

class FictionBook2FirstNameContext : public FictionBook2ParserContext
{
public:
  FictionBook2FirstNameContext(FictionBook2ParserContext *parentContext, FictionBook2Authors &authors);

private:
  FictionBook2XMLParserContext *element(const FictionBook2TokenData &name, const FictionBook2TokenData &ns) override;
  void startOfElement() override;
  void endOfElement() override;
  void endOfAttributes() override {}
  void attribute(const FictionBook2TokenData &name, const FictionBook2TokenData *ns, const char *value) override;
  void text(const char *value) override;

private:
  FictionBook2Authors &m_authors;
};

class FictionBook2GenreContext : public FictionBook2ParserContext
{
public:
  explicit FictionBook2GenreContext(FictionBook2ParserContext *parentContext);

private:
  FictionBook2XMLParserContext *element(const FictionBook2TokenData &name, const FictionBook2TokenData &ns) override;
  void startOfElement() override;
  void endOfElement() override;
  void endOfAttributes() override {}
  void attribute(const FictionBook2TokenData &name, const FictionBook2TokenData *ns, const char *value) override;
  void text(const char *value) override;
};

class FictionBook2HistoryContext : public FictionBook2ParserContext
{
public:
  explicit FictionBook2HistoryContext(FictionBook2ParserContext *parentContext);

private:
  FictionBook2XMLParserContext *element(const FictionBook2TokenData &name, const FictionBook2TokenData &ns) override;
  void startOfElement() override;
  void endOfElement() override;
  void endOfAttributes() override {}
  void attribute(const FictionBook2TokenData &name, const FictionBook2TokenData *ns, const char *value) override;
  void text(const char *value) override;
};

class FictionBook2HomePageContext : public FictionBook2ParserContext
{
public:
  explicit FictionBook2HomePageContext(FictionBook2ParserContext *parentContext);

private:
  FictionBook2XMLParserContext *element(const FictionBook2TokenData &name, const FictionBook2TokenData &ns) override;
  void startOfElement() override;
  void endOfElement() override;
  void endOfAttributes() override {}
  void attribute(const FictionBook2TokenData &name, const FictionBook2TokenData *ns, const char *value) override;
  void text(const char *value) override;
};

class FictionBook2IdContext : public FictionBook2ParserContext
{
public:
  explicit FictionBook2IdContext(FictionBook2ParserContext *parentContext);

private:
  FictionBook2XMLParserContext *element(const FictionBook2TokenData &name, const FictionBook2TokenData &ns) override;
  void startOfElement() override;
  void endOfElement() override;
  void endOfAttributes() override {}
  void attribute(const FictionBook2TokenData &name, const FictionBook2TokenData *ns, const char *value) override;
  void text(const char *value) override;
};

class FictionBook2IsbnContext : public FictionBook2ParserContext
{
public:
  explicit FictionBook2IsbnContext(FictionBook2ParserContext *parentContext);

private:
  FictionBook2XMLParserContext *element(const FictionBook2TokenData &name, const FictionBook2TokenData &ns) override;
  void startOfElement() override;
  void endOfElement() override;
  void endOfAttributes() override {}
  void attribute(const FictionBook2TokenData &name, const FictionBook2TokenData *ns, const char *value) override;
  void text(const char *value) override;
};

class FictionBook2KeywordsContext : public FictionBook2ParserContext
{
public:
  explicit FictionBook2KeywordsContext(FictionBook2ParserContext *parentContext);

private:
  FictionBook2XMLParserContext *element(const FictionBook2TokenData &name, const FictionBook2TokenData &ns) override;
  void startOfElement() override;
  void endOfElement() override;
  void endOfAttributes() override {}
  void attribute(const FictionBook2TokenData &name, const FictionBook2TokenData *ns, const char *value) override;
  void text(const char *value) override;
};

class FictionBook2LangContext : public FictionBook2ParserContext
{
public:
  explicit FictionBook2LangContext(FictionBook2ParserContext *parentContext);

private:
  FictionBook2XMLParserContext *element(const FictionBook2TokenData &name, const FictionBook2TokenData &ns) override;
  void startOfElement() override;
  void endOfElement() override;
  void endOfAttributes() override {}
  void attribute(const FictionBook2TokenData &name, const FictionBook2TokenData *ns, const char *value) override;
  void text(const char *value) override;
};

class FictionBook2LastNameContext : public FictionBook2ParserContext
{
public:
  FictionBook2LastNameContext(FictionBook2ParserContext *parentContext, FictionBook2Authors &authors);

private:
  FictionBook2XMLParserContext *element(const FictionBook2TokenData &name, const FictionBook2TokenData &ns) override;
  void startOfElement() override;
  void endOfElement() override;
  void endOfAttributes() override {}
  void attribute(const FictionBook2TokenData &name, const FictionBook2TokenData *ns, const char *value) override;
  void text(const char *value) override;

private:
  FictionBook2Authors &m_authors;
};

class FictionBook2MiddleNameContext : public FictionBook2ParserContext
{
public:
  FictionBook2MiddleNameContext(FictionBook2ParserContext *parentContext, FictionBook2Authors &authors);

private:
  FictionBook2XMLParserContext *element(const FictionBook2TokenData &name, const FictionBook2TokenData &ns) override;
  void startOfElement() override;
  void endOfElement() override;
  void endOfAttributes() override {}
  void attribute(const FictionBook2TokenData &name, const FictionBook2TokenData *ns, const char *value) override;
  void text(const char *value) override;

private:
  FictionBook2Authors &m_authors;
};

class FictionBook2NicknameContext : public FictionBook2ParserContext
{
public:
  FictionBook2NicknameContext(FictionBook2ParserContext *parentContext, FictionBook2Authors &authors);

private:
  FictionBook2XMLParserContext *element(const FictionBook2TokenData &name, const FictionBook2TokenData &ns) override;
  void startOfElement() override;
  void endOfElement() override;
  void endOfAttributes() override {}
  void attribute(const FictionBook2TokenData &name, const FictionBook2TokenData *ns, const char *value) override;
  void text(const char *value) override;

private:
  FictionBook2Authors &m_authors;
};

class FictionBook2OutputContext : public FictionBook2ParserContext
{
public:
  explicit FictionBook2OutputContext(FictionBook2ParserContext *parentContext);

private:
  FictionBook2XMLParserContext *element(const FictionBook2TokenData &name, const FictionBook2TokenData &ns) override;
  void startOfElement() override;
  void endOfElement() override;
  void endOfAttributes() override {}
  void attribute(const FictionBook2TokenData &name, const FictionBook2TokenData *ns, const char *value) override;
  void text(const char *value) override;
};

class FictionBook2OutputDocumentClassContext : public FictionBook2ParserContext
{
public:
  explicit FictionBook2OutputDocumentClassContext(FictionBook2ParserContext *parentContext);

private:
  FictionBook2XMLParserContext *element(const FictionBook2TokenData &name, const FictionBook2TokenData &ns) override;
  void startOfElement() override;
  void endOfElement() override;
  void endOfAttributes() override {}
  void attribute(const FictionBook2TokenData &name, const FictionBook2TokenData *ns, const char *value) override;
  void text(const char *value) override;
};

class FictionBook2PartContext : public FictionBook2ParserContext
{
public:
  explicit FictionBook2PartContext(FictionBook2ParserContext *parentContext);

private:
  FictionBook2XMLParserContext *element(const FictionBook2TokenData &name, const FictionBook2TokenData &ns) override;
  void startOfElement() override;
  void endOfElement() override;
  void endOfAttributes() override {}
  void attribute(const FictionBook2TokenData &name, const FictionBook2TokenData *ns, const char *value) override;
  void text(const char *value) override;
};

class FictionBook2ProgramUsedContext : public FictionBook2ParserContext
{
public:
  explicit FictionBook2ProgramUsedContext(FictionBook2ParserContext *parentContext);

private:
  FictionBook2XMLParserContext *element(const FictionBook2TokenData &name, const FictionBook2TokenData &ns) override;
  void startOfElement() override;
  void endOfElement() override;
  void endOfAttributes() override {}
  void attribute(const FictionBook2TokenData &name, const FictionBook2TokenData *ns, const char *value) override;
  void text(const char *value) override;
};

class FictionBook2PublisherContext : public FictionBook2ParserContext
{
public:
  explicit FictionBook2PublisherContext(FictionBook2ParserContext *parentContext);

private:
  FictionBook2XMLParserContext *element(const FictionBook2TokenData &name, const FictionBook2TokenData &ns) override;
  void startOfElement() override;
  void endOfElement() override;
  void endOfAttributes() override {}
  void attribute(const FictionBook2TokenData &name, const FictionBook2TokenData *ns, const char *value) override;
  void text(const char *value) override;
};

class FictionBook2PublishInfoContext : public FictionBook2ParserContext
{
public:
  explicit FictionBook2PublishInfoContext(FictionBook2ParserContext *parentContext);

private:
  FictionBook2XMLParserContext *element(const FictionBook2TokenData &name, const FictionBook2TokenData &ns) override;
  void startOfElement() override;
  void endOfElement() override;
  void endOfAttributes() override {}
  void attribute(const FictionBook2TokenData &name, const FictionBook2TokenData *ns, const char *value) override;
  void text(const char *value) override;
};

class FictionBook2SequenceContext : public FictionBook2ParserContext
{
public:
  explicit FictionBook2SequenceContext(FictionBook2ParserContext *parentContext);

private:
  FictionBook2XMLParserContext *element(const FictionBook2TokenData &name, const FictionBook2TokenData &ns) override;
  void startOfElement() override;
  void endOfElement() override;
  void endOfAttributes() override {}
  void attribute(const FictionBook2TokenData &name, const FictionBook2TokenData *ns, const char *value) override;
  void text(const char *value) override;
};

class FictionBook2SrcLangContext : public FictionBook2ParserContext
{
public:
  explicit FictionBook2SrcLangContext(FictionBook2ParserContext *parentContext);

private:
  FictionBook2XMLParserContext *element(const FictionBook2TokenData &name, const FictionBook2TokenData &ns) override;
  void startOfElement() override;
  void endOfElement() override;
  void endOfAttributes() override {}
  void attribute(const FictionBook2TokenData &name, const FictionBook2TokenData *ns, const char *value) override;
  void text(const char *value) override;
};

class FictionBook2SrcOcrContext : public FictionBook2ParserContext
{
public:
  explicit FictionBook2SrcOcrContext(FictionBook2ParserContext *parentContext);

private:
  FictionBook2XMLParserContext *element(const FictionBook2TokenData &name, const FictionBook2TokenData &ns) override;
  void startOfElement() override;
  void endOfElement() override;
  void endOfAttributes() override {}
  void attribute(const FictionBook2TokenData &name, const FictionBook2TokenData *ns, const char *value) override;
  void text(const char *value) override;
};

class FictionBook2SrcTitleInfoContext : public FictionBook2ParserContext
{
public:
  explicit FictionBook2SrcTitleInfoContext(FictionBook2ParserContext *parentContext);

private:
  FictionBook2XMLParserContext *element(const FictionBook2TokenData &name, const FictionBook2TokenData &ns) override;
  void startOfElement() override;
  void endOfElement() override;
  void endOfAttributes() override {}
  void attribute(const FictionBook2TokenData &name, const FictionBook2TokenData *ns, const char *value) override;
  void text(const char *value) override;
};

class FictionBook2SrcUrlContext : public FictionBook2ParserContext
{
public:
  explicit FictionBook2SrcUrlContext(FictionBook2ParserContext *parentContext);

private:
  FictionBook2XMLParserContext *element(const FictionBook2TokenData &name, const FictionBook2TokenData &ns) override;
  void startOfElement() override;
  void endOfElement() override;
  void endOfAttributes() override {}
  void attribute(const FictionBook2TokenData &name, const FictionBook2TokenData *ns, const char *value) override;
  void text(const char *value) override;
};

class FictionBook2TitleInfoContext : public FictionBook2ParserContext
{
  // no copying
  FictionBook2TitleInfoContext(const FictionBook2TitleInfoContext &other);
  FictionBook2TitleInfoContext &operator=(const FictionBook2TitleInfoContext &other);

public:
  explicit FictionBook2TitleInfoContext(FictionBook2ParserContext *parentContext);

private:
  ~FictionBook2TitleInfoContext() override;

  FictionBook2XMLParserContext *element(const FictionBook2TokenData &name, const FictionBook2TokenData &ns) override;
  void startOfElement() override;
  void endOfElement() override;
  void endOfAttributes() override {}
  void attribute(const FictionBook2TokenData &name, const FictionBook2TokenData *ns, const char *value) override;
  void text(const char *value) override;

private:
  std::unique_ptr<FictionBook2Authors> m_authors;
};

class FictionBook2TranslatorContext : public FictionBook2ParserContext
{
public:
  explicit FictionBook2TranslatorContext(FictionBook2ParserContext *parentContext);

private:
  FictionBook2XMLParserContext *element(const FictionBook2TokenData &name, const FictionBook2TokenData &ns) override;
  void startOfElement() override;
  void endOfElement() override;
  void endOfAttributes() override {}
  void attribute(const FictionBook2TokenData &name, const FictionBook2TokenData *ns, const char *value) override;
  void text(const char *value) override;
};

class FictionBook2VersionContext : public FictionBook2ParserContext
{
public:
  explicit FictionBook2VersionContext(FictionBook2ParserContext *parentContext);

private:
  FictionBook2XMLParserContext *element(const FictionBook2TokenData &name, const FictionBook2TokenData &ns) override;
  void startOfElement() override;
  void endOfElement() override;
  void endOfAttributes() override {}
  void attribute(const FictionBook2TokenData &name, const FictionBook2TokenData *ns, const char *value) override;
  void text(const char *value) override;
};

class FictionBook2YearContext : public FictionBook2ParserContext
{
public:
  explicit FictionBook2YearContext(FictionBook2ParserContext *parentContext);

private:
  FictionBook2XMLParserContext *element(const FictionBook2TokenData &name, const FictionBook2TokenData &ns) override;
  void startOfElement() override;
  void endOfElement() override;
  void endOfAttributes() override {}
  void attribute(const FictionBook2TokenData &name, const FictionBook2TokenData *ns, const char *value) override;
  void text(const char *value) override;
};

}

#endif // FICTIONBOOK2METADATACONTEXT_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
