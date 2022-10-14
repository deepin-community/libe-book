/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <unordered_map>

#include <librevenge/librevenge.h>

#include "BBeBCollector.h"

using std::string;

namespace libebook
{

namespace
{

const unsigned DEFAULT_FONT_SIZE = 10; // pt
const unsigned DEFAULT_PARA_IDENT = 10; // pt
const BBeBColor DEFAULT_BACKGROUND_COLOR(255, 255, 255);

// TODO: this is not ideal. Could we (mis)use defineEmbeddedFont to
// define just font properties? E.g., a font being serif/sans-serif/monospace.
const std::unordered_map<string, string> FONT_REPLACEMENT_TABLE
{
  {"Dutch801 Rm BT Roman", "Liberation Serif"},
  {"Swis721 BT Roman", "Liberation Sans"},
  {"Courier10 BT Roman", "Liberation Mono"},
};

}

namespace
{

double toInches(const unsigned px, const double dpi)
{
  return double(px) / dpi;
}

const BBeBColor combine(const BBeBColor &fg, const BBeBColor &bg)
{
  assert(0 == bg.a);

  const double a = 1 - fg.a / 255.0;

  const double r = (1 - a) * bg.r + a * fg.r;
  const double g = (1 - a) * bg.g + a * fg.g;
  const double b = (1 - a) * bg.b + a * fg.b;

  return BBeBColor((unsigned char)(r + 0.5), (unsigned char)(g + 0.5), (unsigned char)(b + 0.5));
}

const librevenge::RVNGString makeColor(const BBeBColor &color)
{
  librevenge::RVNGString str;
  str.sprintf("#%02x%02x%02x", color.r, color.g, color.b);
  return str;
}

void insert(librevenge::RVNGPropertyList &props, const char *const name, const unsigned value, const double dpi)
{
  props.insert(name, toInches(value, dpi));
}

void insert(librevenge::RVNGPropertyList &props, const char *const name, const unsigned value)
{
  props.insert(name, static_cast<int>(value));
}

void insertRelative(librevenge::RVNGPropertyList &props, const char *const name, const unsigned value, const unsigned base)
{
  props.insert(name, int(double(value) / 100 * base + 0.5), librevenge::RVNG_POINT);
}

void insert(librevenge::RVNGPropertyList &props, const char *const name, const string &value)
{
  props.insert(name, value.c_str());
}

void insert(librevenge::RVNGPropertyList &props, const char *const name, const BBeBColor &value, const BBeBColor &background)
{
  if (0 == value.a)
    props.insert(name, makeColor(value));
  else
    props.insert(name, makeColor(combine(value, background)));
}

void writeLine(librevenge::RVNGPropertyList &props, const BBeBEmptyLineMode &mode, const char *const typePropName, const char *const stylePropName)
{
  switch (mode)
  {
  case BBEB_EMPTY_LINE_MODE_NONE :
    insert(props, typePropName, "none");
    break;
  case BBEB_EMPTY_LINE_MODE_SOLID:
    insert(props, typePropName, "single");
    insert(props, stylePropName, "solid");
    break;
  case BBEB_EMPTY_LINE_MODE_DASHED:
    insert(props, typePropName, "single");
    insert(props, stylePropName, "dash");
    break;
  case BBEB_EMPTY_LINE_MODE_DOUBLE :
    insert(props, typePropName, "double");
    insert(props, stylePropName, "solid");
    break;
  case BBEB_EMPTY_LINE_MODE_DOTTED:
    insert(props, typePropName, "single");
    insert(props, stylePropName, "dotted");
    break;
  default:
    break;
  }
}

librevenge::RVNGPropertyList makeCharacterProperties(const BBeBAttributes &attributes, const double /*dpi*/)
{
  librevenge::RVNGPropertyList props;

  if (attributes.fontSize)
    insertRelative(props, "fo:font-size", get(attributes.fontSize), DEFAULT_FONT_SIZE);
  if (attributes.fontWeight && (400 < get(attributes.fontWeight)))
    insert(props, "fo:font-weight", "bold");
  if (attributes.fontFacename)
  {
    const std::unordered_map<string, string>::const_iterator it = FONT_REPLACEMENT_TABLE.find(get(attributes.fontFacename));
    if (FONT_REPLACEMENT_TABLE.end() == it)
      insert(props, "style:font-name", get(attributes.fontFacename));
    else
      insert(props, "style:font-name", it->second);
  }
  if (attributes.italic)
    insert(props, "fo:font-style", "italic");
  if (attributes.sub)
    insert(props, "style:text-position", "sub");
  if (attributes.sup)
    insert(props, "style:text-position", "super");
  if (attributes.emptyLine)
  {
    switch (get(attributes.emptyLine).m_position)
    {
    case BBEB_EMPTY_LINE_POSITION_BEFORE :
      writeLine(props, get(attributes.emptyLine).m_mode, "style:text-overline-type", "style:text-overline-style");
      break;
    case BBEB_EMPTY_LINE_POSITION_AFTER :
      writeLine(props, get(attributes.emptyLine).m_mode, "style:text-underline-type", "style:text-underline-style");
      break;
    case BBEB_EMPTY_LINE_POSITION_UNKNOWN:
    default :
      break;
    }
  }

  {
    BBeBColor bgColor(DEFAULT_BACKGROUND_COLOR);

    if (attributes.textBgColor)
    {
      if (0 == get(attributes.textBgColor).a)
        bgColor = get(attributes.textBgColor);
      else
        bgColor = combine(get(attributes.textBgColor), DEFAULT_BACKGROUND_COLOR);
      props.insert("fo:background-color", makeColor(bgColor));
    }
    if (attributes.textColor)
      insert(props, "fo:color", get(attributes.textColor), bgColor);
  }

  return props;
}

librevenge::RVNGPropertyList makePageProperties(const BBeBAttributes &attributes, const double dpi)
{
  librevenge::RVNGPropertyList props;

  if (attributes.width)
    insert(props, "fo:page-width", get(attributes.width), dpi);
  if (attributes.height)
    insert(props, "fo:page-height", get(attributes.height), dpi);
  if (attributes.oddSideMargin && attributes.evenSideMargin)
  {
    // We have to choose one...
    const unsigned margin = std::min(get(attributes.oddSideMargin), get(attributes.evenSideMargin));
    insert(props, "fo:margin-left", margin, dpi);
    insert(props, "fo:margin-right", margin, dpi);
  }
  else if (attributes.oddSideMargin)
  {
    insert(props, "fo:margin-left", get(attributes.oddSideMargin), dpi);
    insert(props, "fo:margin-right", get(attributes.oddSideMargin), dpi);
  }
  else if (attributes.evenSideMargin)
  {
    insert(props, "fo:margin-left", get(attributes.evenSideMargin), dpi);
    insert(props, "fo:margin-right", get(attributes.evenSideMargin), dpi);
  }
  else
  {
    insert(props, "fo:margin-left", 0);
    insert(props, "fo:margin-right", 0);
  }
  if (attributes.topMargin)
    insert(props, "fo:margin-top", get(attributes.topMargin), dpi);
  insert(props, "fo:margin-bottom", 0);

  return props;
}

librevenge::RVNGPropertyList makeParagraphProperties(const BBeBAttributes &attributes, const double /*dpi*/)
{
  librevenge::RVNGPropertyList props;

  if (attributes.parIndent)
    insertRelative(props, "fo:text-indent", get(attributes.parIndent), DEFAULT_PARA_IDENT);
  if (attributes.baseLineSkip)
    props.insert("fo:line-height", double(get(attributes.baseLineSkip)) / 1000 * DEFAULT_FONT_SIZE, librevenge::RVNG_PERCENT);
  if (attributes.align)
  {
    switch (get(attributes.align))
    {
    case BBEB_ALIGN_START :
      // Emulate behavior of readers.
      if (attributes.heading)
        insert(props, "fo:text-align", "left");
      else
        insert(props, "fo:text-align", "justify");
      break;
    case BBEB_ALIGN_END :
      insert(props, "fo:text-align", "end");
      break;
    case BBEB_ALIGN_CENTER :
      insert(props, "fo:text-align", "center");
      break;
    default:
      break;
    }
  }
  if (attributes.heading)
  {
    props.insert("style:display-name", "BBeB Heading");
    props.insert("text:outline-level", "1"); // BBeB has only got one level of headings
  }

  return props;
}

void merge(BBeBAttributes &merged, const BBeBAttributes &other)
{
  if (other.fontSize)
    merged.fontSize = other.fontSize;
  if (other.fontWidth)
    merged.fontWidth = other.fontWidth;
  if (other.fontEscapement)
    merged.fontEscapement = other.fontEscapement;
  if (other.fontOrientation)
    merged.fontOrientation = other.fontOrientation;
  if (other.fontWeight)
    merged.fontWeight = other.fontWeight;
  if (other.fontFacename)
    merged.fontFacename = other.fontFacename;
  if (other.textColor)
    merged.textColor = other.textColor;
  if (other.textBgColor)
    merged.textBgColor = other.textBgColor;
  if (other.wordSpace)
    merged.wordSpace = other.wordSpace;
  if (other.letterSpace)
    merged.letterSpace = other.letterSpace;
  if (other.baseLineSkip)
    merged.baseLineSkip = other.baseLineSkip;
  if (other.lineSpace)
    merged.lineSpace = other.lineSpace;
  if (other.parIndent)
    merged.parIndent = other.parIndent;
  if (other.parSkip)
    merged.parSkip = other.parSkip;
  if (other.height)
    merged.height = other.height;
  if (other.width)
    merged.width = other.width;
  if (other.locationX)
    merged.locationX = other.locationX;
  if (other.locationY)
    merged.locationY = other.locationY;
  merged.italic = other.italic;
  merged.sup = other.sup;
  merged.sub = other.sub;
  if (other.heading)
    merged.heading = other.heading;
  if (other.emptyLine)
    merged.emptyLine = other.emptyLine;
  if (other.align)
    merged.align = other.align;
  if (other.topSkip)
    merged.topSkip = other.topSkip;
  if (other.topMargin)
    merged.topMargin = other.topMargin;
  if (other.oddSideMargin)
    merged.oddSideMargin = other.oddSideMargin;
  if (other.evenSideMargin)
    merged.evenSideMargin = other.evenSideMargin;
}

}

BBeBCollector::ImageStreamData::ImageStreamData()
  : image()
  , type()
{
}

BBeBCollector::BBeBCollector(librevenge::RVNGTextInterface *const document)
  : m_bookAttributes()
  , m_textAttributeMap()
  , m_blockAttributeMap()
  , m_pageAttributeMap()
  , m_paragraphAttributeMap()
  , m_document(document)
  , m_currentAttributes()
  , m_imageDataMap()
  , m_imageMap()
  , m_dpi(166.0)
  , m_firstParaInBlock(false)
{
}

BBeBCollector::~BBeBCollector()
{
}

void BBeBCollector::startDocument()
{
  m_document->startDocument(librevenge::RVNGPropertyList());
  m_currentAttributes.push(m_bookAttributes);
}

void BBeBCollector::endDocument()
{
  m_document->endDocument();
  m_currentAttributes.pop();
}

void BBeBCollector::openPage(const unsigned pageAtrID, const BBeBAttributes &attributes)
{
  openBlock(pageAtrID, attributes, &m_pageAttributeMap);
  m_document->openPageSpan(makePageProperties(m_currentAttributes.top(), m_dpi));
}

void BBeBCollector::closePage()
{
  m_document->closePageSpan();
  closeBlock();
}

void BBeBCollector::openBlock(unsigned atrID, const BBeBAttributes &attributes)
{
  openBlock(atrID, attributes, &m_blockAttributeMap);
}

void BBeBCollector::closeBlock()
{
  m_currentAttributes.pop();
}

void BBeBCollector::openTextBlock(unsigned atrID, const BBeBAttributes &attributes)
{
  openBlock(atrID, attributes, &m_textAttributeMap);
  m_firstParaInBlock = true;
}

void BBeBCollector::closeTextBlock()
{
  closeBlock();
}

void BBeBCollector::openParagraph(unsigned atrID, const BBeBAttributes &attributes)
{
  openBlock(atrID, attributes, &m_paragraphAttributeMap);

  librevenge::RVNGPropertyList props(makeParagraphProperties(m_currentAttributes.top(), m_dpi));

  if (m_firstParaInBlock)
  {
    if (m_currentAttributes.top().topSkip)
      insert(props, "fo:margin-top", get(m_currentAttributes.top().topSkip), m_dpi);

    m_firstParaInBlock = false;
  }

  m_document->openParagraph(props);
}

void BBeBCollector::closeParagraph()
{
  m_document->closeParagraph();
  closeBlock();
}

void BBeBCollector::collectText(const std::string &text, const BBeBAttributes &attributes)
{
  openBlock(0, attributes, nullptr);
  m_document->openSpan(makeCharacterProperties(m_currentAttributes.top(), m_dpi));
  m_document->insertText(librevenge::RVNGString(text.c_str()));
  m_document->closeSpan();
  closeBlock();
}

void BBeBCollector::insertLineBreak()
{
  m_document->insertLineBreak();
}

void BBeBCollector::collectMetadata(const BBeBMetadata &metadata)
{
  // TODO: implement me
  (void) metadata;

  librevenge::RVNGPropertyList props;

  m_document->setDocumentMetaData(props);
}

void BBeBCollector::collectBookAttributes(const BBeBAttributes &attributes)
{
  m_bookAttributes = attributes;
}

void BBeBCollector::collectTextAttributes(const unsigned id, const BBeBAttributes &attributes)
{
  collectAttributes(id, attributes, m_textAttributeMap);
}

void BBeBCollector::collectBlockAttributes(const unsigned id, const BBeBAttributes &attributes)
{
  collectAttributes(id, attributes, m_blockAttributeMap);
}

void BBeBCollector::collectPageAttributes(const unsigned id, const BBeBAttributes &attributes)
{
  collectAttributes(id, attributes, m_pageAttributeMap);
}

void BBeBCollector::collectParagraphAttributes(const unsigned id, const BBeBAttributes &attributes)
{
  collectAttributes(id, attributes, m_paragraphAttributeMap);
}

void BBeBCollector::collectImage(const unsigned id, const unsigned dataId, const unsigned width, const unsigned height)
{
  if ((0 == id) || (0 == dataId))
  {
    EBOOK_DEBUG_MSG(("invalid image id 0\n"));
    return;
  }

  const ImageDataMap_t::const_iterator it = m_imageDataMap.find(dataId);
  if (it == m_imageDataMap.end())
  {
    EBOOK_DEBUG_MSG(("image data with id %x is not known\n", dataId));
    return;
  }

  m_imageMap[id].dataId = dataId;
  m_imageMap[id].width = width;
  m_imageMap[id].height = height;
}

void BBeBCollector::insertImage(const unsigned id)
{
  if (0 == id)
  {
    EBOOK_DEBUG_MSG(("invalid image id 0\n"));
    return;
  }

  const ImageMap_t::const_iterator it = m_imageMap.find(id);
  if (it != m_imageMap.end())
  {
    const ImageDataMap_t::const_iterator dataIt = m_imageDataMap.find(it->second.dataId);
    if (dataIt != m_imageDataMap.end())
    {
      const char *mimetype = nullptr;
      switch (dataIt->second.type)
      {
      case BBEB_IMAGE_TYPE_JPEG :
        mimetype = "image/jpeg";
        break;
      case BBEB_IMAGE_TYPE_PNG :
        mimetype = "image/png";
        break;
      case BBEB_IMAGE_TYPE_BMP :
        mimetype = "image/bmp";
        break;
      case BBEB_IMAGE_TYPE_GIF :
        mimetype = "image/gif";
        break;
      case BBEB_IMAGE_TYPE_UNKNOWN:
      default :
        EBOOK_DEBUG_MSG(("unknown image type %x\n", dataIt->second.type));
        return;
      }

      if ((0 != it->second.width) && (0 != it->second.height))
      {
        librevenge::RVNGPropertyList frameProps;
        insert(frameProps, "svg:width", it->second.width, m_dpi);
        insert(frameProps, "svg:height", it->second.height, m_dpi);
        frameProps.insert("style:horizontal-pos", "left");
        frameProps.insert("style:horizontal-rel", "paragraph-start-margin");
        frameProps.insert("style:vertical-pos", "top");
        frameProps.insert("style:vertical-rel", "paragraph-start-margin");
        frameProps.insert("style:wrap", "none");

        m_document->openFrame(frameProps);

        librevenge::RVNGPropertyList props;
        props.insert("librevenge:mime-type", mimetype);

        const RVNGInputStreamPtr_t &image = dataIt->second.image;
        image->seek(0, librevenge::RVNG_SEEK_END);
        const auto length = (unsigned long) image->tell();
        image->seek(0, librevenge::RVNG_SEEK_SET);
        const unsigned char *const bytes = readNBytes(image, length);
        librevenge::RVNGBinaryData data(bytes, length);
        props.insert("office:binary-data", data);

        m_document->insertBinaryObject(props);

        m_document->closeFrame();
      }
    }
    else
    {
      EBOOK_DEBUG_MSG(("image data with id %x not found\n", id));
    }
  }
  else
  {
    EBOOK_DEBUG_MSG(("image with id %x not found\n", id));
  }
}

void BBeBCollector::collectImageData(const unsigned id, const BBeBImageType type, const RVNGInputStreamPtr_t &image)
{
  ImageStreamData data;
  data.image = image;
  data.type = type;

  if (!m_imageDataMap.insert(ImageDataMap_t::value_type(id, data)).second)
  {
    EBOOK_DEBUG_MSG(("cannot insert image data %x: already present\n", id));
  }
}

void BBeBCollector::collectAttributes(const unsigned id, const BBeBAttributes &attributes, BBeBAttributeMap_t &attributeMap)
{
  if (!attributeMap.insert(BBeBAttributeMap_t::value_type(id, attributes)).second)
  {
    EBOOK_DEBUG_MSG(("cannot insert attributes for object %x: already present\n", id));
  }
}

void BBeBCollector::openBlock(unsigned atrID, const BBeBAttributes &attributes, const BBeBAttributeMap_t *attributeMap)
{
  BBeBAttributes mergedAttributes(m_currentAttributes.top());

  if ((0 != atrID) && attributeMap)
  {
    const BBeBAttributeMap_t::const_iterator it = attributeMap->find(atrID);
    if (it != attributeMap->end())
      merge(mergedAttributes, it->second);
    else
    {
      EBOOK_DEBUG_MSG(("attributes for object %x not found\n", atrID));
    }
  }

  merge(mergedAttributes, attributes);

  m_currentAttributes.push(mergedAttributes);
}

void BBeBCollector::setDPI(const double dpi)
{
  assert(0 > dpi);
  m_dpi = dpi;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
