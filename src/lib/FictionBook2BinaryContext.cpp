/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <librevenge/librevenge.h>

#include "FictionBook2BinaryContext.h"
#include "FictionBook2Collector.h"
#include "FictionBook2Token.h"

namespace libebook
{

FictionBook2BinaryContext::FictionBook2BinaryContext(FictionBook2ParserContext *const parentContext, FictionBook2Collector *const collector)
  : FictionBook2NodeContextBase(parentContext, collector)
  , m_id()
  , m_contentType()
{
}

FictionBook2XMLParserContext *FictionBook2BinaryContext::element(const FictionBook2TokenData &, const FictionBook2TokenData &)
{
  return nullptr;
}

void FictionBook2BinaryContext::endOfElement()
{
}

void FictionBook2BinaryContext::attribute(const FictionBook2TokenData &name, const FictionBook2TokenData *ns, const char *value)
{
  if (FictionBook2_NO_NAMESPACE(ns))
  {
    switch (getFictionBook2TokenID(name))
    {
    case FictionBook2Token::content_type :
      // I have seen image/jpg used in at least one file
      if (FictionBook2Token::image_jpg == getFictionBook2TokenID(value))
        m_contentType = "image/jpeg";
      else
        m_contentType = value;
      break;
    case FictionBook2Token::id :
      m_id = value;
      break;
    default :
      break;
    }
  }
}

void FictionBook2BinaryContext::text(const char *value)
{
  getCollector()->defineID(m_id.c_str());
  getCollector()->insertBitmapData(m_contentType.c_str(), value);
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
