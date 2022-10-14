/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef EPUBTOKEN_H_INCLUDED
#define EPUBTOKEN_H_INCLUDED

#include "EBOOKToken.h"

namespace libebook
{

namespace EPubToken
{

enum
{
  INVALID_TOKEN = 0,
  FIRST_TOKEN = EBOOKToken<EPubParser>::FIRST_TOKEN,

  // namespace prefixes
  container,
  xml,
  xmlns,

  // elements
  rootfiles,
  rootfile,
  signatures,
  encryption,

  // attributes
  version,
  full_path,
  media_type,

  // attribute values
  _1_0,
  MIME_epub,
  MIME_opf,

  LAST_TOKEN
};

static_assert(EBOOKToken<EPubParser>::LAST_TOKEN >= LAST_TOKEN, "token number overflow");

enum Namespace
{
  NS_container = container << 16,
  NS_xml = xml << 16,
  NS_xmlns = xmlns << 16
};

}

int getEPubTokenId(const char *name, const char *ns = nullptr);
int getEPubTokenId(const char *name, std::size_t nameLen, const char *ns, std::size_t nsLen);
int getEPubTokenId(const char *name, std::size_t nameLen);

}

#endif // EPUBTOKEN_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
