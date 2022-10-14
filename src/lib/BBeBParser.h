/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef BBEBPARSER_H_INCLUDED
#define BBEBPARSER_H_INCLUDED

#include <vector>
#include <map>
#include <memory>

#include "BBeBCollector.h"


namespace libebook
{

struct BBeBHeader;

class BBeBParser
{
  // disable copying
  BBeBParser(const BBeBParser &other);
  BBeBParser &operator=(const BBeBParser &other);

  enum ObjectType
  {
    OBJECT_TYPE_UNSPECIFIED = 0x0,
    OBJECT_TYPE_PAGE_TREE = 0x1,
    OBJECT_TYPE_PAGE = 0x2,
    OBJECT_TYPE_HEADER = 0x3,
    OBJECT_TYPE_FOOTER = 0x4,
    OBJECT_TYPE_PAGE_ATR = 0x5,
    OBJECT_TYPE_BLOCK = 0x6,
    OBJECT_TYPE_BLOCK_ATR = 0x7,
    OBJECT_TYPE_MINI_PAGE = 0x8,
    OBJECT_TYPE_BLOCK_LIST = 0x9,
    OBJECT_TYPE_TEXT = 0xa,
    OBJECT_TYPE_TEXT_ATR = 0xb,
    OBJECT_TYPE_IMAGE = 0xc,
    OBJECT_TYPE_CANVAS = 0xd,
    OBJECT_TYPE_PARAGRAPH_ATR = 0xe,
    OBJECT_TYPE_IMAGE_STREAM = 0x11,
    OBJECT_TYPE_SoftBookORT = 0x12,
    OBJECT_TYPE_BUTTON = 0x13,
    OBJECT_TYPE_WINDOW = 0x14,
    OBJECT_TYPE_POP_UP_WIN = 0x15,
    OBJECT_TYPE_SOUND = 0x16,
    OBJECT_TYPE_PLANE_STREAM = 0x17,
    OBJECT_TYPE_FONT = 0x19,
    OBJECT_TYPE_OBJECT_INFO = 0x1a,
    OBJECT_TYPE_BOOK_ATR = 0x1c,
    OBJECT_TYPE_SSoftBookLE_TEXT = 0x1d,
    OBJECT_TYPE_TOC = 0x1e
  };

  struct BBeBIndexEntry
  {
    unsigned offset;
    unsigned size;
    bool reading;
    bool read;
  };

  typedef std::map<unsigned, BBeBIndexEntry> ObjectIndex_t;
  typedef std::vector<unsigned> ToC_t;

public:
  BBeBParser(librevenge::RVNGInputStream *input, librevenge::RVNGTextInterface *document);
  ~BBeBParser();

  bool parse();

  static bool isSupported(librevenge::RVNGInputStream *input);

private:
  void readHeader();
  void readMetadata();
  void readThumbnail();
  void readObjectIndex();

  void readObject(unsigned id, unsigned type = OBJECT_TYPE_UNSPECIFIED);

  void readPageTreeObject(librevenge::RVNGInputStream *object);
  void readPageObject(librevenge::RVNGInputStream *object);
  void readFooterObject(librevenge::RVNGInputStream *object);
  void readHeaderObject(librevenge::RVNGInputStream *object);
  void readPageAtrObject(librevenge::RVNGInputStream *object, unsigned id);
  void readBlockObject(librevenge::RVNGInputStream *object, unsigned id);
  void readBlockAtrObject(librevenge::RVNGInputStream *object, unsigned id);
  void readMiniPageObject(librevenge::RVNGInputStream *object);
  void readBlockListObject(librevenge::RVNGInputStream *object);
  void readTextObject(librevenge::RVNGInputStream *object);
  void readTextAtrObject(librevenge::RVNGInputStream *object, unsigned id);
  void readImageObject(librevenge::RVNGInputStream *object, unsigned id);
  void readCanvasObject(librevenge::RVNGInputStream *object);
  void readParagraphAtrObject(librevenge::RVNGInputStream *object, unsigned id);
  void readImageStreamObject(librevenge::RVNGInputStream *object, unsigned id);
  void readImportObject(librevenge::RVNGInputStream *object);
  void readButtonObject(librevenge::RVNGInputStream *object);
  void readWindowObject(librevenge::RVNGInputStream *object);
  void readPopUpWinObject(librevenge::RVNGInputStream *object);
  void readSoundObject(librevenge::RVNGInputStream *object);
  void readPlaneStreamObject(librevenge::RVNGInputStream *object);
  void readFontObject(librevenge::RVNGInputStream *object);
  void readObjectInfoObject(librevenge::RVNGInputStream *object);
  void readBookAtrObject(librevenge::RVNGInputStream *object);
  void readSimpleTextObject(librevenge::RVNGInputStream *object);
  void readTOCObject(librevenge::RVNGInputStream *object);

  void readToCStream(librevenge::RVNGInputStream *input);

  bool readAttribute(unsigned tag, librevenge::RVNGInputStream *input, BBeBAttributes &attributes);

  void skipUnhandledTag(unsigned tag, librevenge::RVNGInputStream *input, const char *objectType);

  bool isObjectRead(unsigned id) const;

  double toInches(unsigned px) const;

private:
  BBeBCollector m_collector;
  librevenge::RVNGInputStream *m_input;
  std::unique_ptr<BBeBHeader> m_header;
  ObjectIndex_t m_objectIndex;
  unsigned m_pageTree;
  ToC_t m_toc;
};

} // namespace libebook

#endif // BBEBPARSER_H_INCLUDED
/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
