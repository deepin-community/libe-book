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

#include <librevenge/librevenge.h>
#include <librevenge-stream/librevenge-stream.h>

#include "libebook_utils.h"
#include "EBOOKCharsetConverter.h"
#include "EBOOKMemoryStream.h"
#include "EBOOKUTF8Stream.h"
#include "EBOOKZlibStream.h"
#include "PDBLZ77Stream.h"
#include "PeanutPressParser.h"
#include "PeanutPressTypes.h"

using std::unique_ptr;

using librevenge::RVNGInputStream;

using std::string;
using std::vector;

namespace libebook
{

namespace
{

static const uint32_t PEANUTPRESS_TYPE = PDB_CODE("PNRd");
static const uint32_t PEANUTPRESS_CREATOR = PDB_CODE("PPrs");

enum Compression
{
  PEANUTPRESS_COMPRESSION_UNKNOWN,
  PEANUTPRESS_COMPRESSION_LZ77, // also known as PalmDoc
  PEANUTPRESS_COMPRESSION_LZ77_OBFUSCATED, // PalmDoc, xor-ed with 0xa5
  PEANUTPRESS_COMPRESSION_ZLIB,
  PEANUTPRESS_COMPRESSION_DRM
};

Compression readCompression(librevenge::RVNGInputStream *indexStream);

class XorStream : public librevenge::RVNGInputStream
{
  class StreamException
  {
  };

public:
  XorStream(librevenge::RVNGInputStream *stream, unsigned char xorValue);

  bool isStructured() override;
  unsigned subStreamCount() override;
  const char *subStreamName(unsigned id) override;
  bool existsSubStream(const char *name) override;
  librevenge::RVNGInputStream *getSubStreamByName(const char *name) override;
  RVNGInputStream *getSubStreamById(unsigned id) override;

  const unsigned char *read(unsigned long numBytes, unsigned long &numBytesRead) override;
  int seek(long offset, librevenge::RVNG_SEEK_TYPE seekType) override;
  long tell() override;
  bool isEnd() override;

private:
  std::unique_ptr<librevenge::RVNGInputStream> m_stream;
};

}

namespace
{

Compression readCompression(librevenge::RVNGInputStream *strm)
{
  const uint32_t compression = readU16(strm, true);
  switch (compression)
  {
  case 2 :
    return PEANUTPRESS_COMPRESSION_LZ77;
  case 4 :
    return PEANUTPRESS_COMPRESSION_LZ77_OBFUSCATED;
  case 10 :
    return PEANUTPRESS_COMPRESSION_ZLIB;
  case 260 :
  case 272 :
    return PEANUTPRESS_COMPRESSION_DRM;
  default :
    break;
  }

  return PEANUTPRESS_COMPRESSION_UNKNOWN;
}

XorStream::XorStream(librevenge::RVNGInputStream *strm, unsigned char xorValue)
  : m_stream()
{
  assert(strm);

  if (0 != strm->seek(0, librevenge::RVNG_SEEK_END))
    throw StreamException();
  const long end = strm->tell();
  if (0 != strm->seek(0, librevenge::RVNG_SEEK_SET))
    throw StreamException();
  if (0 >= end)
    throw StreamException();

  unsigned long numBytesRead = 0;
  const unsigned char *bytes = strm->read((unsigned long) end, numBytesRead);
  if (static_cast<unsigned long>(end) != numBytesRead)
    throw StreamException();

  vector<unsigned char> data;
  data.reserve(numBytesRead);
  for (unsigned long i = 0; i != numBytesRead; ++i)
    data.push_back(bytes[i] ^ xorValue);

  m_stream.reset(new EBOOKMemoryStream(&data[0], (unsigned) data.size()));
}

bool XorStream::isStructured()
{
  return m_stream->isStructured();
}

unsigned XorStream::subStreamCount()
{
  return 0;
}

const char *XorStream::subStreamName(unsigned)
{
  return nullptr;
}

bool XorStream::existsSubStream(const char *const name)
{
  return m_stream->existsSubStream(name);
}

librevenge::RVNGInputStream *XorStream::getSubStreamByName(const char *name)
{
  return m_stream->getSubStreamByName(name);
}

librevenge::RVNGInputStream *XorStream::getSubStreamById(unsigned)
{
  return nullptr;
}

const unsigned char *XorStream::read(unsigned long numBytes, unsigned long &numBytesRead)
{
  return m_stream->read(numBytes, numBytesRead);
}

int XorStream::seek(long offset, librevenge::RVNG_SEEK_TYPE seekType)
{
  return m_stream->seek(offset, seekType);
}

long XorStream::tell()
{
  return m_stream->tell();
}

bool XorStream::isEnd()
{
  return m_stream->isEnd();
}

void toggle(bool &value)
{
  value = !value;
}

librevenge::RVNGPropertyList makeParagraphProperties(const PeanutPressAttributes &attributes)
{
  librevenge::RVNGPropertyList props;

  if (attributes.right)
    props.insert("fo:text-align", "end");
  else if (attributes.center)
    props.insert("fo:text-align", "center");

  if (attributes.pageBreak)
    props.insert("fo:break-before", true);

  return props;
}

librevenge::RVNGPropertyList makeCharacterProperties(const PeanutPressAttributes &attributes)
{
  librevenge::RVNGPropertyList props;

  if (attributes.italic)
    props.insert("fo:font-style", "italic");

  if (attributes.underline)
    props.insert("style:text-underline-type", "single");

  if (attributes.bold || (PEANUTPRESS_FONT_TYPE_BOLD == attributes.font))
    props.insert("fo:font-weight", "bold");

  if (attributes.smallcaps)
    props.insert("fo:font-variant", "small-caps");

  return props;
}

}

namespace
{

/** Parser for the Palm Markup Language.
  */
class MarkupParser
{
  // -Weffc++
  MarkupParser(const MarkupParser &other);
  MarkupParser &operator=(const MarkupParser &other);

public:
  MarkupParser(librevenge::RVNGTextInterface *document, const PeanutPressParser::ImageMap_t &imageMap);
  ~MarkupParser();

  /** Parse an input stream.
    *
    * The function can be called more than once. In that case, the
    * parsing continues with the old state.
    *
    * @arg[in] input input stream
    */
  void parse(librevenge::RVNGInputStream *input);

private:
  void readCommand();
  void readImage();
  void readFootnote();
  void readSidebar();
  void readCp1252Code();
  void readUnicodeCode();

  string readCommandArgument();
  void skipCommandArgument();

  void insertParagraphBreak();
  void flushText();

private:
  librevenge::RVNGTextInterface *m_document;
  const PeanutPressParser::ImageMap_t &m_imageMap;

  librevenge::RVNGInputStream *m_input;

  PeanutPressAttributes m_attributes;

  string m_text;

  bool m_paragraphOpened;

  bool m_visible;

  bool m_stdFontOpened;
  bool m_boldFontOpened;
  bool m_largeFontOpened;
};

}

namespace
{

MarkupParser::MarkupParser(librevenge::RVNGTextInterface *const document, const PeanutPressParser::ImageMap_t &imageMap)
  : m_document(document)
  , m_imageMap(imageMap)
  , m_input(nullptr)
  , m_attributes()
  , m_text()
  , m_paragraphOpened(false)
  , m_visible(true)
  , m_stdFontOpened(false)
  , m_boldFontOpened(false)
  , m_largeFontOpened(false)
{
}

MarkupParser::~MarkupParser()
{
}

void MarkupParser::parse(librevenge::RVNGInputStream *const input)
{
  librevenge::RVNGInputStream *const savedInput = m_input;
  m_input = input;

  while (!m_input->isEnd())
  {
    const unsigned char c = readU8(input);

    switch (c)
    {
    case '\\' :
      readCommand();
      break;
    case '\n' :
      insertParagraphBreak();
      break;
    default :
      m_text.push_back((char) c);
      break;
    }
  }

  m_input = savedInput;
}

void MarkupParser::readCommand()
{
  const unsigned char c = readU8(m_input);

  switch (c)
  {
  case 'p' :
    m_attributes.pageBreak = true;
    break;
  case 'x' :
    flushText();
    m_attributes.pageBreak = true;
    toggle(m_attributes.chapter);
    break;
  case 'X' :
    skip(m_input, 1);
    flushText();
    toggle(m_attributes.chapter);
    break;
  case 'C' :
  {
    // has no visible effect
    const unsigned char cc = readU8(m_input);
    if ('n' != cc)
    {
      EBOOK_DEBUG_MSG(("unknown command C%c\n", cc));
    }
    skipCommandArgument();
    break;
  }
  case 'c' :
    toggle(m_attributes.center);
    break;
  case 'r' :
    toggle(m_attributes.right);
    break;
  case 'i' :
    flushText();
    toggle(m_attributes.italic);
    break;
  case 'u' :
    flushText();
    toggle(m_attributes.underline);
    break;
  case 'o' :
    flushText();
    toggle(m_attributes.overstrike);
    break;
  case 'v' :
    flushText();
    toggle(m_visible);
    break;
  case 't' :
    toggle(m_attributes.indent);
    break;
  case 'T' :
    // TODO: implement me
    skipCommandArgument();
    break;
  case 'w' :
    // no rules
    skipCommandArgument();
    break;
  case 'n' :
    flushText();
    m_attributes.font = PEANUTPRESS_FONT_TYPE_NORMAL;
    break;
  case 's' :
    flushText();
    toggle(m_stdFontOpened);
    if (m_stdFontOpened)
      m_attributes.font = PEANUTPRESS_FONT_TYPE_STD;
    else
      m_attributes.font = PEANUTPRESS_FONT_TYPE_NORMAL;
    break;
  case 'b' :
    flushText();
    toggle(m_boldFontOpened);
    if (m_boldFontOpened)
      m_attributes.font = PEANUTPRESS_FONT_TYPE_BOLD;
    else
      m_attributes.font = PEANUTPRESS_FONT_TYPE_NORMAL;
    break;
  case 'l' :
    flushText();
    toggle(m_largeFontOpened);
    if (m_largeFontOpened)
      m_attributes.font = PEANUTPRESS_FONT_TYPE_LARGE;
    else
      m_attributes.font = PEANUTPRESS_FONT_TYPE_NORMAL;
    break;
  case 'B' :
    flushText();
    toggle(m_attributes.bold);
    break;
  case 'S' :
  {
    const unsigned char cc = readU8(m_input);
    switch (cc)
    {
    case 'p' :
      flushText();
      toggle(m_attributes.superscript);
      break;
    case 'b' :
      flushText();
      toggle(m_attributes.subscript);
      break;
    case 'd' :
      readSidebar();
      break;
    default :
      EBOOK_DEBUG_MSG(("unknown command S%c\n", cc));
      break;
    }
    break;
  }
  case 'k' :
    flushText();
    toggle(m_attributes.smallcaps);
    break;
  case '\\' :
    m_text.push_back((char) c);
    break;
  case 'a' :
    readCp1252Code();
    break;
  case 'U' :
    readUnicodeCode();
    break;
  case 'm' :
    readImage();
    break;
  case 'q' :
  {
    const unsigned char cc = readU8(m_input);
    if ('=' == cc)
    {
      m_input->seek(-1, librevenge::RVNG_SEEK_CUR);
      skipCommandArgument();
    }
    break;
  }
  case 'Q' :
    // links are not implemented
    skipCommandArgument();
    break;
  case '-' :
    // TODO: implement me
    break;
  case 'F' :
  {
    const unsigned char cc = readU8(m_input);
    if ('n' != cc)
      EBOOK_DEBUG_MSG(("unknown command F%c\n", cc));
    else
      readFootnote();
    break;
  }
  case 'I' :
    // ignore
    break;
  default :
    EBOOK_DEBUG_MSG(("skipping unknown command %c\n", c));
    break;
  }
}

void MarkupParser::readImage()
{
  const std::string &name = readCommandArgument();

  const PeanutPressParser::ImageMap_t::const_iterator it = m_imageMap.find(name);
  if (m_imageMap.end() != it)
  {
    librevenge::RVNGPropertyList props;
    props.insert("librevenge:mime-type", "image/png");
    const librevenge::RVNGBinaryData image(&it->second[0], it->second.size());
    props.insert("office:binary-data", image);
    m_document->insertBinaryObject(props);
  }
}

void MarkupParser::readFootnote()
{
  // TODO: implement me
  skipCommandArgument();

  while (true)
  {
    const unsigned char c = readU8(m_input);
    if ('\\' == c)
    {
      const unsigned char cF = readU8(m_input);
      const unsigned char cn = readU8(m_input);
      if (('F' != cF) || ('n' != cn))
      {
        EBOOK_DEBUG_MSG(("expected end of footnote, found %c%c\n", cF, cn));
        throw GenericException();
      }
      break;
    }
    else
    {
      // ignore footnote text
    }
  }
}

void MarkupParser::readSidebar()
{
  // TODO: implement me
  skipCommandArgument();

  while (true)
  {
    const unsigned char c = readU8(m_input);
    if ('\\' == c)
    {
      const unsigned char cS = readU8(m_input);
      const unsigned char cd = readU8(m_input);
      if (('S' != cS) || ('d' != cd))
      {
        EBOOK_DEBUG_MSG(("expected end of sidebar, found %c%c\n", cS, cd));
        throw GenericException();
      }
      break;
    }
    else
    {
      m_text.push_back((char) c);
    }
  }
}

void MarkupParser::readCp1252Code()
{
  const unsigned char *s = readNBytes(m_input, 3);
  (void) s;
  // TODO: implement me
}

void MarkupParser::readUnicodeCode()
{
  const unsigned char *s = readNBytes(m_input, 4);
  (void) s;
  // TODO: implement me
}

string MarkupParser::readCommandArgument()
{
  string value;

  if ('=' == readU8(m_input))
  {
    if ('"' == readU8(m_input))
    {
      unsigned char c = 0;
      while ('"' != (c = readU8(m_input)))
        value.push_back((char) c);
    }
  }

  return value;
}

void MarkupParser::skipCommandArgument()
{
  if ('=' == readU8(m_input))
  {
    if ('"' == readU8(m_input))
    {
      while ('"' != readU8(m_input))
        ;
    }
  }
}

void MarkupParser::insertParagraphBreak()
{
  flushText();

  if (m_paragraphOpened)
  {
    m_document->closeParagraph();
    m_paragraphOpened = false;
  }
}

void MarkupParser::flushText()
{
  if (!m_visible)
  {
    m_text.clear();
    return;
  }

  if (m_text.empty())
    return;

  if (!m_paragraphOpened)
  {
    m_document->openParagraph(makeParagraphProperties(m_attributes));
    m_paragraphOpened = true;
    m_attributes.pageBreak = false;
  }

  m_document->openSpan(makeCharacterProperties(m_attributes));
  m_document->insertText(librevenge::RVNGString(m_text.c_str()));
  m_text.clear();
  m_document->closeSpan();
}

}

namespace
{

void parseEncodedText(MarkupParser *const parser, RVNGInputStream *input, EBOOKCharsetConverter *const converter)
{
  assert(parser);
  assert(input);

  unique_ptr<RVNGInputStream> utf8Input;

  if (converter)
  {
    utf8Input.reset(new EBOOKUTF8Stream(input, converter));
    input = utf8Input.get();
  }

  parser->parse(input);
}

}

struct PeanutPressHeader
{
  PeanutPressHeader();

  unsigned nonTextRecordStart;
  unsigned imageCount;
  unsigned footnoteCount;
  unsigned sidebarCount;
  unsigned imageDataRecordStart;
  unsigned metadataRecordStart;
  unsigned footnoteRecordStart;
  unsigned sidebarRecordStart;
  unsigned lastDataRecord;
  Compression compression;
  bool hasMetadata;
  bool is202Header;
};

struct PeanutPressTextParserState
{
  PeanutPressTextParserState();

};

PeanutPressHeader::PeanutPressHeader()
  : nonTextRecordStart(0)
  , imageCount(0)
  , footnoteCount(0)
  , sidebarCount(0)
  , imageDataRecordStart(0)
  , metadataRecordStart(0)
  , footnoteRecordStart(0)
  , sidebarRecordStart(0)
  , lastDataRecord(0)
  , compression(PEANUTPRESS_COMPRESSION_DRM)
  , hasMetadata(false)
  , is202Header(false)
{
}

PeanutPressParser::PeanutPressParser(librevenge::RVNGInputStream *const input, librevenge::RVNGTextInterface *const document)
  : PDBParser(input, document, PEANUTPRESS_TYPE, PEANUTPRESS_CREATOR)
  , m_header(new PeanutPressHeader())
  , m_imageMap()
{
  const unique_ptr<librevenge::RVNGInputStream> record(getIndexRecord());
  const Compression compression = readCompression(record.get());

  if (PEANUTPRESS_COMPRESSION_UNKNOWN == compression)
    throw UnsupportedFormat();
  if (PEANUTPRESS_COMPRESSION_DRM == compression)
    throw UnsupportedEncryption();
}

PeanutPressParser::~PeanutPressParser()
{
}

bool PeanutPressParser::checkType(unsigned type, unsigned creator)
{
  return (PEANUTPRESS_TYPE == type) && (PEANUTPRESS_CREATOR == creator);
}

void PeanutPressParser::readAppInfoRecord(librevenge::RVNGInputStream *)
{
}

void PeanutPressParser::readSortInfoRecord(librevenge::RVNGInputStream *)
{
}

void PeanutPressParser::readIndexRecord(librevenge::RVNGInputStream *record)
{
  record->seek(0, librevenge::RVNG_SEEK_END);
  const auto size = (unsigned long) record->tell();
  record->seek(0, librevenge::RVNG_SEEK_SET);

  if (132 == size) // header created by Dropbook
  {
    m_header->compression = readCompression(record);
    skip(record, 10);
    m_header->nonTextRecordStart = readU16(record, true);
    skip(record, 6);
    m_header->imageCount = readU16(record, true);
    skip(record, 2);
    m_header->hasMetadata = 1 == readU16(record, true);
    skip(record, 2);
    m_header->footnoteCount = readU16(record, true);
    m_header->sidebarCount = readU16(record, true);
    skip(record, 8);
    m_header->imageDataRecordStart = readU16(record, true);
    skip(record, 2);
    m_header->metadataRecordStart = readU16(record, true);
    skip(record, 2);
    m_header->footnoteRecordStart = readU16(record, true);
    m_header->sidebarRecordStart = readU16(record, true);
    m_header->lastDataRecord = readU16(record, true);
  }
  else if (202 == size) // header created by Makebook
  {
    m_header->is202Header = true;
    m_header->compression = PEANUTPRESS_COMPRESSION_LZ77_OBFUSCATED;
    skip(record, 8);
    m_header->nonTextRecordStart = readU16(record, true);
  }
}

void PeanutPressParser::readDataRecord(librevenge::RVNGInputStream *record, bool last)
{
  // TODO: implement me
  (void) record;
  (void) last;
}

void PeanutPressParser::readDataRecords()
{
  readImages();
  readText();
}

void PeanutPressParser::readText()
{
  openDocument();

  MarkupParser parser(getDocument(), m_imageMap);
  EBOOKCharsetConverter charsetConverter("cp1252");
  const unsigned lastTextRecord(m_header->nonTextRecordStart > 0 ? m_header->nonTextRecordStart - 1 : 0);

  switch (m_header->compression)
  {
  case PEANUTPRESS_COMPRESSION_LZ77 :
    // TODO(check): doesn't this miss a record?
    for (unsigned i = 1; i < lastTextRecord; ++i)
    {
      const unique_ptr<librevenge::RVNGInputStream> record(getDataRecord(i - 1));
      if (bool(record))
      {
        PDBLZ77Stream compressedInput(record.get());
        parseEncodedText(&parser, &compressedInput, &charsetConverter);
      }
      else
      {
        EBOOK_DEBUG_MSG(("could not get data record %u\n", i));
      }
    }
    break;
  case PEANUTPRESS_COMPRESSION_LZ77_OBFUSCATED :
    // TODO(check): doesn't this miss a record?
    for (unsigned i = 1; i < lastTextRecord; ++i)
    {
      const unique_ptr<librevenge::RVNGInputStream> record(getDataRecord(i - 1));
      if (bool(record))
      {
        XorStream unobfuscatedInput(record.get(), 0xa5);
        PDBLZ77Stream uncompressedInput(&unobfuscatedInput);
        parseEncodedText(&parser, &uncompressedInput, &charsetConverter);
      }
      else
      {
        EBOOK_DEBUG_MSG(("could not get data record %u\n", i));
      }
    }
    break;
  case PEANUTPRESS_COMPRESSION_ZLIB :
  {
    const unique_ptr<librevenge::RVNGInputStream> block(getDataRecords(0, lastTextRecord));
    if (bool(block))
    {
      EBOOKZlibStream uncompressedInput(block.get());
      parseEncodedText(&parser, &uncompressedInput, &charsetConverter);
    }
    else
    {
      EBOOK_DEBUG_MSG(("could not get data records\n"));
    }
    break;
  }
  case PEANUTPRESS_COMPRESSION_UNKNOWN:
  case PEANUTPRESS_COMPRESSION_DRM:
  default :
    throw GenericException();
  }

  closeDocument();
}

void PeanutPressParser::readImages()
{
  if (m_header->is202Header)
  {
    // detect images
    for (unsigned i = m_header->nonTextRecordStart; i < getDataRecordCount(); ++i)
    {
      const unique_ptr<librevenge::RVNGInputStream> record(getDataRecord(i));

      unsigned char p = readU8(record.get());
      unsigned char n = readU8(record.get());
      unsigned char g = readU8(record.get());
      unsigned char s = readU8(record.get());
      if (('P' == p) && ('N' == n) && ('G' == g) && (' ' == s))
        readImage(record.get(), true);
    }
  }
  else
  {
    for (unsigned i = 0; i != m_header->imageCount; ++i)
    {
      const unique_ptr<librevenge::RVNGInputStream> record(getDataRecord(m_header->imageDataRecordStart + i - 1));
      readImage(record.get());
    }
  }
}

void PeanutPressParser::readImage(librevenge::RVNGInputStream *const record, const bool verified)
{
  if (!verified)
    skip(record, 4); // "PNG "

  const unsigned maxNameLen = 32;
  const unsigned char *const nameBytes = readNBytes(record, maxNameLen);
  const unsigned char *const nameBytesEnd = nameBytes + maxNameLen;

  std::string name;
  if (std::find(nameBytes, nameBytesEnd, 0) == nameBytesEnd)
    name.assign(reinterpret_cast<const char *>(nameBytes), reinterpret_cast<const char *>(nameBytesEnd - 1));
  else
    name = reinterpret_cast<const char *>(nameBytes);

  skip(record, 62 - 36);

  vector<unsigned char> data;
  while (!record->isEnd())
    data.push_back(readU8(record));

  m_imageMap.insert(ImageMap_t::value_type(name, data));
}

void PeanutPressParser::openDocument()
{
  getDocument()->startDocument(librevenge::RVNGPropertyList());
  getDocument()->openPageSpan(getDefaultPageSpanPropList());
}

void PeanutPressParser::closeDocument()
{
  getDocument()->closePageSpan();
  getDocument()->endDocument();
}

} // namespace libebook

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
