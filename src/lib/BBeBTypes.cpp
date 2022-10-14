/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "BBeBTypes.h"

namespace libebook
{

BBeBBookInfo::BBeBBookInfo()
  : author()
  , title()
  , bookID()
  , publisher()
  , label()
  , categories()
  , classification()
  , freeText()
{
}

BBeBDocInfo::BBeBDocInfo()
  : language()
  , creator()
  , creationDate()
  , producer()
  , page(0)
{
}

BBeBMetadata::BBeBMetadata()
  : bookInfo()
  , docInfo()
  , keyword()
{
}

BBeBColor::BBeBColor()
  : r(0)
  , g(0)
  , b(0)
  , a(0)
{
}

BBeBColor::BBeBColor(const unsigned color)
  : r((unsigned char)((color >> 24) & 0xff))
  , g((unsigned char)((color >> 16) & 0xff))
  , b((unsigned char)((color >> 8) & 0xff))
  , a((unsigned char)(color & 0xff))
{
}

BBeBColor::BBeBColor(const unsigned char red, const unsigned char green, const unsigned char blue, const unsigned char alpha)
  : r(red)
  , g(green)
  , b(blue)
  , a(alpha)
{
}

BBeBEmptyLine::BBeBEmptyLine()
  : m_position(BBEB_EMPTY_LINE_POSITION_UNKNOWN)
  , m_mode(BBEB_EMPTY_LINE_MODE_NONE)
{
}

BBeBAttributes::BBeBAttributes()
  : fontSize()
  , fontWidth()
  , fontEscapement()
  , fontOrientation()
  , fontWeight()
  , fontFacename()
  , textColor()
  , textBgColor()
  , wordSpace()
  , letterSpace()
  , baseLineSkip()
  , lineSpace()
  , parIndent()
  , parSkip()
  , height()
  , width()
  , locationX()
  , locationY()
  , italic(false)
  , sup(false)
  , sub(false)
  , heading(false)
  , emptyLine()
  , align()
  , topSkip()
  , topMargin()
  , oddSideMargin()
  , evenSideMargin()
{
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */

