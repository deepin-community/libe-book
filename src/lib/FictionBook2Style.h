/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef FICTIONBOOK2STYLE_H_INCLUDED
#define FICTIONBOOK2STYLE_H_INCLUDED

#include <string>

#include "libebook_utils.h"


namespace libebook
{

struct FictionBook2TextFormat
{
  FictionBook2TextFormat();

  uint8_t a;
  uint8_t code;
  uint8_t emphasis;
  uint8_t strikethrough;
  uint8_t strong;
  uint8_t sub;
  uint8_t sup;
  std::string lang;
};

struct FictionBook2BlockFormat
{
  FictionBook2BlockFormat();

  uint8_t annotation;
  uint8_t cite;
  uint8_t epigraph;
  bool headerRow;
  bool p;
  uint8_t poem;
  bool stanza;
  bool subtitle;
  bool table;
  bool textAuthor;
  bool title;
  bool v;
  uint8_t headingLevel;
  std::string lang;
};

class FictionBook2Style
{
public:
  explicit FictionBook2Style(const FictionBook2BlockFormat &blockFormat);

  FictionBook2TextFormat &getTextFormat();
  const FictionBook2TextFormat &getTextFormat() const;
  const FictionBook2BlockFormat &getBlockFormat() const;

private:
  FictionBook2TextFormat m_textFormat;
  const FictionBook2BlockFormat m_blockFormat;
};

librevenge::RVNGPropertyList makePropertyList(const FictionBook2Style &style);
librevenge::RVNGPropertyList makePropertyList(const FictionBook2TextFormat &format, const FictionBook2BlockFormat &blockFormat);
librevenge::RVNGPropertyList makePropertyList(const FictionBook2BlockFormat &format);

}

#endif // FICTIONBOOK2STYLE_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
