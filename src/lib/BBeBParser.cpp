/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <algorithm>

#include <librevenge-stream/librevenge-stream.h>

#include "libebook_utils.h"
#include "EBOOKCharsetConverter.h"
#include "EBOOKMemoryStream.h"
#include "EBOOKStreamView.h"
#include "EBOOKUTF8Stream.h"
#include "EBOOKZlibStream.h"
#include "BBeBMetadataParser.h"
#include "BBeBParser.h"

using std::string;

namespace libebook
{

namespace
{

enum Tag
{
  TAG_OBJECT_START = 0xf500,
  TAG_OBJECT_END = 0xf501,
  TAG_OBJECT_INFO_LINK = 0xf502,
  TAG_LINK = 0xf503,
  TAG_STREAM_SIZE = 0xf504,
  TAG_STREAM_START = 0xf505,
  TAG_STREAM_END = 0xf506,
  TAG_F507 = 0xf507,
  TAG_F508 = 0xf508,
  TAG_F509 = 0xf509,
  TAG_F50A = 0xf50a,
  TAG_CONTAINED_OBJECTS_LIST = 0xf50b,
  TAG_F50D = 0xf50d,
  TAG_F50E = 0xf50e,
  TAG_FONT_SIZE = 0xf511,
  TAG_FONT_WIDTH = 0xf512,
  TAG_FONT_ESCAPEMENT = 0xf513,
  TAG_FONT_ORIENTATION = 0xf514,
  TAG_FONT_WEIGHT = 0xf515,
  TAG_FONT_FACENAME = 0xf516,
  TAG_TEXT_COLOR = 0xf517,
  TAG_TEXT_BG_COLOR = 0xf518,
  TAG_WORD_SPACE = 0xf519,
  TAG_LETTER_SPACE = 0xf51a,
  TAG_BASE_LINE_SKIP = 0xf51b,
  TAG_LINE_SPACE = 0xf51c,
  TAG_PAR_INDENT = 0xf51d,
  TAG_PAR_SKIP = 0xf51e,
  TAG_TOP_MARGIN = 0xf521,
  TAG_F522 = 0xf522,
  TAG_F523 = 0xf523,
  TAG_ODD_SIDE_MARGIN = 0xf524,
  TAG_PAGE_HEIGHT = 0xf525,
  TAG_PAGE_WIDTH = 0xf526,
  TAG_F527 = 0xf527,
  TAG_F528 = 0xf528,
  TAG_F529 = 0xf529,
  TAG_F52A = 0xf52a,
  TAG_F52B = 0xf52b,
  TAG_EVEN_SIDE_MARGIN = 0xf52c,
  TAG_F52D = 0xf52d,
  TAG_F52E = 0xf52e,
  TAG_BLOCK_WIDTH = 0xf531,
  TAG_BLOCK_HEIGHT = 0xf532,
  TAG_BLOCK_RULE = 0xf533,
  TAG_F534 = 0xf534,
  TAG_F535 = 0xf535,
  TAG_F536 = 0xf536,
  TAG_F537 = 0xf537,
  TAG_TOP_SKIP = 0xf538,
  TAG_F539 = 0xf539,
  TAG_F53A = 0xf53a,
  TAG_ALIGN = 0xf53c,
  TAG_F53D = 0xf53d,
  TAG_F53E = 0xf53e,
  TAG_MINI_PAGE_HEIGHT = 0xf541,
  TAG_MINI_PAGE_WIDTH = 0xf542,
  TAG_F544 = 0xf544,
  TAG_F545 = 0xf545,
  TAG_LOCATION_Y = 0xf546,
  TAG_LOCATION_X = 0xf547,
  TAG_F548 = 0xf548,
  TAG_PUT_SOUND = 0xf549,
  TAG_IMAGE_CROP_RECT = 0xf54a,
  TAG_IMAGE_SIZE = 0xf54b,
  TAG_IMAGE_STREAM = 0xf54c,
  TAG_F54D = 0xf54d,
  TAG_F54E = 0xf54e,
  TAG_CANVAS_WIDTH = 0xf551,
  TAG_CANVAS_HEIGHT = 0xf552,
  TAG_F553 = 0xf553,
  TAG_STREAM_FLAGS = 0xf554,
  TAG_F555 = 0xf555,
  TAG_F556 = 0xf556,
  TAG_F557 = 0xf557,
  TAG_F558 = 0xf558,
  TAG_FONT_FILE_NAME = 0xf559,
  TAG_F55A = 0xf55a,
  TAG_VIEW_POINT = 0xf55b,
  TAG_PAGE_LIST = 0xf55c,
  TAG_FONT_FACE_NAME = 0xf55d,
  TAG_F55E = 0xf55e,
  TAG_F561 = 0xf561,
  TAG_F562 = 0xf562,
  TAG_F563 = 0xf563,
  TAG_F564 = 0xf564,
  TAG_F565 = 0xf565,
  TAG_F566 = 0xf566,
  TAG_F567 = 0xf567,
  TAG_F568 = 0xf568,
  TAG_F569 = 0xf569,
  TAG_F56A = 0xf56a,
  TAG_F56B = 0xf56b,
  TAG_JUMP_TO = 0xf56c,
  TAG_F56D = 0xf56d,
  TAG_F56E = 0xf56e,
  TAG_F571 = 0xf571,
  TAG_F572 = 0xf572,
  TAG_RULED_LINE = 0xf573,
  TAG_RUBY_ALIGN = 0xf575,
  TAG_RUBY_OVERHANG = 0xf576,
  TAG_EMPTY_DOTS_POSITION = 0xf577,
  TAG_EMPTY_DOTS_CODE = 0xf578,
  TAG_EMPTY_LINE_POSITION = 0xf579,
  TAG_EMPTY_LINE_MODE = 0xf57a,
  TAG_CHILD_PAGE_TREE = 0xf57b,
  TAG_PARENT_PAGE_TREE = 0xf57c,
  TAG_BEGIN_ITALIC = 0xf581,
  TAG_END_ITALIC = 0xf582,
  TAG_BEGIN_P = 0xf5a1,
  TAG_END_P = 0xf5a2,
  TAG_KOMA_GAIJI = 0xf5a5,
  TAG_KOMA_EMP_DOT_CHAR = 0xf5a6,
  TAG_BEGIN_BUTTON = 0xf5a7,
  TAG_END_BUTTON = 0xf5a8,
  TAG_BEGIN_RUBY = 0xf5a9,
  TAG_END_RUBY = 0xf5aa,
  TAG_BEGIN_RUBY_BASE = 0xf5ab,
  TAG_END_RUBY_BASE = 0xf5ac,
  TAG_BEGIN_RUBY_TEXT = 0xf5ad,
  TAG_END_RUBY_TEXT = 0xf5ae,
  TAG_KOMA_YOKOMOJI = 0xf5b1,
  TAG_F5B2 = 0xf5b2,
  TAG_BEGIN_TATE = 0xf5b3,
  TAG_END_TATE = 0xf5b4,
  TAG_BEGIN_NEKASE = 0xf5b5,
  TAG_END_NEKASE = 0xf5b6,
  TAG_BEGIN_SUP = 0xf5b7,
  TAG_END_SUP = 0xf5b8,
  TAG_BEGIN_SUB = 0xf5b9,
  TAG_END_SUB = 0xf5ba,
  TAG_F5BB = 0xf5bb,
  TAG_F5BC = 0xf5bc,
  TAG_F5BD = 0xf5bd,
  TAG_F5BE = 0xf5be,
  TAG_BEGIN_EMP_LINE = 0xf5c1,
  TAG_END_EMP_LINE = 0xf5c2,
  TAG_BEGIN_DRAW_CHAR = 0xf5c3,
  TAG_END_DRAW_CHAR = 0xf5c4,
  TAG_F5C5 = 0xf5c5,
  TAG_F5C6 = 0xf5c6,
  TAG_F5C7 = 0xf5c7,
  TAG_KOMA_AUTO_SPACING = 0xf5c8,
  TAG_F5C9 = 0xf5c9,
  TAG_SPACE = 0xf5ca,
  TAG_F5CB = 0xf5cb,
  TAG_TEXT_SIZE = 0xf5cc,
  TAG_KOMA_PLOT = 0xf5d1,
  TAG_EOL = 0xf5d2,
  TAG_WAIT = 0xf5d4,
  TAG_SOUND_STOP = 0xf5d6,
  TAG_MOVE_OBJ = 0xf5d7,
  TAG_BOOK_FONT = 0xf5d8,
  TAG_KOMA_PLOT_TEXT = 0xf5d9,
  TAG_F5DA = 0xf5da,
  TAG_F5DB = 0xf5db,
  TAG_F5DC = 0xf5dc,
  TAG_CHAR_SPACE = 0xf5dd,
  TAG_LINE_WIDTH = 0xf5f1,
  TAG_LINE_COLOR = 0xf5f2,
  TAG_FILL_COLOR = 0xf5f3,
  TAG_LINE_MODE = 0xf5f4,
  TAG_MOVE_TO = 0xf5f5,
  TAG_LINE_TO = 0xf5f6,
  TAG_DRAW_BOX = 0xf5f7,
  TAG_DRAW_ELLIPSE = 0xf5f8,
  TAG_F5F9 = 0xf5f9
};

// NOTE: This is just guessing. I have no idea if the stream flags are
// actually binary flags, or just a set of possible values.
enum StreamType
{
  STREAM_TYPE_BBEB_TAGS = 0,
  STREAM_TYPE_JPEG = BBEB_IMAGE_TYPE_JPEG,
  STREAM_TYPE_PNG = BBEB_IMAGE_TYPE_PNG,
  STREAM_TYPE_BMP = BBEB_IMAGE_TYPE_BMP,
  STREAM_TYPE_GIF = BBEB_IMAGE_TYPE_GIF,
  STREAM_TYPE_BBEB_TOC = 0x51,
  STREAM_TYPE_BBEB_TAGS_COMPRESSED = 0x100
};

class ParserException
{
};

const std::string readString(librevenge::RVNGInputStream *const input)
{
  const unsigned size = readU16(input);

  EBOOKStreamView textStrm(input, input->tell(), input->tell() + int(size));
  EBOOKCharsetConverter utf16("UTF-16LE");
  EBOOKUTF8Stream utf8Strm(&textStrm, &utf16);

  std::string text;
  while (!utf8Strm.isEnd())
    text.push_back((char) readU8(&utf8Strm));

  return text;
}

}

struct BBeBHeader
{
  BBeBHeader();

  unsigned version;
  uint16_t key;
  unsigned rootOID;
  uint64_t numberOfObjects;
  uint64_t objectIndexOffset;
  unsigned xmlCompSize;
  BBeBImageType thumbnailType;
  unsigned thumbnailSize;
  unsigned dpi;
  unsigned width;
  unsigned tocOID;
};

BBeBHeader::BBeBHeader()
  : version(0)
  , key(0)
  , rootOID(0)
  , numberOfObjects(0)
  , objectIndexOffset(0)
  , xmlCompSize(0)
  , thumbnailType(BBEB_IMAGE_TYPE_UNKNOWN)
  , thumbnailSize(0)
  , dpi(0)
  , width(0)
  , tocOID(0)
{
}

BBeBParser::BBeBParser(librevenge::RVNGInputStream *const input, librevenge::RVNGTextInterface *const document)
  : m_collector(document)
  , m_input(input)
  , m_header(new BBeBHeader())
  , m_objectIndex()
  , m_pageTree(0)
  , m_toc()
{
}

BBeBParser::~BBeBParser()
{
}

bool BBeBParser::parse()
{
  readHeader();
  readMetadata();
  readThumbnail();
  readObjectIndex();

  if (0 != m_header->tocOID)
    readObject(m_header->tocOID, OBJECT_TYPE_TOC);

  m_collector.startDocument();
  readObject(m_header->rootOID, OBJECT_TYPE_BOOK_ATR);
  m_collector.endDocument();

  return false;
}

bool BBeBParser::isSupported(librevenge::RVNGInputStream *const input)
{
  const unsigned char signature[] = { 'L', '\0', 'R', '\0', 'F', '\0' };
  unsigned long readBytes = 0;
  const unsigned char *s = input->read(sizeof(signature), readBytes);
  return (sizeof(signature) == readBytes) && std::equal(signature, signature + sizeof(signature), s);
}

void BBeBParser::readHeader()
{
  skip(m_input, 8);
  m_header->version = readU16(m_input);
  m_header->key = readU16(m_input);
  m_header->rootOID = readU32(m_input);
  m_header->numberOfObjects = readU64(m_input);
  m_header->objectIndexOffset = readU64(m_input);
  skip(m_input, 6);
  m_header->dpi = readU16(m_input);
  if (0 == m_header->dpi)
  {
    EBOOK_DEBUG_MSG(("DPI is 0, likely a broken file\n"));
    m_header->dpi = 1660;
  }
  skip(m_input, 2);
  m_header->width = readU16(m_input);
  skip(m_input, 24);
  m_header->tocOID = readU32(m_input);
  skip(m_input, 4);
  m_header->xmlCompSize = readU16(m_input);

  if (800 <= m_header->version)
  {
    const unsigned thumbnailType = readU16(m_input);
    switch (thumbnailType)
    {
    case BBEB_IMAGE_TYPE_JPEG :
    case BBEB_IMAGE_TYPE_PNG :
    case BBEB_IMAGE_TYPE_BMP :
    case BBEB_IMAGE_TYPE_GIF :
      m_header->thumbnailType = static_cast<BBeBImageType>(thumbnailType);
      break;
    default :
      EBOOK_DEBUG_MSG(("unknown thumbnail type %x\n", thumbnailType));
      break;
    }
    m_header->thumbnailSize = readU32(m_input);
  }
}

void BBeBParser::readMetadata()
{
  skip(m_input, 4); // uncompressed size?
  unsigned const char *const data = readNBytes(m_input, m_header->xmlCompSize);
  EBOOKMemoryStream memoryStrm(data, m_header->xmlCompSize);
  EBOOKZlibStream zlibStrm(&memoryStrm);

  BBeBMetadataParser parser(&zlibStrm);
  parser.parse();

  m_collector.collectMetadata(parser.getMetadata());
}

void BBeBParser::readThumbnail()
{
  // TODO: implement me
}

void BBeBParser::readObjectIndex()
{
  m_input->seek((long) m_header->objectIndexOffset, librevenge::RVNG_SEEK_SET);
  for (uint64_t i = m_header->numberOfObjects; 0 != i; --i)
  {
    unsigned id = readU32(m_input);
    BBeBIndexEntry entry;
    entry.offset = readU32(m_input);
    entry.size = readU32(m_input);
    entry.read = false;
    entry.reading = false;
    m_objectIndex.insert(ObjectIndex_t::value_type(id, entry));
    skip(m_input, 4);
  }
}

void BBeBParser::readObject(const unsigned id, const unsigned type)
{
  auto it = m_objectIndex.find(id);
  if (it == m_objectIndex.end())
  {
    EBOOK_DEBUG_MSG(("object with ID %x not found\n", id));
    throw ParserException();
  }

  BBeBIndexEntry &entry = it->second;
  if (entry.reading)
  {
    EBOOK_DEBUG_MSG(("object %x is already being read\n", id));
    throw ParserException();
  }
  if (entry.read)
  {
    EBOOK_DEBUG_MSG(("object %x has already been read\n", id));
  }

  m_input->seek((long) entry.offset, librevenge::RVNG_SEEK_SET);

  const unsigned startTag = readU16(m_input);
  if (TAG_OBJECT_START != startTag)
  {
    EBOOK_DEBUG_MSG(("reading object %x, but there is no object at the position\n", id));
    throw ParserException();
  }

  const unsigned objectID = readU32(m_input);
  if (id != objectID)
  {
    EBOOK_DEBUG_MSG(("expected object with ID %x, but found %x\n", id, objectID));
    throw ParserException();
  }

  const unsigned objectType = readU16(m_input);
  if ((OBJECT_TYPE_PAGE_TREE > objectType) || (OBJECT_TYPE_TOC < objectType))
  {
    EBOOK_DEBUG_MSG(("object is of unknown type %x\n", objectType));
    throw ParserException();
  }
  if ((OBJECT_TYPE_UNSPECIFIED != type) && (type != objectType))
  {
    EBOOK_DEBUG_MSG(("expected object of type %x, but found %x\n", type, objectType));
    throw ParserException();
  }

  const unsigned char *const data = readNBytes(m_input, entry.size - 10);
  EBOOKMemoryStream strm(data, entry.size - 10);

  const unsigned endTag = readU16(m_input);
  if (TAG_OBJECT_END != endTag)
  {
    EBOOK_DEBUG_MSG(("no end tag found at the end of the object\n"));
    throw ParserException();
  }

  entry.reading = true;

  switch (objectType)
  {
  case OBJECT_TYPE_PAGE_TREE :
    m_pageTree = id;
    readPageTreeObject(&strm);
    m_pageTree = 0;
    break;
  case OBJECT_TYPE_PAGE :
    readPageObject(&strm);
    break;
  case OBJECT_TYPE_HEADER :
    readHeaderObject(&strm);
    break;
  case OBJECT_TYPE_FOOTER :
    readFooterObject(&strm);
    break;
  case OBJECT_TYPE_PAGE_ATR :
    readPageAtrObject(&strm, id);
    break;
  case OBJECT_TYPE_BLOCK :
    readBlockObject(&strm, id);
    break;
  case OBJECT_TYPE_BLOCK_ATR :
    readBlockAtrObject(&strm, id);
    break;
  case OBJECT_TYPE_MINI_PAGE :
    readMiniPageObject(&strm);
    break;
  case OBJECT_TYPE_BLOCK_LIST :
    readBlockListObject(&strm);
    break;
  case OBJECT_TYPE_TEXT :
    readTextObject(&strm);
    break;
  case OBJECT_TYPE_TEXT_ATR :
    readTextAtrObject(&strm, id);
    break;
  case OBJECT_TYPE_IMAGE :
    readImageObject(&strm, id);
    break;
  case OBJECT_TYPE_CANVAS :
    readCanvasObject(&strm);
    break;
  case OBJECT_TYPE_PARAGRAPH_ATR :
    readParagraphAtrObject(&strm, id);
    break;
  case OBJECT_TYPE_IMAGE_STREAM :
    readImageStreamObject(&strm, id);
    break;
  case OBJECT_TYPE_SoftBookORT :
    readImportObject(&strm);
    break;
  case OBJECT_TYPE_BUTTON :
    readButtonObject(&strm);
    break;
  case OBJECT_TYPE_WINDOW :
    readWindowObject(&strm);
    break;
  case OBJECT_TYPE_POP_UP_WIN :
    readPopUpWinObject(&strm);
    break;
  case OBJECT_TYPE_SOUND :
    readSoundObject(&strm);
    break;
  case OBJECT_TYPE_PLANE_STREAM :
    readPlaneStreamObject(&strm);
    break;
  case OBJECT_TYPE_FONT :
    readFontObject(&strm);
    break;
  case OBJECT_TYPE_OBJECT_INFO :
    readObjectInfoObject(&strm);
    break;
  case OBJECT_TYPE_BOOK_ATR :
    readBookAtrObject(&strm);
    break;
  case OBJECT_TYPE_SSoftBookLE_TEXT :
    readSimpleTextObject(&strm);
    break;
  case OBJECT_TYPE_TOC :
    readTOCObject(&strm);
    break;
  default :
    EBOOK_DEBUG_MSG(("unhandled object type %x\n", type));
    break;
  }

  entry.reading = false;
  entry.read = true;
}

void BBeBParser::readPageTreeObject(librevenge::RVNGInputStream *const object)
{
  bool readAnyPage = false;
  if (TAG_PAGE_LIST == readU16(object))
  {
    unsigned count = readU16(object);
    if (count > getRemainingLength(object) / 4)
      count = getRemainingLength(object) / 4;
    readAnyPage = 0 != count;
    for (unsigned i = 0; i != count; ++i)
      readObject(readU32(object), OBJECT_TYPE_PAGE);
  }

  if (!readAnyPage)
  {
    EBOOK_DEBUG_MSG(("page tree has no pages\n"));
    throw ParserException();
  }
}

void BBeBParser::readPageObject(librevenge::RVNGInputStream *const object)
{
  unsigned pageAtrID = 0;
  BBeBAttributes attributes;
  unsigned streamFlags = 0;
  unsigned streamSize = 0;
  std::unique_ptr<EBOOKMemoryStream> strm;

  while (!object->isEnd())
  {
    const unsigned tag = readU16(object);
    switch (tag)
    {
    case TAG_LINK :
      pageAtrID = readU32(object);
      // It is possible that pages can share attributes. So avoid
      // reading the same Page Atr object twice.
      if (!isObjectRead(pageAtrID))
        readObject(pageAtrID, OBJECT_TYPE_PAGE_ATR);
      break;
    case TAG_PARENT_PAGE_TREE :
      if (readU32(object) != m_pageTree)
      {
        EBOOK_DEBUG_MSG(("page is not belonging to the current page tree\n"));
        throw ParserException();
      }
      break;
    case TAG_STREAM_FLAGS :
      streamFlags = readU16(object);
      if (streamFlags != 0)
      {
        EBOOK_DEBUG_MSG(("page stream with weird flags, giving up\n"));
        throw ParserException();
      }
      break;
    case TAG_STREAM_SIZE :
      streamSize = readU32(object);
      if (streamSize > getRemainingLength(object))
        streamSize = getRemainingLength(object);
      break;
    case TAG_STREAM_START :
    {
      const unsigned char *streamData = readNBytes(object, streamSize);
      strm.reset(new EBOOKMemoryStream(streamData, streamSize));
      if (TAG_STREAM_END != readU16(object))
      {
        EBOOK_DEBUG_MSG(("Stream does not end by Stream End tag\n"));
        throw ParserException();
      }
      break;
    }
    default :
      if (!readAttribute(tag, object, attributes))
        skipUnhandledTag(tag, object, "Page");
      break;
    }
  }

  if (!strm)
  {
    EBOOK_DEBUG_MSG(("No stream data in PageObject\n"));
    throw ParserException();
  }

  m_collector.openPage(pageAtrID, attributes);

  while (!strm->isEnd())
  {
    const unsigned tag = readU16(strm.get());
    if (TAG_LINK == tag)
      readObject(readU32(strm.get())); // this might be a block or a canvas
    else
      skipUnhandledTag(tag, strm.get(), "Page Stream");
  }

  m_collector.closePage();
}

void BBeBParser::readFooterObject(librevenge::RVNGInputStream *const object)
{
  // TODO: implement me
  (void) object;
  EBOOK_DEBUG_MSG(("object type Footer is not supported yet\n"));
}

void BBeBParser::readHeaderObject(librevenge::RVNGInputStream *const object)
{
  // TODO: implement me
  (void) object;
  EBOOK_DEBUG_MSG(("object type Header is not supported yet\n"));
}

void BBeBParser::readPageAtrObject(librevenge::RVNGInputStream *const object, const unsigned id)
{
  BBeBAttributes attributes;

  while (!object->isEnd())
  {
    const unsigned tag = readU16(object);
    if (!readAttribute(tag, object, attributes))
      skipUnhandledTag(tag, object, "Page Atr");
  }

  m_collector.collectPageAttributes(id, attributes);
}

void BBeBParser::readBlockObject(librevenge::RVNGInputStream *const object, const unsigned id)
{
  unsigned blockAtrID = 0;
  BBeBAttributes attributes;
  unsigned streamFlags = 0;
  unsigned streamSize = 0;
  std::unique_ptr<EBOOKMemoryStream> strm;

  while (!object->isEnd())
  {
    const unsigned tag = readU16(object);
    switch (tag)
    {
    case TAG_LINK :
      blockAtrID = readU32(object);
      if (!isObjectRead(blockAtrID))
        readObject(blockAtrID, OBJECT_TYPE_BLOCK_ATR);
      break;
    case TAG_STREAM_FLAGS :
      streamFlags = readU16(object);
      break;
    case TAG_STREAM_SIZE :
      streamSize = readU32(object);
      if (streamSize > getRemainingLength(object))
        streamSize = getRemainingLength(object);
      break;
    case TAG_STREAM_START :
    {
      const unsigned char *const streamData = readNBytes(object, streamSize);
      if (0 == streamFlags)
        strm.reset(new EBOOKMemoryStream(streamData, streamSize));
      if (TAG_STREAM_END != readU16(object))
      {
        EBOOK_DEBUG_MSG(("Stream does not end by Stream End tag\n"));
        throw ParserException();
      }
      break;
    }
    default :
      if (!readAttribute(tag, object, attributes))
        skipUnhandledTag(tag, object, "Block");
      break;
    }
  }

  if ((0 == streamFlags) && !strm)
  {
    EBOOK_DEBUG_MSG(("No stream data in BlockObject\n"));
    throw ParserException();
  }

  if (std::binary_search(m_toc.begin(), m_toc.end(), id))
    attributes.heading = true;

  m_collector.openBlock(blockAtrID, attributes);

  if (bool(strm))
  {
    while (!strm->isEnd())
    {
      const unsigned tag = readU16(strm.get());
      if (TAG_LINK == tag)
        readObject(readU32(strm.get()));
      else
        skipUnhandledTag(tag, strm.get(), "Block Stream");
    }
  }
  else
  {
    EBOOK_DEBUG_MSG(("block stream with weird flags, giving up\n"));
    throw ParserException();
  }

  m_collector.closeBlock();
}

void BBeBParser::readBlockAtrObject(librevenge::RVNGInputStream *const object, const unsigned id)
{
  BBeBAttributes attributes;

  while (!object->isEnd())
  {
    const unsigned tag = readU16(object);
    if (!readAttribute(tag, object, attributes))
      skipUnhandledTag(tag, object, "Block Atr");
  }

  m_collector.collectBlockAttributes(id, attributes);
}

void BBeBParser::readMiniPageObject(librevenge::RVNGInputStream *const object)
{
  // TODO: implement me
  (void) object;
  EBOOK_DEBUG_MSG(("object type Mini Page is not supported yet\n"));
}

void BBeBParser::readBlockListObject(librevenge::RVNGInputStream *const object)
{
  // TODO: implement me
  (void) object;
  EBOOK_DEBUG_MSG(("object type Block List is not supported yet\n"));
}

void BBeBParser::readTextObject(librevenge::RVNGInputStream *const object)
{
  unsigned textAtrID = 0;
  BBeBAttributes attributes;
  unsigned streamFlags = 0;
  unsigned streamSize = 0;
  std::unique_ptr<librevenge::RVNGInputStream> textStrm;
  unsigned textLength = 0;

  while (!object->isEnd())
  {
    const unsigned tag = readU16(object);
    switch (tag)
    {
    case TAG_LINK :
      textAtrID = readU32(object);
      if (!isObjectRead(textAtrID))
        readObject(textAtrID, OBJECT_TYPE_TEXT_ATR);
      break;
    case TAG_STREAM_FLAGS :
      streamFlags = readU16(object);
      break;
    case TAG_STREAM_SIZE :
      streamSize = readU32(object);
      break;
    case TAG_STREAM_START :
    {
      if (streamSize > getRemainingLength(object))
        streamSize = getRemainingLength(object);
      if (STREAM_TYPE_BBEB_TAGS_COMPRESSED == streamFlags)
      {
        textLength = readU32(object);
        if (streamSize <= 4)
        {
          EBOOK_DEBUG_MSG(("Compressed stream is too short\n"));
          throw ParserException();
        }
        streamSize -= 4;
      }
      else
        textLength = streamSize;
      const unsigned char *const streamData = readNBytes(object, streamSize);

      // prepare text stream
      if (STREAM_TYPE_BBEB_TAGS == streamFlags)
        textStrm.reset(new EBOOKMemoryStream(streamData, streamSize));
      else if (STREAM_TYPE_BBEB_TAGS_COMPRESSED == streamFlags)
      {
        EBOOKMemoryStream strm(streamData, streamSize);
        textStrm.reset(new EBOOKZlibStream(&strm));
      }
      else
      {
        EBOOK_DEBUG_MSG(("Text stream of invalid type\n"));
        throw ParserException();
      }

      if (TAG_STREAM_END != readU16(object))
      {
        EBOOK_DEBUG_MSG(("Stream does not end by Stream End tag\n"));
        throw ParserException();
      }
    }
    break;
    default :
      if (!readAttribute(tag, object, attributes))
        skipUnhandledTag(tag, object, "Text");
      break;
    }
  }

  if (0 != textLength)
  {
    m_collector.openTextBlock(textAtrID, attributes);

    // process text, if any
    BBeBAttributes textAttributes;
    bool inParagraph = false;

    while (!textStrm->isEnd())
    {
      const unsigned tag = readU16(textStrm.get());

      switch (tag)
      {
      case TAG_BEGIN_P :
      {
        // NOTE: I am not quite sure the argument is an object ID.
        // But it makes sense.
        const unsigned paragraphAtrID = readU32(textStrm.get());
        if ((0 != paragraphAtrID) && !isObjectRead(paragraphAtrID))
          readObject(paragraphAtrID, OBJECT_TYPE_PARAGRAPH_ATR);
        m_collector.openParagraph(paragraphAtrID, attributes);
        inParagraph = true;
      }
      break;
      case TAG_END_P :
        if (inParagraph)
        {
          m_collector.closeParagraph();
        }
        else
        {
          EBOOK_DEBUG_MSG(("End P tag encountered, but there is no paragraph opened\n"));
        }
        inParagraph = false;
        break;
      case TAG_TEXT_SIZE :
      {
        const std::string &text = readString(textStrm.get());
        m_collector.collectText(text, textAttributes);
      }
      break;
      case TAG_EOL :
        if (inParagraph)
          m_collector.insertLineBreak();
        break;
      case TAG_KOMA_PLOT :
      {
        skip(textStrm.get(), 4);
        const unsigned imageID = readU32(textStrm.get());
        skip(textStrm.get(), 4);
        if (!isObjectRead(imageID))
          readObject(imageID, OBJECT_TYPE_IMAGE);
        m_collector.insertImage(imageID);
      }
      break;
      default :
        if (!readAttribute(tag, textStrm.get(), textAttributes))
          skipUnhandledTag(tag, textStrm.get(), "Text Stream");
      }
    }

    if (inParagraph)
    {
      EBOOK_DEBUG_MSG(("No End P tag encountered\n"));
      m_collector.closeParagraph();
    }

    m_collector.closeTextBlock();
  }
}

void BBeBParser::readTextAtrObject(librevenge::RVNGInputStream *const object, const unsigned id)
{
  BBeBAttributes attributes;

  while (!object->isEnd())
  {
    const unsigned tag = readU16(object);
    if (!readAttribute(tag, object, attributes))
      skipUnhandledTag(tag, object, "Text Atr");
  }

  m_collector.collectTextAttributes(id, attributes);
}

void BBeBParser::readImageObject(librevenge::RVNGInputStream *const object, const unsigned id)
{
  unsigned width = 0;
  unsigned height = 0;

  while (!object->isEnd())
  {
    const unsigned tag = readU16(object);

    switch (tag)
    {
    case TAG_IMAGE_CROP_RECT :
      skip(object, 8);
      break;
    case TAG_IMAGE_SIZE :
    {
      width = readU16(object);
      height = readU16(object);
      break;
    }
    case TAG_IMAGE_STREAM :
    {
      const unsigned image = readU32(object);
      if (!isObjectRead(image))
        readObject(image, OBJECT_TYPE_IMAGE_STREAM);
      m_collector.collectImage(id, image, width, height);
      break;
    }
    default :
      skipUnhandledTag(tag, object, "Image");
      break;
    }
  }
}

void BBeBParser::readCanvasObject(librevenge::RVNGInputStream *const object)
{
  // TODO: implement me
  (void) object;
  EBOOK_DEBUG_MSG(("object type Canvas is not supported yet\n"));
}

void BBeBParser::readParagraphAtrObject(librevenge::RVNGInputStream *const object, const unsigned id)
{
  BBeBAttributes attributes;

  while (!object->isEnd())
  {
    const unsigned tag = readU16(object);
    if (!readAttribute(tag, object, attributes))
      skipUnhandledTag(tag, object, "Paragraph Atr");
  }

  m_collector.collectParagraphAttributes(id, attributes);
}

void BBeBParser::readImageStreamObject(librevenge::RVNGInputStream *const object, const unsigned id)
{
  unsigned streamType = 0;
  unsigned streamSize = 0;
  RVNGInputStreamPtr_t image;

  while (!object->isEnd())
  {
    const unsigned tag = readU16(object);

    switch (tag)
    {
    case TAG_STREAM_FLAGS :
      streamType = readU16(object);
      break;
    case TAG_STREAM_SIZE :
      streamSize = readU16(object);
      if (streamSize > getRemainingLength(object))
        streamSize = getRemainingLength(object);
      break;
    case TAG_STREAM_START :
    {
      const unsigned char *const streamData = readNBytes(object, streamSize);

      switch (streamType)
      {
      case STREAM_TYPE_JPEG :
      case STREAM_TYPE_PNG :
      case STREAM_TYPE_BMP :
      case STREAM_TYPE_GIF :
        image.reset(new EBOOKMemoryStream(streamData, streamSize));
        break;
      default :
        EBOOK_DEBUG_MSG(("unknown image type %x\n", streamType));
        throw ParserException();
      }

      if (TAG_STREAM_END != readU16(object))
      {
        EBOOK_DEBUG_MSG(("Stream does not end by Stream End tag\n"));
        throw ParserException();
      }
      break;
    }
    default :
      skipUnhandledTag(tag, object, "Image Stream");
    }
  }

  if (!image)
  {
    EBOOK_DEBUG_MSG(("No stream data in ImageStreamObject\n"));
    throw ParserException();
  }

  m_collector.collectImageData(id, static_cast<BBeBImageType>(streamType), image);
}

void BBeBParser::readImportObject(librevenge::RVNGInputStream *const object)
{
  // TODO: implement me
  (void) object;
  EBOOK_DEBUG_MSG(("object type Import is not supported yet\n"));
}

void BBeBParser::readButtonObject(librevenge::RVNGInputStream *const object)
{
  // TODO: implement me
  (void) object;
  EBOOK_DEBUG_MSG(("object type Button is not supported yet\n"));
}

void BBeBParser::readWindowObject(librevenge::RVNGInputStream *const object)
{
  // TODO: implement me
  (void) object;
  EBOOK_DEBUG_MSG(("object type Window is not supported yet\n"));
}

void BBeBParser::readPopUpWinObject(librevenge::RVNGInputStream *const object)
{
  // TODO: implement me
  (void) object;
  EBOOK_DEBUG_MSG(("object type Pop Up Win is not supported yet\n"));
}

void BBeBParser::readSoundObject(librevenge::RVNGInputStream *const object)
{
  // TODO: implement me
  (void) object;
  EBOOK_DEBUG_MSG(("object type Sound is not supported yet\n"));
}

void BBeBParser::readPlaneStreamObject(librevenge::RVNGInputStream *const object)
{
  // TODO: implement me
  (void) object;
  EBOOK_DEBUG_MSG(("object type Plane Stream is not supported yet\n"));
}

void BBeBParser::readFontObject(librevenge::RVNGInputStream *const object)
{
  // TODO: implement me
  (void) object;
  EBOOK_DEBUG_MSG(("object type Font is not supported yet\n"));
}

void BBeBParser::readObjectInfoObject(librevenge::RVNGInputStream *const object)
{
  // TODO: implement me
  (void) object;
  EBOOK_DEBUG_MSG(("object type Object Info is not supported yet\n"));
}

void BBeBParser::readBookAtrObject(librevenge::RVNGInputStream *const object)
{
  BBeBAttributes attributes;
  unsigned pageTree = 0;

  while (!object->isEnd())
  {
    const unsigned tag = readU16(object);
    switch (tag)
    {
    case TAG_CHILD_PAGE_TREE :
      pageTree = readU32(object);
      break;
    default :
      if (!readAttribute(tag, object, attributes))
        skipUnhandledTag(tag, object, "Book Atr");
    }
  }

  m_collector.collectBookAttributes(attributes);

  if (0 == pageTree)
  {
    EBOOK_DEBUG_MSG(("the book has no page tree"));
    throw ParserException();
  }
  readObject(pageTree, OBJECT_TYPE_PAGE_TREE);
}

void BBeBParser::readSimpleTextObject(librevenge::RVNGInputStream *const object)
{
  // TODO: implement me
  (void) object;
  EBOOK_DEBUG_MSG(("object type Simple Text is not supported yet\n"));
}

void BBeBParser::readTOCObject(librevenge::RVNGInputStream *const object)
{
  unsigned streamFlags = 0;
  unsigned streamSize = 0;
  std::unique_ptr<EBOOKMemoryStream> data;

  while (!object->isEnd())
  {
    const unsigned tag = readU16(object);
    switch (tag)
    {
    case TAG_STREAM_FLAGS :
      streamFlags = readU16(object);
      if (STREAM_TYPE_BBEB_TOC != streamFlags)
      {
        EBOOK_DEBUG_MSG(("unexpected ToC stream type %x\n", streamFlags));
      }
      break;
    case TAG_STREAM_SIZE :
      streamSize = readU32(object);
      if (streamSize > getRemainingLength(object))
        streamSize = getRemainingLength(object);
      break;
    case TAG_STREAM_START :
      if (STREAM_TYPE_BBEB_TOC == streamFlags)
      {
        const unsigned char *const streamData = readNBytes(object, streamSize);
        data.reset(new EBOOKMemoryStream(streamData, streamSize));
        if (TAG_STREAM_END != readU16(object))
        {
          EBOOK_DEBUG_MSG(("Stream does not end by Stream End tag\n"));
          throw ParserException();
        }
      }
      else
      {
        skip(object, streamSize);
      }
      break;
    default :
      skipUnhandledTag(tag, object, "ToC");
      break;
    }
  }

  if (bool(data))
    readToCStream(data.get());
}

void BBeBParser::readToCStream(librevenge::RVNGInputStream *const input)
{
  unsigned count = readU32(input);
  if (count > getRemainingLength(input) / 4)
    count = getRemainingLength(input) / 4;
  std::vector<unsigned> offsets;

  offsets.reserve(count);
  for (unsigned i = 0; count != i; ++i)
    offsets.push_back(readU32(input));

  const long start = input->tell();
  m_toc.reserve(count);
  for (std::vector<unsigned>::const_iterator it = offsets.begin(); offsets.end() != it; ++it)
  {
    seek(input, (unsigned long)(start + int(*it) + 4));
    const unsigned oid = readU32(input);
    if (m_objectIndex.end() != m_objectIndex.find(oid))
    {
      m_toc.push_back(oid);
    }
    else
    {
      EBOOK_DEBUG_MSG(("ToC refers to an unexistent block id %x\n", oid));
    }
  }

  std::sort(m_toc.begin(), m_toc.end());
}

bool BBeBParser::readAttribute(const unsigned tag, librevenge::RVNGInputStream *const input, BBeBAttributes &attributes)
{
  bool handled = true;

  switch (tag)
  {
  case TAG_FONT_SIZE :
    attributes.fontSize = readU16(input);
    break;
  case TAG_FONT_WIDTH :
    attributes.fontWidth = readU16(input);
    break;
  case TAG_FONT_ESCAPEMENT :
    attributes.fontEscapement = readU16(input);
    break;
  case TAG_FONT_ORIENTATION :
    attributes.fontOrientation = readU16(input);
    break;
  case TAG_FONT_WEIGHT :
    attributes.fontWeight = readU16(input);
    break;
  case TAG_FONT_FACENAME :
    attributes.fontFacename = readString(input);
    break;
  case TAG_TEXT_COLOR :
    attributes.textColor = BBeBColor(readU32(input));
    break;
  case TAG_TEXT_BG_COLOR :
    attributes.textBgColor = BBeBColor(readU32(input));
    break;
  case TAG_WORD_SPACE :
    attributes.wordSpace = readU16(input);
    break;
  case TAG_LETTER_SPACE :
    attributes.letterSpace = readU16(input);
    break;
  case TAG_BASE_LINE_SKIP :
    attributes.baseLineSkip = readU16(input);
    break;
  case TAG_LINE_SPACE :
    attributes.lineSpace = readU16(input);
    break;
  case TAG_PAR_INDENT :
    attributes.parIndent = readU16(input);
    break;
  case TAG_PAR_SKIP :
    attributes.parSkip = readU16(input);
    break;
  case TAG_PAGE_HEIGHT :
  case TAG_BLOCK_HEIGHT :
  case TAG_MINI_PAGE_HEIGHT :
  case TAG_CANVAS_HEIGHT :
    attributes.height = readU16(input);
    break;
  case TAG_PAGE_WIDTH :
  case TAG_BLOCK_WIDTH :
  case TAG_MINI_PAGE_WIDTH :
  case TAG_CANVAS_WIDTH :
    attributes.width = readU16(input);
    break;
  case TAG_LOCATION_X :
    attributes.locationX = readU16(input);
    break;
  case TAG_LOCATION_Y :
    attributes.locationY = readU16(input);
    break;
  case TAG_BEGIN_ITALIC :
    attributes.italic = true;
    break;
  case TAG_END_ITALIC :
    attributes.italic = false;
    break;
  case TAG_BEGIN_SUP :
    attributes.sup = true;
    break;
  case TAG_END_SUP :
    attributes.sup = false;
    break;
  case TAG_BEGIN_SUB :
    attributes.sub = true;
    break;
  case TAG_END_SUB :
    attributes.sub = false;
    break;
  case TAG_BEGIN_EMP_LINE :
    attributes.emptyLine = BBeBEmptyLine();
    break;
  case TAG_END_EMP_LINE :
    attributes.emptyLine.reset();
    break;
  case TAG_EMPTY_LINE_POSITION :
  {
    const unsigned position = readU16(input);
    if (attributes.emptyLine)
    {
      switch (position)
      {
      case 0x1 :
        get(attributes.emptyLine).m_position = BBEB_EMPTY_LINE_POSITION_BEFORE;
        break;
      case 0x2 :
        get(attributes.emptyLine).m_position = BBEB_EMPTY_LINE_POSITION_AFTER;
        break;
      default :
        EBOOK_DEBUG_MSG(("unkown Empty Line Position: %u\n", position));
        break;
      }
    }
    else
    {
      EBOOK_DEBUG_MSG(("Empty Line Position without Begin Emp Line\n"));
    }
    break;
  }
  case TAG_EMPTY_LINE_MODE :
  {
    const unsigned mode = readU16(input);
    if (attributes.emptyLine)
    {
      switch (mode)
      {
      case 0x0 :
        get(attributes.emptyLine).m_mode = BBEB_EMPTY_LINE_MODE_NONE;
        break;
      case 0x10 :
        get(attributes.emptyLine).m_mode = BBEB_EMPTY_LINE_MODE_SOLID;
        break;
      case 0x20 :
        get(attributes.emptyLine).m_mode = BBEB_EMPTY_LINE_MODE_DASHED;
        break;
      case 0x30 :
        get(attributes.emptyLine).m_mode = BBEB_EMPTY_LINE_MODE_DOUBLE;
        break;
      case 0x40 :
        get(attributes.emptyLine).m_mode = BBEB_EMPTY_LINE_MODE_DOTTED;
        break;
      default :
        EBOOK_DEBUG_MSG(("unkown Empty Line Mode: %u\n", mode));
        break;
      }
    }
    else
    {
      EBOOK_DEBUG_MSG(("Empty Line Mode without Begin Emp Line\n"));
    }
    break;
  }
  case TAG_ALIGN :
  {
    const unsigned align = readU16(input);
    switch (align)
    {
    case 0x1 :
      attributes.align = BBEB_ALIGN_START;
      break;
    case 0x4 :
      attributes.align = BBEB_ALIGN_CENTER;
      break;
    case 0x8 :
      attributes.align = BBEB_ALIGN_END;
      break;
    default :
      EBOOK_DEBUG_MSG(("unkown Align: %u\n", align));
      break;
    }
    break;
  }
  case TAG_TOP_SKIP :
    attributes.topSkip = readU16(input);
    break;
  case TAG_TOP_MARGIN :
    attributes.topMargin = readU16(input);
    break;
  case TAG_ODD_SIDE_MARGIN :
    attributes.oddSideMargin = readU16(input);
    break;
  case TAG_EVEN_SIDE_MARGIN :
    attributes.evenSideMargin = readU16(input);
    break;
  default :
    handled = false;
    break;
  }

  return handled;
}

void BBeBParser::skipUnhandledTag(const unsigned tag, librevenge::RVNGInputStream *const input, const char *const objectType)
{
  switch (tag)
  {
  case TAG_OBJECT_END :
    EBOOK_DEBUG_MSG(("Object End tag should never appear\n"));
  case TAG_STREAM_START :
  case TAG_STREAM_END :
  case TAG_F54D :
  case TAG_F562 :
  case TAG_F563 :
  case TAG_F564 :
  case TAG_F565 :
  case TAG_F566 :
  case TAG_F567 :
  case TAG_F568 :
  case TAG_F569 :
  case TAG_F56A :
  case TAG_F56B :
  case TAG_F56E :
  case TAG_F571 :
  case TAG_F572 :
  case TAG_BEGIN_ITALIC :
  case TAG_END_ITALIC :
  case TAG_END_P :
  case TAG_KOMA_EMP_DOT_CHAR :
  case TAG_END_BUTTON :
  case TAG_BEGIN_RUBY :
  case TAG_END_RUBY :
  case TAG_BEGIN_RUBY_BASE :
  case TAG_END_RUBY_BASE :
  case TAG_BEGIN_RUBY_TEXT :
  case TAG_END_RUBY_TEXT :
  case TAG_KOMA_YOKOMOJI :
  case TAG_F5B2 :
  case TAG_BEGIN_TATE :
  case TAG_END_TATE :
  case TAG_BEGIN_NEKASE :
  case TAG_END_NEKASE :
  case TAG_BEGIN_SUP :
  case TAG_END_SUP :
  case TAG_BEGIN_SUB :
  case TAG_END_SUB :
  case TAG_F5BB :
  case TAG_F5BC :
  case TAG_F5BD :
  case TAG_F5BE :
  case TAG_BEGIN_EMP_LINE :
  case TAG_END_EMP_LINE :
  case TAG_END_DRAW_CHAR :
  case TAG_F5C7 :
  case TAG_F5C9 :
  case TAG_EOL :
  case TAG_SOUND_STOP :
    // no arguments
    break;

  case TAG_F50E :
  case TAG_FONT_SIZE :
  case TAG_FONT_WIDTH :
  case TAG_FONT_ESCAPEMENT :
  case TAG_FONT_ORIENTATION :
  case TAG_FONT_WEIGHT :
  case TAG_WORD_SPACE :
  case TAG_LETTER_SPACE :
  case TAG_BASE_LINE_SKIP :
  case TAG_LINE_SPACE :
  case TAG_PAR_INDENT :
  case TAG_PAR_SKIP :
  case TAG_TOP_MARGIN :
  case TAG_F522 :
  case TAG_F523 :
  case TAG_ODD_SIDE_MARGIN :
  case TAG_PAGE_HEIGHT :
  case TAG_PAGE_WIDTH :
  case TAG_F527 :
  case TAG_F528 :
  case TAG_F52A :
  case TAG_F52B :
  case TAG_EVEN_SIDE_MARGIN :
  case TAG_F52E :
  case TAG_BLOCK_WIDTH :
  case TAG_BLOCK_HEIGHT :
  case TAG_BLOCK_RULE :
  case TAG_F535 :
  case TAG_F536 :
  case TAG_TOP_SKIP :
  case TAG_F539 :
  case TAG_F53A :
  case TAG_ALIGN :
  case TAG_F53D :
  case TAG_F53E :
  case TAG_MINI_PAGE_HEIGHT :
  case TAG_MINI_PAGE_WIDTH :
  case TAG_LOCATION_Y :
  case TAG_LOCATION_X :
  case TAG_F548 :
  case TAG_CANVAS_WIDTH :
  case TAG_CANVAS_HEIGHT :
  case TAG_STREAM_FLAGS :
  case TAG_F557 :
  case TAG_F558 :
  case TAG_F55E :
  case TAG_F561 :
  case TAG_RUBY_ALIGN :
  case TAG_RUBY_OVERHANG :
  case TAG_EMPTY_DOTS_POSITION :
  case TAG_EMPTY_LINE_POSITION :
  case TAG_EMPTY_LINE_MODE :
  case TAG_BEGIN_DRAW_CHAR :
  case TAG_F5C5 :
  case TAG_F5C6 :
  case TAG_KOMA_AUTO_SPACING :
  case TAG_SPACE :
  case TAG_TEXT_SIZE :
  case TAG_WAIT :
  case TAG_F5DA :
  case TAG_F5DB :
  case TAG_F5DC :
  case TAG_CHAR_SPACE :
  case TAG_LINE_WIDTH :
  case TAG_LINE_MODE :
    skip(input, 2);
    break;

  case TAG_OBJECT_INFO_LINK :
  case TAG_LINK :
  case TAG_STREAM_SIZE :
  case TAG_F507 :
  case TAG_F508 :
  case TAG_F509 :
  case TAG_F50A :
  case TAG_TEXT_COLOR :
  case TAG_TEXT_BG_COLOR :
  case TAG_F52D :
  case TAG_F534 :
  case TAG_F537 :
  case TAG_F544 :
  case TAG_F545 :
  case TAG_IMAGE_SIZE :
  case TAG_IMAGE_STREAM :
  case TAG_F553 :
  case TAG_VIEW_POINT :
  case TAG_CHILD_PAGE_TREE :
  case TAG_PARENT_PAGE_TREE :
  case TAG_BEGIN_P :
  case TAG_BEGIN_BUTTON :
  case TAG_BOOK_FONT :
  case TAG_LINE_COLOR :
  case TAG_FILL_COLOR :
  case TAG_MOVE_TO :
  case TAG_LINE_TO :
  case TAG_DRAW_BOX :
  case TAG_DRAW_ELLIPSE :
    skip(input, 4);
    break;

  case TAG_OBJECT_START :
    EBOOK_DEBUG_MSG(("Object Start tag should never appear\n"));
  // fall-through intended
  case TAG_F529 :
  case TAG_F5F9 :
    skip(input, 6);
    break;

  case TAG_PUT_SOUND :
  case TAG_IMAGE_CROP_RECT :
  case TAG_JUMP_TO :
  case TAG_KOMA_PLOT_TEXT :
    skip(input, 8);
    break;

  case TAG_RULED_LINE :
    skip(input, 10);
    break;

  case TAG_F54E :
  case TAG_KOMA_PLOT :
    skip(input, 12);
    break;

  case TAG_MOVE_OBJ :
    skip(input, 14);
    break;

  case TAG_CONTAINED_OBJECTS_LIST :
  case TAG_PAGE_LIST :
  {
    unsigned count = readU16(input);
    skip(input, count * 4);
  }
  break;
  case TAG_F50D :
  case TAG_FONT_FACENAME :
  case TAG_F555 :
  case TAG_F556 :
  case TAG_FONT_FILE_NAME :
  case TAG_F55A :
  case TAG_FONT_FACE_NAME :
  case TAG_F56D :
  case TAG_EMPTY_DOTS_CODE :
  case TAG_KOMA_GAIJI :
  case TAG_F5CB :
  {
    // try to find the next tag
    unsigned n = 1;
    while (!input->isEnd() && ((0xf5 != readU8(input)) || (2 <= n)))
      ++n;
    if (!input->isEnd())
      input->seek(-2, librevenge::RVNG_SEEK_CUR);
  }
  break;
  default :
    EBOOK_DEBUG_MSG(("unknown tag %x\n", tag));
    return;
  }

  (void) objectType;
  EBOOK_DEBUG_MSG(("skipping unhandled tag %x at %s object\n", tag, objectType));
}

bool BBeBParser::isObjectRead(const unsigned id) const
{
  const ObjectIndex_t::const_iterator it = m_objectIndex.find(id);
  return (m_objectIndex.end() != it) && it->second.read;
}

} // namespace libebook

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
