/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef BBEBTYPES_H_INCLUDED
#define BBEBTYPES_H_INCLUDED

#include <deque>
#include <map>
#include <string>

#include <boost/optional.hpp>

namespace libebook
{

enum BBeBImageType
{
  BBEB_IMAGE_TYPE_UNKNOWN = 0,
  BBEB_IMAGE_TYPE_JPEG = 0x11,
  BBEB_IMAGE_TYPE_PNG = 0x12,
  BBEB_IMAGE_TYPE_BMP = 0x13,
  BBEB_IMAGE_TYPE_GIF = 0x14
};

enum BBeBEmptyLinePosition
{
  BBEB_EMPTY_LINE_POSITION_UNKNOWN,
  BBEB_EMPTY_LINE_POSITION_BEFORE,
  BBEB_EMPTY_LINE_POSITION_AFTER
};

enum BBeBEmptyLineMode
{
  BBEB_EMPTY_LINE_MODE_NONE,
  BBEB_EMPTY_LINE_MODE_SOLID,
  BBEB_EMPTY_LINE_MODE_DASHED,
  BBEB_EMPTY_LINE_MODE_DOUBLE,
  BBEB_EMPTY_LINE_MODE_DOTTED
};

enum BBeBAlign
{
  BBEB_ALIGN_START,
  BBEB_ALIGN_END,
  BBEB_ALIGN_CENTER
};

struct BBeBBookInfo
{
  BBeBBookInfo();

  std::string author;
  std::string title;
  std::string bookID;
  std::string publisher;
  boost::optional<std::string> label;
  std::deque<std::string> categories;
  std::string classification;
  boost::optional<std::string> freeText;
};

struct BBeBDocInfo
{
  BBeBDocInfo();

  std::string language;
  std::string creator;
  std::string creationDate;
  std::string producer;
  unsigned page;
};

struct BBeBMetadata
{
  BBeBMetadata();

  BBeBBookInfo bookInfo;
  BBeBDocInfo docInfo;
  boost::optional<std::string> keyword;
};

struct BBeBColor
{
  BBeBColor();
  explicit BBeBColor(unsigned color);
  BBeBColor(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha = 0);

  unsigned char r;
  unsigned char g;
  unsigned char b;
  unsigned char a;
};

struct BBeBEmptyLine
{
  BBeBEmptyLine();

  BBeBEmptyLinePosition m_position;
  BBeBEmptyLineMode m_mode;
};

struct BBeBAttributes
{
  BBeBAttributes();

  boost::optional<unsigned> fontSize;
  boost::optional<unsigned> fontWidth;
  boost::optional<unsigned> fontEscapement;
  boost::optional<unsigned> fontOrientation;
  boost::optional<unsigned> fontWeight;
  boost::optional<std::string> fontFacename;
  boost::optional<BBeBColor> textColor;
  boost::optional<BBeBColor> textBgColor;
  boost::optional<unsigned> wordSpace;
  boost::optional<unsigned> letterSpace;
  boost::optional<unsigned> baseLineSkip;
  boost::optional<unsigned> lineSpace;
  boost::optional<unsigned> parIndent;
  boost::optional<unsigned> parSkip;
  boost::optional<unsigned> height;
  boost::optional<unsigned> width;
  boost::optional<unsigned> locationX;
  boost::optional<unsigned> locationY;
  bool italic;
  bool sup;
  bool sub;
  bool heading;
  boost::optional<BBeBEmptyLine> emptyLine;
  boost::optional<BBeBAlign> align;
  boost::optional<unsigned> topSkip;
  boost::optional<unsigned> topMargin;
  boost::optional<unsigned> oddSideMargin;
  boost::optional<unsigned> evenSideMargin;
};

typedef std::map<unsigned, BBeBAttributes> BBeBAttributeMap_t;

}

#endif // BBEBTYPES_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
