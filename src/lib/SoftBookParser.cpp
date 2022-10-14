/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <cassert>
#include <deque>

#include <librevenge-stream/librevenge-stream.h>

#include "libebook_utils.h"
#include "EBOOKStreamView.h"
#include "SoftBookLZSSStream.h"
#include "SoftBookParser.h"
#include "SoftBookResourceDir.h"
#include "SoftBookText.h"

using std::shared_ptr;

namespace libebook
{

namespace
{

static const char *const SOFTBOOK_TEXT_FILE_NAME = "    ";

}

namespace
{

struct SoftBookParserException {};

#ifdef UNUSED
void skipStandardHeader(std::shared_ptr<librevenge::RVNGInputStream> stream)
{
  assert(bool(stream));
  skip(stream.get(), 32);
}
#endif

SoftBookLZSSStream::Configuration makeLZSSConfiguration(const unsigned offsetBits, const unsigned lengthBits, const unsigned uncompressedLength)
{
  SoftBookLZSSStream::Configuration configuration;

  configuration.offsetBits = offsetBits;
  configuration.lengthBits = lengthBits;
  configuration.uncompressedLength = uncompressedLength;
  configuration.fillPos = 1;
  configuration.allowOverflow = false;
  configuration.bigEndian = true;

  return configuration;
}

}

namespace
{

class Resource
{
  struct ResourceIndexEntry
  {
    ResourceIndexEntry();

    unsigned id;
    unsigned length;
    unsigned offset;
  };

  typedef std::deque<ResourceIndexEntry> ResourceIndex_t;

public:
  enum IndexMode
  {
    INDEX_MODE_NORMAL,
    INDEX_MODE_WORKAROUND
  };

public:
  Resource(const char *name, shared_ptr<librevenge::RVNGInputStream> stream, const SoftBookHeader &header);

  unsigned getRecordCount() const;

  unsigned getRecordId(unsigned n) const;

  shared_ptr<librevenge::RVNGInputStream> getRecord(unsigned n) const;

private:
  void readResourceIndex(IndexMode mode);

private:
  const shared_ptr<librevenge::RVNGInputStream> m_stream;
  long m_start;
  ResourceIndex_t m_resourceIndex;
};

Resource::ResourceIndexEntry::ResourceIndexEntry()
  : id(0)
  , length(0)
  , offset(0)
{
}

Resource::Resource(const char *const name, const shared_ptr<librevenge::RVNGInputStream> stream, const SoftBookHeader &header)
  : m_stream(stream)
  , m_start(stream->tell())
  , m_resourceIndex()
{
  const unsigned version = readU16(stream, true);
  if (1 != version)
  {
    EBOOK_DEBUG_MSG(("version of resource %s is not 1\n", name));
  }

  const char *const type = reinterpret_cast<const char *>(readNBytes(stream, 4));
  if (!std::equal(name, name + 4, type))
  {
    EBOOK_DEBUG_MSG(("type of resource does not match: %s expected, %s got\n", name, type));
  }

  readResourceIndex((SOFTBOOK_COLOR_MODE_GRAYSCALE == header.getColorMode()) ? INDEX_MODE_WORKAROUND : INDEX_MODE_NORMAL);
}

unsigned Resource::getRecordCount() const
{
  return (unsigned) m_resourceIndex.size();
}

unsigned Resource::getRecordId(const unsigned n) const
{
  return (n < m_resourceIndex.size()) ? m_resourceIndex[n].id : 0;
}

shared_ptr<librevenge::RVNGInputStream> Resource::getRecord(const unsigned n) const
{
  shared_ptr<librevenge::RVNGInputStream> record;

  if (n < m_resourceIndex.size())
  {
    const ResourceIndexEntry &entry = m_resourceIndex[n];
    const long start = m_start + (long) entry.offset;
    record.reset(new EBOOKStreamView(m_stream.get(), start, start + (long) entry.length));
  }

  return record;
}

void Resource::readResourceIndex(const IndexMode mode)
{
  m_stream->seek(m_start + 0xa, librevenge::RVNG_SEEK_SET);
  const unsigned offset = readU32(m_stream, true);
  m_stream->seek(m_start + (long) offset, librevenge::RVNG_SEEK_SET);

  while (!m_stream->isEnd())
  {
    ResourceIndexEntry entry;
    if (INDEX_MODE_NORMAL == mode)
    {
      entry.id = readU16(m_stream, true);
      entry.length = readU32(m_stream, true);
      entry.offset = readU32(m_stream, true);
    }
    else
    {
      // this is really LITTLE endian
      entry.id = readU32(m_stream);
      entry.length = readU32(m_stream);
      entry.offset = readU32(m_stream);
    }
    skip(m_stream, 2);
    m_resourceIndex.push_front(entry);
  }
}

}

SoftBookParser::SoftBookParser(librevenge::RVNGInputStream *const input, librevenge::RVNGTextInterface *const document)
  : m_header(input)
  , m_input(input)
  , m_collector(document)
  , m_resources()
  , m_text()
{
}

bool SoftBookParser::parse()
{
  SoftBookResourceDir resourceDir(m_input, m_header);
  m_resources = resourceDir.getTypeStream();

  createTextStream();

  SoftBookText text(m_text.get(), &m_collector);
  return text.parse();
}

void SoftBookParser::createTextStream()
{
  assert(bool(m_resources));

  const shared_ptr<librevenge::RVNGInputStream> textStream = getFileStream(SOFTBOOK_TEXT_FILE_NAME);

  if (m_header.getCompressed())
  {
    const shared_ptr<librevenge::RVNGInputStream> compressionInfo = getFileStream("!!cm");
    const Resource cm("!!cm", compressionInfo, m_header);
    unsigned windowSize = 14;
    unsigned lookAheadSize = 3;
    unsigned textLength = 0;

    for (unsigned i = 0; i != cm.getRecordCount(); ++i)
    {
      const unsigned id = cm.getRecordId(i);
      if (0x64 == id)
      {
        const shared_ptr<librevenge::RVNGInputStream> record = cm.getRecord(i);
        skip(record, 6);
        windowSize = readU16(record, true);
        lookAheadSize = readU16(record, true);
        break;
      }
      else if (0x65 == id)
      {
        const shared_ptr<librevenge::RVNGInputStream> record = cm.getRecord(i);
        seek(record, getRemainingLength(record) - 10);
        textLength = readU32(record, true);
      }
      else
      {
        EBOOK_DEBUG_MSG(("unknown resource ID %x\n", id));
      }
    }

    m_text.reset(new SoftBookLZSSStream(textStream.get(), makeLZSSConfiguration(windowSize, lookAheadSize, textLength)));
  }
  else
    m_text = textStream;
}

shared_ptr<librevenge::RVNGInputStream> SoftBookParser::getFileStream(const char *const name) const
{
  const shared_ptr<librevenge::RVNGInputStream> stream(m_resources->getSubStreamByName(name));
  if (!stream)
  {
    EBOOK_DEBUG_MSG(("resource %s not found\n", name));
    throw SoftBookParserException();
  }

  // skip file header
  // TODO: check that the resource type matches name
  skip(stream, 20);

  return stream;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
