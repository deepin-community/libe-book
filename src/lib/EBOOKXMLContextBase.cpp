/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "EBOOKXMLContextBase.h"

#include "libebook_utils.h"

namespace libebook
{

using std::shared_ptr;

void EBOOKXMLElementContextBase::attribute(const int, const char *)
{
}

void EBOOKXMLElementContextBase::endOfAttributes()
{
}

void EBOOKXMLElementContextBase::text(const char *)
{
  EBOOK_DEBUG_MSG(("text content is not expected at this element\n"));
}

void EBOOKXMLElementContextBase::endOfElement()
{
}

void EBOOKXMLTextContextBase::startOfElement()
{
}

void EBOOKXMLTextContextBase::attribute(const int, const char *)
{
}

void EBOOKXMLTextContextBase::endOfAttributes()
{
}

shared_ptr<EBOOKXMLContext> EBOOKXMLTextContextBase::element(int)
{
  EBOOK_DEBUG_MSG(("no subelement is expected at this element\n"));
  return shared_ptr<EBOOKXMLContext>();
}

void EBOOKXMLTextContextBase::endOfElement()
{
}

void EBOOKXMLMixedContextBase::startOfElement()
{
}

void EBOOKXMLMixedContextBase::attribute(const int, const char *)
{
}

void EBOOKXMLMixedContextBase::endOfAttributes()
{
}

void EBOOKXMLMixedContextBase::endOfElement()
{
}

void EBOOKXMLEmptyContextBase::startOfElement()
{
}

void EBOOKXMLEmptyContextBase::endOfAttributes()
{
}

shared_ptr<EBOOKXMLContext> EBOOKXMLEmptyContextBase::element(int)
{
  EBOOK_DEBUG_MSG(("no subelement is expected at this element\n"));
  return shared_ptr<EBOOKXMLContext>();
}

void EBOOKXMLEmptyContextBase::text(const char *)
{
  EBOOK_DEBUG_MSG(("text content is not expected at this element\n"));
}

void EBOOKXMLEmptyContextBase::endOfElement()
{
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
