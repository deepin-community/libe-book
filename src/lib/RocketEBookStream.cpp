/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * For further information visit http://libebook.sourceforge.net
 */

#include <algorithm>
#include <deque>
#include <numeric>
#include <vector>

#include "EBOOKMemoryStream.h"
#include "EBOOKStreamView.h"
#include "EBOOKZlibStream.h"
#include "RocketEBookHeader.h"
#include "RocketEBookStream.h"

using librevenge::RVNGInputStream;

using std::deque;
using std::vector;

namespace libebook
{

namespace
{

void uncompress(RVNGInputStream *stream, vector<unsigned char> &data)
{
  const unsigned count = readU32(stream);
  const unsigned length = readU32(stream);

  deque<unsigned> lengths;
  for (unsigned i = 0; count != i; ++i)
    lengths.push_back(readU32(stream));

  data.reserve(length);

  unsigned long offset = stream->tell();
  for (deque<unsigned>::const_iterator it = lengths.begin(); lengths.end() != it; ++it)
  {
    EBOOKStreamView compressed(stream, offset, *it);
    EBOOKZlibStream uncompressed(&compressed);
    const unsigned uncompressedLen = getRemainingLength(&uncompressed);
    const unsigned char *const uncompressedData = readNBytes(&uncompressed, uncompressedLen);
    data.insert(data.end(), uncompressedData, uncompressedData + uncompressedLen);

    offset += *it;
    if (static_cast<unsigned long>(stream->tell()) != offset)
      seek(stream, offset);
  }

  if ((data.size() != length) || !stream->isEnd())
    throw ParseError();
}

RVNGInputStream *makeSubStream(const RVNGInputStreamPtr_t &stream, const RocketEBookHeader::Entry &entry)
{
  if (RocketEBookHeader::FLAG_DEFLATED & entry.flags)
  {
    vector<unsigned char> data;
    EBOOKStreamView subStream(stream.get(), entry.offset, entry.length);
    uncompress(&subStream, data);
    return new EBOOKMemoryStream(&data[0], data.size());
  }
  else
  {
    seek(stream, entry.offset);
    const unsigned char *const data = readNBytes(stream, entry.length);
    return new EBOOKMemoryStream(data, entry.length);
  }

  return 0;
}

struct MatchName
{
  MatchName(const char *const name)
    : m_name(name)
  {
  }

  bool operator()(const RocketEBookHeader::Entry &entry)
  {
    return m_name == entry.name;
  }

private:
  const char *const m_name;
};

}

struct RocketEBookStream::Impl
{
  const RVNGInputStreamPtr_t stream;
  const RocketEBookHeader::Directory_t dir;

  Impl(const RVNGInputStreamPtr_t &input, const RocketEBookHeader::Directory_t &directory);
};

RocketEBookStream::Impl::Impl(const RVNGInputStreamPtr_t &input, const RocketEBookHeader::Directory_t &directory)
  : stream(input)
  , dir(directory)
{
}

RocketEBookStream::RocketEBookStream(const RVNGInputStreamPtr_t &stream, const RocketEBookHeader &header)
  : m_impl(new Impl(stream, header.getDirectory()))
{
}

RocketEBookStream::~RocketEBookStream()
{
}

bool RocketEBookStream::isStructured()
{
  return true;
}

unsigned RocketEBookStream::subStreamCount()
{
  return m_impl->dir.size();
}

const char *RocketEBookStream::subStreamName(const unsigned id)
{
  if (m_impl->dir.size() <= id)
    return 0;
  return m_impl->dir[id].name.c_str();
}

bool RocketEBookStream::existsSubStream(const char *const name)
{
  return m_impl->dir.end() != find_if(m_impl->dir.begin(), m_impl->dir.end(), MatchName(name));
}

librevenge::RVNGInputStream *RocketEBookStream::getSubStreamByName(const char *name)
{
  const RocketEBookHeader::Directory_t::const_iterator it = find_if(m_impl->dir.begin(), m_impl->dir.end(), MatchName(name));
  if (m_impl->dir.end() != it)
    return makeSubStream(m_impl->stream, *it);
  return 0;
}

librevenge::RVNGInputStream *RocketEBookStream::getSubStreamById(const unsigned id)
{
  if (m_impl->dir.size() > id)
    return makeSubStream(m_impl->stream, m_impl->dir[id]);
  return 0;
}

const unsigned char *RocketEBookStream::read(const unsigned long numBytes, unsigned long &numBytesRead)
{
  return m_impl->stream->read(numBytes, numBytesRead);
}

int RocketEBookStream::seek(const long offset, const librevenge::RVNG_SEEK_TYPE seekType)
{
  return m_impl->stream->seek(offset, seekType);
}

long RocketEBookStream::tell()
{
  return m_impl->stream->tell();
}

bool RocketEBookStream::isEnd()
{
  return m_impl->stream->isEnd();
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
