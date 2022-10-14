/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <librevenge/librevenge.h>

#include "FictionBook2Style.h"

namespace libebook
{

FictionBook2TextFormat::FictionBook2TextFormat()
  : a(0)
  , code(0)
  , emphasis(0)
  , strikethrough(0)
  , strong(0)
  , sub(0)
  , sup(0)
  , lang()
{
}

FictionBook2BlockFormat::FictionBook2BlockFormat()
  : annotation(0)
  , cite(0)
  , epigraph(0)
  , headerRow(false)
  , p(false)
  , poem(0)
  , stanza(false)
  , subtitle(false)
  , table(false)
  , textAuthor(false)
  , title(false)
  , v(false)
  , headingLevel(0)
  , lang()
{
}

FictionBook2Style::FictionBook2Style(const FictionBook2BlockFormat &blockFormat)
  : m_textFormat()
  , m_blockFormat(blockFormat)
{
}

FictionBook2TextFormat &FictionBook2Style::getTextFormat()
{
  return m_textFormat;
}

const FictionBook2TextFormat &FictionBook2Style::getTextFormat() const
{
  return m_textFormat;
}

const FictionBook2BlockFormat &FictionBook2Style::getBlockFormat() const
{
  return m_blockFormat;
}

librevenge::RVNGPropertyList makePropertyList(const FictionBook2Style &style)
{
  return makePropertyList(style.getTextFormat(), style.getBlockFormat());
}

librevenge::RVNGPropertyList makePropertyList(const FictionBook2TextFormat &format, const FictionBook2BlockFormat &blockFormat)
{
  librevenge::RVNGPropertyList props;

  if ((format.strong > 0) || blockFormat.title || blockFormat.subtitle | blockFormat.headerRow)
    props.insert("fo:font-weight", "bold");
  if ((format.emphasis > 0) || blockFormat.textAuthor)
    props.insert("fo:font-style", "italic");
  if (format.strikethrough > 0)
    props.insert("style:text-line-through-type", "single");
  if (format.sub)
    props.insert("style:text-position", "sub");
  if (format.sup)
    props.insert("style:text-position", "super");
  if (format.code)
    props.insert("style:font-pitch", "fixed");

  return props;
}

librevenge::RVNGPropertyList makePropertyList(const FictionBook2BlockFormat &format)
{
  librevenge::RVNGPropertyList props;

  if (format.epigraph)
    props.insert("fo:text-align", "right");
  else if (format.title || format.subtitle)
    props.insert("fo:text-align", "left");
  else if (format.p)
    props.insert("fo:text-align", "justify");
  else
    props.insert("fo:text-align", "left");

  return props;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
