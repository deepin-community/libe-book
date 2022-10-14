/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef FICTIONBOOK2TOKEN_H_INCLUDED
#define FICTIONBOOK2TOKEN_H_INCLUDED

#include "EBOOKToken.h"

namespace libebook
{

#define FictionBook2_NO_NAMESPACE(ns) (0 == ns)

namespace FictionBook2Token
{

enum
{
  INVALID_TOKEN = 0,
  FIRST_TOKEN = EBOOKToken<FictionBook2Parser>::FIRST_TOKEN,

  // namespaces
  NS_FICTIONBOOK,
  NS_XLINK,
  NS_XML,
  NS_XMLNS,

  // elements
  a,
  annotation,
  author,
  binary,
  body,
  book_name,
  book_title,
  cite,
  city,
  code,
  coverpage,
  custom_info,
  date,
  description,
  document_info,
  email,
  emphasis,
  empty_line,
  epigraph,
  FictionBook,
  first_name,
  genre,
  history,
  home_page,
  id,
  image,
  isbn,
  keywords,
  lang,
  last_name,
  middle_name,
  nickname,
  output,
  output_document,
  p,
  part,
  poem,
  program_used,
  publisher,
  publish_info,
  section,
  sequence,
  src_lang,
  src_ocr,
  src_title_info,
  src_url,
  stanza,
  strikethrough,
  strong,
  style,
  stylesheet,
  sub,
  subtitle,
  sup,
  table,
  td,
  text_author,
  th,
  title,
  title_info,
  tr,
  translator,
  v,
  version,
  year,

  // attributes
  align,
  alt,
  colspan,
  content_type,
  create,
  currency,
  href,
  include,
  include_all,
  info_type,
  match,
  mode,
  name,
  number,
  price,
  rowspan,
  type,
  value,

  // attribute values
  image_jpg,
  note,
  simple,

  LAST_TOKEN
};

static_assert(EBOOKToken<FictionBook2Parser>::LAST_TOKEN >= LAST_TOKEN, "token number overflow");

}

struct FictionBook2TokenData
{
  const char *name;
  int id;
};

const FictionBook2TokenData *getFictionBook2Token(const char *str, const unsigned length);
const FictionBook2TokenData *getFictionBook2Token(const char *str);
const FictionBook2TokenData *getFictionBook2Token(const unsigned char *str);

int getFictionBook2TokenID(const FictionBook2TokenData &token);
int getFictionBook2TokenID(const FictionBook2TokenData *token);
int getFictionBook2TokenID(const char *str, const unsigned length);
int getFictionBook2TokenID(const char *str);
int getFictionBook2TokenID(const unsigned char *str);

const char *getFictionBook2TokenName(const FictionBook2TokenData &token);
const char *getFictionBook2TokenName(const FictionBook2TokenData *token);
const char *getFictionBook2TokenName(const char *str, const unsigned length);
const char *getFictionBook2TokenName(const char *str);
const char *getFictionBook2TokenName(const unsigned char *str);

}

#endif // FICTIONBOOK2TOKEN_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
