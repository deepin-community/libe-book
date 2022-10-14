/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef EBOOKOPFTOKEN_H_INCLUDED
#define EBOOKOPFTOKEN_H_INCLUDED

#include "EBOOKToken.h"

namespace libebook
{

namespace EBOOKOPFToken
{

enum
{
  INVALID_TOKEN = 0,
  FIRST_TOKEN = EBOOKToken<EBOOKOPFParser>::FIRST_TOKEN,

  // namespace prefixes
  dc,
  opf,
  xml,
  xmlns,

  // elements
  Contributor,
  Coverage,
  Creator,
  Date,
  Description,
  Format,
  Identifier,
  Language,
  Publisher,
  Relation,
  Rights,
  Source,
  Subject,
  Title,
  Type,
  contributor,
  coverage,
  creator,
  date,
  dc_metadata,
  description,
  format,
  guide,
  identifier,
  item,
  itemref,
  language,
  manifest,
  meta,
  metadata,
  package,
  publisher,
  reference,
  relation,
  rights,
  site,
  source,
  spine,
  subject,
  title,
  tour,
  tours,
  type,
  x_metadata,

  // attributes
  content,
  event,
  fallback,
  fallback_style,
  file_as,
  href,
  id,
  idref,
  lang,
  linear,
  media_type,
  name,
  required_modules,
  required_namespace,
  role,
  scheme,
  toc,
  unique_identifier,
  version,

  // attribute values
  MIME_image,
  MIME_oeb,
  MIME_xhtml,
  _2_0,
  _3_0,
  aut,
  creation,
  edt,
  modification,
  no,
  publication,
  yes,

  LAST_TOKEN
};

static_assert(EBOOKToken<EBOOKOPFParser>::LAST_TOKEN >= LAST_TOKEN, "token number overflow");

enum Namespace
{
  NS_dc = dc << 16,
  NS_opf = opf << 16,
  NS_xml = xml << 16,
  NS_xmlns = xmlns << 16
};

}

int getOPFTokenId(const char *name, const char *ns = nullptr);
int getOPFTokenId(const char *name, std::size_t nameLen, const char *ns, std::size_t nsLen);
int getOPFTokenId(const char *name, std::size_t nameLen);

}

#endif // EBOOKOPFTOKEN_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
