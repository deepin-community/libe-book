/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <librevenge-stream/librevenge-stream.h>

#include "libebook_utils.h"
#include "EBOOKCharsetConverter.h"
#include "EBOOKMemoryStream.h"
#include "EBOOKUTF8Stream.h"

using std::unique_ptr;

using std::vector;

namespace libebook
{

namespace
{

class StreamException
{
};

}

EBOOKUTF8Stream::EBOOKUTF8Stream(librevenge::RVNGInputStream *const strm, EBOOKCharsetConverter *converter)
  : m_stream()
{
  if (!strm)
    throw StreamException();

  vector<char> data;

  const long begin = strm->tell();
  strm->seek(0, librevenge::RVNG_SEEK_END);
  const long end = strm->tell();
  strm->seek(begin, librevenge::RVNG_SEEK_SET);

  if (begin == end)
  {
    m_stream.reset(new EBOOKMemoryStream());
    return;
  }

  const auto bytesToRead = static_cast<unsigned long>(end - begin);
  const auto *const s = reinterpret_cast<const char *>(readNBytes(strm, bytesToRead));

  unique_ptr<EBOOKCharsetConverter> createdConverter;

  if (!converter)
  {
    createdConverter.reset(new EBOOKCharsetConverter());
    converter = createdConverter.get();
    if (!converter->guessEncoding(s, static_cast<unsigned>(bytesToRead)))
      throw StreamException();
  }

  data.reserve(static_cast<unsigned>(bytesToRead));

  if (!converter->convertBytes(s, static_cast<unsigned>(bytesToRead), data))
    throw StreamException();
  if (data.empty())
    throw StreamException();

  m_stream.reset(new EBOOKMemoryStream(reinterpret_cast<const unsigned char *>(&data[0]), static_cast<unsigned>(data.size())));
}

EBOOKUTF8Stream::~EBOOKUTF8Stream()
{
}

bool EBOOKUTF8Stream::isStructured()
{
  return m_stream->isStructured();
}

unsigned EBOOKUTF8Stream::subStreamCount()
{
  return m_stream->subStreamCount();
}

const char *EBOOKUTF8Stream::subStreamName(const unsigned id)
{
  return m_stream->subStreamName(id);
}

bool EBOOKUTF8Stream::existsSubStream(const char *)
{
  return false;
}

librevenge::RVNGInputStream *EBOOKUTF8Stream::getSubStreamByName(const char *const name)
{
  return m_stream->getSubStreamByName(name);
}

librevenge::RVNGInputStream *EBOOKUTF8Stream::getSubStreamById(const unsigned id)
{
  return m_stream->getSubStreamById(id);
}


const unsigned char *EBOOKUTF8Stream::read(unsigned long numBytes, unsigned long &numBytesRead)
{
  return m_stream->read(numBytes, numBytesRead);
}

int EBOOKUTF8Stream::seek(long offset, librevenge::RVNG_SEEK_TYPE seekType)
{
  return m_stream->seek(offset, seekType);
}

long EBOOKUTF8Stream::tell()
{
  return m_stream->tell();
}

bool EBOOKUTF8Stream::isEnd()
{
  return m_stream->isEnd();
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
