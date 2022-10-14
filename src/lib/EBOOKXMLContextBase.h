/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef EBOOKXMLCONTEXTBASE_H_INCLUDED
#define EBOOKXMLCONTEXTBASE_H_INCLUDED

#include "EBOOKXMLContext.h"

namespace libebook
{

class EBOOKXMLElementContextBase : public EBOOKXMLContext
{
protected:
  void startOfElement() override;
  void attribute(int name, const char *value) override;
  void endOfAttributes() override;
  void text(const char *value) override;
  void endOfElement() override;
};

class EBOOKXMLTextContextBase : public EBOOKXMLContext
{
protected:
  void startOfElement() override;
  void attribute(int name, const char *value) override;
  void endOfAttributes() override;
  std::shared_ptr<EBOOKXMLContext> element(int name) override;
  void endOfElement() override;
};

class EBOOKXMLMixedContextBase : public EBOOKXMLContext
{
protected:
  void startOfElement() override;
  void attribute(int name, const char *value) override;
  void endOfAttributes() override;
  void endOfElement() override;
};

class EBOOKXMLEmptyContextBase : public EBOOKXMLContext
{
protected:
  void startOfElement() override;
  void endOfAttributes() override;
  std::shared_ptr<EBOOKXMLContext> element(int name) override;
  void text(const char *value) override;
  void endOfElement() override;
};

}

#endif // EBOOKXMLCONTEXTBASE_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
