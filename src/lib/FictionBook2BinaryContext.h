/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef FICTIONBOOK2BINARYCONTEXT_H_INCLUDED
#define FICTIONBOOK2BINARYCONTEXT_H_INCLUDED

#include <string>

#include "FictionBook2ParserContext.h"

namespace libebook
{

class FictionBook2BinaryContext : public FictionBook2NodeContextBase
{
public:
  FictionBook2BinaryContext(FictionBook2ParserContext *parentContext, FictionBook2Collector *collector);

private:
  FictionBook2XMLParserContext *element(const FictionBook2TokenData &name, const FictionBook2TokenData &ns) override;
  void endOfElement() override;
  void attribute(const FictionBook2TokenData &name, const FictionBook2TokenData *ns, const char *value) override;
  void text(const char *text) override;

private:
  std::string m_id;
  std::string m_contentType;
};

}

#endif // FICTIONBOOK2BINARYCONTEXT_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
