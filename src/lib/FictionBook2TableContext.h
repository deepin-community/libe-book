/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef FICTIONBOOK2TABLECONTEXT_H_INCLUDED
#define FICTIONBOOK2TABLECONTEXT_H_INCLUDED

#include <memory>

#include "FictionBook2ParserContext.h"

namespace libebook
{

class FictionBook2TableModel;

class FictionBook2TableContext : public FictionBook2BlockFormatContextBase
{
  // disable copying
  FictionBook2TableContext(const FictionBook2TableContext &other);
  FictionBook2TableContext &operator=(const FictionBook2TableContext &other);

public:
  FictionBook2TableContext(FictionBook2ParserContext *parentContext, const FictionBook2BlockFormat &format);
  ~FictionBook2TableContext() override;

private:
  FictionBook2XMLParserContext *element(const FictionBook2TokenData &name, const FictionBook2TokenData &ns) override;
  void startOfElement() override;
  void endOfElement() override;
  void attribute(const FictionBook2TokenData &name, const FictionBook2TokenData *ns, const char *value) override;

private:
  std::shared_ptr<FictionBook2TableModel> m_model;
};

class FictionBook2CellContext : public FictionBook2StyleContextBase
{
  // disable copying
  FictionBook2CellContext(const FictionBook2CellContext &other);
  FictionBook2CellContext &operator=(const FictionBook2CellContext &other);

public:
  FictionBook2CellContext(FictionBook2ParserContext *parentContext, const std::shared_ptr<FictionBook2TableModel> &model, const FictionBook2BlockFormat &format, bool header);

private:
  void startOfElement() override;
  void endOfElement() override;
  void attribute(const FictionBook2TokenData &name, const FictionBook2TokenData *ns, const char *value) override;
  void endOfAttributes() override;

  void openCell();

private:
  const std::shared_ptr<FictionBook2TableModel> m_model;
  const bool m_header;
  bool m_opened;
  int m_columnSpan;
  int m_rowSpan;
  size_t m_coveredColumns;
};

class FictionBook2TrContext : public FictionBook2BlockFormatContextBase
{
  // no copying
  FictionBook2TrContext(const FictionBook2TrContext &);
  FictionBook2TrContext &operator=(const FictionBook2TrContext &);

public:
  FictionBook2TrContext(FictionBook2ParserContext *parentContext, const std::shared_ptr<FictionBook2TableModel> &model, const FictionBook2BlockFormat &format);

private:
  FictionBook2XMLParserContext *element(const FictionBook2TokenData &name, const FictionBook2TokenData &ns) override;
  void endOfElement() override;
  void attribute(const FictionBook2TokenData &name, const FictionBook2TokenData *ns, const char *value) override;

  void openRow(bool header);

private:
  const std::shared_ptr<FictionBook2TableModel> m_model;
  bool m_opened;
};

}

#endif // FICTIONBOOK2TABLECONTEXT_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
