/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <algorithm>
#include <cassert>
#include <cstring>
#include <deque>
#include <utility>

#include <boost/fusion/include/std_pair.hpp>
#include <boost/optional.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/variant.hpp>

#include <librevenge/librevenge.h>

#include "libebook_utils.h"
#include "EBOOKCharsetConverter.h"
#include "EBOOKMemoryStream.h"
#include "EBOOKUTF8Stream.h"
#include "TealDocParser.h"
#include "PDBLZ77Stream.h"

using boost::optional;

using std::deque;
using std::string;
using std::vector;

namespace libebook
{

namespace
{

static const unsigned TEALDOC_BLOCK_SIZE = 4096;

static const unsigned TEALDOC_TYPE = PDB_CODE("TEXt");
static const unsigned TEALDOC_CREATOR = PDB_CODE("TlDc");

}

namespace
{

enum Font
{
  FONT_NORMAL,
  FONT_BOLD,
  FONT_LARGE
};

enum Style
{
  STYLE_NORMAL,
  STYLE_UNDERLINE,
  STYLE_INVERT
};

enum Align
{
  ALIGN_LEFT,
  ALIGN_RIGHT,
  ALIGN_CENTER
};

enum Token
{
  TOKEN_UNKNOWN,
  // tags
  TOKEN_BOOKMARK,
  TOKEN_HEADER,
  TOKEN_HRULE,
  TOKEN_LABEL,
  TOKEN_LINK,
  TOKEN_TEALPAINT,
  // attributes
  TOKEN_ALIGN,
  TOKEN_FONT,
  TOKEN_STYLE,
  TOKEN_TEXT,
  // values
  TOKEN_0,
  TOKEN_1,
  TOKEN_2,
  TOKEN_CENTER,
  TOKEN_INVERT,
  TOKEN_LEFT,
  TOKEN_NORMAL,
  TOKEN_RIGHT,
  TOKEN_UNDERLINE
};

struct TealDocAttributes
{
  TealDocAttributes();

  optional<Font> font;
  optional<Style> style;
  optional<Align> align;
};

TealDocAttributes::TealDocAttributes()
  : font()
  , style()
  , align()
{
}

}

namespace
{

namespace qi = boost::spirit::qi;

typedef boost::variant<int, string> AttrValue_t;
typedef deque<std::pair<int, AttrValue_t>> Attributes_t;

template<typename Iterator>
struct TagGrammar : public qi::grammar<Iterator, std::pair<int, Attributes_t>, qi::space_type>
{
  TagGrammar() : TagGrammar::base_type(tag, "tag")
  {
    using qi::alnum;
    using qi::alpha;
    using qi::attr;
    using qi::char_;
    using qi::lexeme;
    using qi::no_case;
    using qi::omit;
    using qi::space;

    tag %= '<' >> (no_case[tags] | unknown) >> *attrib >> '>';

    attrib %=
      no_case[eattrs] >> '=' >> (values | text) // accept unknown values too
      | no_case[sattrs] >> '=' >> text
      | unknown >> '=' >> text
      ;

    value %=
      '\'' >> values >> '\''
      | '\"' >> values >> '\"'
      | values
      ;

    text %=
      '\'' >> lexeme[+(char_ - '\'')] >> '\''
      | '\"' >> lexeme[+(char_ - '\"')] >> '\"'
      | lexeme[+(char_ - space)]
      ;

    unknown %= omit[alpha >> *alnum] >> attr(TOKEN_UNKNOWN);

    tag.name("tag");
    attrib.name("attrib");
    value.name("value");
    text.name("text");
    unknown.name("unknown");

    tags.name("tags");
    eattrs.name("eattrs");
    sattrs.name("sattrs");
    values.name("values");
  }

  struct tags_ : qi::symbols<char, int>
  {
    tags_()
    {
      add
      ("bookmark", TOKEN_BOOKMARK)
      ("header", TOKEN_HEADER)
      ("hrule", TOKEN_HRULE)
      ("label", TOKEN_LABEL)
      ("link", TOKEN_LINK)
      ("tealpaint", TOKEN_TEALPAINT)
      ;
    }
  } tags;

  struct eattrs_ : qi::symbols<char, int>
  {
    eattrs_()
    {
      add
      ("align", TOKEN_ALIGN)
      ("font", TOKEN_FONT)
      ("style", TOKEN_STYLE)
      ;
    }
  } eattrs;

  struct sattrs_ : qi::symbols<char, int>
  {
    sattrs_()
    {
      add
      ("text", TOKEN_TEXT)
      ;
    }
  } sattrs;

  struct values_ : qi::symbols<char, int>
  {
    values_()
    {
      add
      ("0", TOKEN_0)
      ("1", TOKEN_1)
      ("2", TOKEN_2)
      ("center", TOKEN_CENTER)
      ("invert", TOKEN_INVERT)
      ("left", TOKEN_LEFT)
      ("normal", TOKEN_NORMAL)
      ("right", TOKEN_RIGHT)
      ("underline", TOKEN_UNDERLINE)
      ;
    }
  } values;

  qi::rule<Iterator, std::pair<int, Attributes_t>, qi::space_type> tag;
  qi::rule<Iterator, std::pair<int, AttrValue_t>, qi::space_type> attrib;
  qi::rule<Iterator, AttrValue_t, qi::space_type> value;
  qi::rule<Iterator, AttrValue_t, qi::space_type> text;
  qi::rule<Iterator, int, qi::space_type> unknown;
};

}

namespace
{

class AttributeHandler : public boost::static_visitor<>
{
public:
  AttributeHandler(const int tokenId, TealDocAttributes &attributes, string &text)
    : m_attr(tokenId)
    , m_attributes(attributes)
    , m_text(text)
  {
  }

  void operator()(const int value) const
  {
    switch (m_attr)
    {
    case TOKEN_ALIGN :
    {
      switch (value)
      {
      case TOKEN_CENTER :
        m_attributes.align = ALIGN_CENTER;
        break;
      case TOKEN_LEFT :
        m_attributes.align = ALIGN_LEFT;
        break;
      case TOKEN_RIGHT :
        m_attributes.align = ALIGN_RIGHT;
        break;
      default :
        EBOOK_DEBUG_MSG(("unknown alignment %d\n", value));
      }
      break;
    }
    case TOKEN_FONT :
    {
      switch (value)
      {
      case TOKEN_0 :
        m_attributes.font = FONT_NORMAL;
        break;
      case TOKEN_1 :
        m_attributes.font = FONT_BOLD;
        break;
      case TOKEN_2 :
        m_attributes.font = FONT_LARGE;
        break;
      default :
        EBOOK_DEBUG_MSG(("unknown font type %d\n", value));
      }
      break;
    }
    case TOKEN_STYLE :
    {
      switch (value)
      {
      case TOKEN_INVERT :
        m_attributes.style = STYLE_INVERT;
        break;
      case TOKEN_NORMAL :
        m_attributes.style = STYLE_NORMAL;
        break;
      case TOKEN_UNDERLINE :
        m_attributes.style = STYLE_UNDERLINE;
        break;
      default :
        EBOOK_DEBUG_MSG(("unknown style %d\n", value));
      }
      break;
    }
    default :
      break;
    }
  }

  void operator()(const std::string &value) const
  {
    if (m_attr == TOKEN_TEXT)
      m_text = value;
  }

private:
  const int m_attr;
  TealDocAttributes &m_attributes;
  string &m_text;
};

}

class TealDocTextParser
{
  // -Weffc++
  TealDocTextParser(const TealDocTextParser &other);
  TealDocTextParser operator=(const TealDocTextParser &other);

public:
  explicit TealDocTextParser(librevenge::RVNGTextInterface *document);

  void parse(librevenge::RVNGInputStream *input, bool last = false);

private:
  bool parseTag(librevenge::RVNGInputStream *input);

  bool parseHeaderTag(const Attributes_t &attributeList);

  void openParagraph(const TealDocAttributes &attributes = TealDocAttributes());
  void closeParagraph();

  void finishParagraph();
  void flushText(const TealDocAttributes &attributes = TealDocAttributes());

private:
  librevenge::RVNGTextInterface *const m_document;

  string m_text;

  bool m_openedParagraph;
};

TealDocTextParser::TealDocTextParser(librevenge::RVNGTextInterface *const document)
  : m_document(document)
  , m_text()
  , m_openedParagraph(false)
{
}

void TealDocTextParser::parse(librevenge::RVNGInputStream *const input, const bool last)
{
  while (!input->isEnd())
  {
    const unsigned char c = readU8(input);

    switch (c)
    {
    case '\n' :
      finishParagraph();
      break;
    case '<' :
      if (!parseTag(input))
        m_text.push_back('<');
      break;
    default :
      m_text.push_back((char) c);
    }
  }

  if (last)
    finishParagraph();
}

bool TealDocTextParser::parseTag(librevenge::RVNGInputStream *const input)
{
  const auto pos = (unsigned long) input->tell();
  string tag("<");

  // read tag into string
  // I suppose it would be possible to create an iterator adaptor for
  // librevenge::RVNGInputStream, but this is much simpler
  unsigned char c = 0;
  do
  {
    c = readU8(input);
    tag.push_back((char) c);
  }
  while ('>' != c);

  // parse tag
  std::pair<int, Attributes_t> parsedTag;

  auto it = tag.cbegin();
  const bool match = qi::phrase_parse(
                       it, tag.cend(),
                       TagGrammar<string::const_iterator>(), qi::space,
                       parsedTag
                     );
  const bool success = match && it == tag.end();

  // process tag
  if (success)
  {
    if (parsedTag.first == TOKEN_HEADER)
    {
      finishParagraph();
      parseHeaderTag(parsedTag.second);
    }
    /* TODO: handle TOKEN_TEALPAINT
       ok to ignore: TOKEN_BOOKMARK, TOKEN_HRULE, TOKEN_LABEL, TOKEN_LINK
       unknown 10 other enumerataions
    */
  }
  else
    input->seek((long) pos, librevenge::RVNG_SEEK_SET);

  return success;
}

bool TealDocTextParser::parseHeaderTag(const Attributes_t &attributeList)
{
  TealDocAttributes attributes;

  for (const auto &it : attributeList)
  {
    if (it.first != TOKEN_UNKNOWN)
      boost::apply_visitor(AttributeHandler(it.first, attributes, m_text), it.second);
  }

  openParagraph(attributes);
  flushText(attributes);
  closeParagraph();

  return true;
}

void TealDocTextParser::openParagraph(const TealDocAttributes &attributes)
{
  librevenge::RVNGPropertyList props;

  if (attributes.align)
  {
    switch (get(attributes.align))
    {
    case ALIGN_LEFT :
      props.insert("fo:text-align", "left");
      break;
    case ALIGN_RIGHT :
      props.insert("fo:text-align", "end");
      break;
    case ALIGN_CENTER :
      props.insert("fo:text-align", "center");
      break;
    default :
      break;
    }
  }

  m_document->openParagraph(props);
  m_openedParagraph = true;
}

void TealDocTextParser::closeParagraph()
{
  if (m_openedParagraph)
    m_document->closeParagraph();
  m_openedParagraph = false;
}

void TealDocTextParser::finishParagraph()
{
  flushText();
  closeParagraph();
}

void TealDocTextParser::flushText(const TealDocAttributes &attributes)
{
  if (!m_openedParagraph)
    openParagraph(attributes);

  if (!m_text.empty())
  {
    librevenge::RVNGPropertyList props;

    if (attributes.font)
    {
      switch (get(attributes.font))
      {
      case FONT_BOLD :
        props.insert("fo:font-weight", "bold");
        break;
      case FONT_LARGE :
        // TODO: handle
        break;
      case FONT_NORMAL :
      // fall through
      default :
        break;
      }
    }

    if (attributes.style)
    {
      switch (get(attributes.style))
      {
      case STYLE_UNDERLINE :
        props.insert("style:text-underline-type", "single");
        break;
      case STYLE_INVERT :
        props.insert("fo:color", "#FFFFFF");
        props.insert("fo:background-color", "#000000");
        break;
      case STYLE_NORMAL :
      // fall through
      default :
        break;
      }
    }

    m_document->openSpan(props);
    m_document->insertText(librevenge::RVNGString(m_text.c_str()));
    m_document->closeSpan();

    m_text.clear();
  }
}

TealDocParser::TealDocParser(librevenge::RVNGInputStream *input, librevenge::RVNGTextInterface *document)
  : PDBParser(input, document, TEALDOC_TYPE, TEALDOC_CREATOR)
  , m_compressed(false)
  , m_textLength(0)
  , m_recordCount(0)
  , m_recordSize(0)
  , m_read(0)
  , m_openedDocument(false)
  , m_converter()
  , m_textParser(new TealDocTextParser(document))
{
}

TealDocParser::~TealDocParser()
{
}

bool TealDocParser::checkType(const unsigned type, const unsigned creator)
{
  return TEALDOC_TYPE == type && TEALDOC_CREATOR == creator;
}

void TealDocParser::readAppInfoRecord(librevenge::RVNGInputStream *)
{
  // there is no appInfo in TealDoc
}

void TealDocParser::readSortInfoRecord(librevenge::RVNGInputStream *)
{
  // there is no sortInfo in TealDoc
}

void TealDocParser::readIndexRecord(librevenge::RVNGInputStream *const input)
{
  const uint16_t compression = readU16(input, true);
  assert(1 == compression || 2 == compression);
  m_compressed = 2 == compression;
  skip(input, 2);
  m_textLength = readU32(input, true);
  m_recordCount = readU16(input, true);
  m_recordSize = readU16(input, true);

  // check consistency
  assert(m_recordCount == getDataRecordCount());
  assert(TEALDOC_BLOCK_SIZE == m_recordSize);
}

void TealDocParser::readDataRecord(librevenge::RVNGInputStream *input, const bool last)
{
  vector<char> uncompressed;
  uncompressed.reserve(m_recordSize);

  std::unique_ptr<librevenge::RVNGInputStream> compressedInput;

  // This should not happen, but it is the easier case anyway :-)
  if (m_compressed)
  {
    compressedInput.reset(new PDBLZ77Stream(input));
    input = compressedInput.get();
  }

  const long origPos = input->tell();
  while (!input->isEnd())
    uncompressed.push_back((char) readU8(input));
  m_read += unsigned(input->tell() - origPos);

  assert(m_read <= m_textLength);
  if (last)
    assert(m_read == m_textLength);

  if (!m_openedDocument)
  {
    createConverter(uncompressed);
    openDocument();
  }

  EBOOKMemoryStream uncompressedStrm(reinterpret_cast<unsigned char *>(&uncompressed[0]), (unsigned) uncompressed.size());
  EBOOKUTF8Stream utf8Strm(&uncompressedStrm);

  m_textParser->parse(&utf8Strm, last);

  if (last)
    closeDocument();
}

void TealDocParser::createConverter(const std::vector<char> &text)
{
  if (text.empty())
  {
    m_converter.reset(new EBOOKCharsetConverter("cp1252")); // try a default encoding
  }
  else
  {
    std::unique_ptr<EBOOKCharsetConverter> converter(new EBOOKCharsetConverter());
    if (converter->guessEncoding(&text[0], (unsigned) text.size()))
      m_converter = std::move(converter);
    else
      throw GenericException();
  }
}

void TealDocParser::openDocument()
{
  if (m_openedDocument)
    return;

  getDocument()->startDocument(librevenge::RVNGPropertyList());

  librevenge::RVNGPropertyList metadata;
  if (*getName())
  {
    vector<char> nameUtf8;
    if (m_converter->convertBytes(getName(), (unsigned int)std::strlen(getName()), nameUtf8) && !nameUtf8.empty())
    {
      nameUtf8.push_back(0);
      metadata.insert("dc:title", librevenge::RVNGString(&nameUtf8[0]));
    }
  }

  getDocument()->setDocumentMetaData(metadata);
  getDocument()->openPageSpan(getDefaultPageSpanPropList());

  m_openedDocument = true;
}

void TealDocParser::closeDocument()
{
  getDocument()->closePageSpan();
  getDocument()->endDocument();
  m_openedDocument = false;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
