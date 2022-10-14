/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <cassert>
#include <vector>

#include <zlib.h>

#include "libebook_utils.h"
#include "EBOOKMemoryStream.h"
#include "EBOOKZlibStream.h"

using std::vector;

namespace libebook
{

namespace
{

class ZlibStreamException
{
};

EBOOKMemoryStream *getInflatedStream(librevenge::RVNGInputStream *const input)
{
  if (0x78 != readU8(input)) // not a zlib stream
    throw ZlibStreamException();

  const bool uncompressed = Z_NO_COMPRESSION == readU8(input);

  auto begin = (unsigned long) input->tell();
  input->seek(0, librevenge::RVNG_SEEK_END);
  auto end = (unsigned long) input->tell();
  unsigned long compressedSize = end - begin;
  input->seek((long) begin, librevenge::RVNG_SEEK_SET);

  if (0 == compressedSize) // No data?!
    throw ZlibStreamException();

  unsigned long numBytesRead = 0;
  auto *compressedData = const_cast<unsigned char *>(input->read(compressedSize, numBytesRead));

  if (uncompressed)
  {
    if (numBytesRead != compressedSize)
      throw ZlibStreamException();
    return new EBOOKMemoryStream(compressedData, static_cast<unsigned>(compressedSize));
  }
  else
  {
    int ret;
    z_stream strm;

    /* allocate inflate state */
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = 0;
    strm.next_in = Z_NULL;
    ret = inflateInit2(&strm,-MAX_WBITS);
    if (ret != Z_OK)
      throw ZlibStreamException();

    strm.avail_in = (unsigned)numBytesRead;
    strm.next_in = (Bytef *)compressedData;
    strm.total_out = 0;

    vector<unsigned char> data(2 * compressedSize);

    while (true)
    {
      strm.next_out = reinterpret_cast<Bytef *>(&data[strm.total_out]);
      strm.avail_out = uInt(data.size() - strm.total_out);
      ret = inflate(&strm, Z_SYNC_FLUSH);

      if (Z_STREAM_END == ret)
        break;
      if ((Z_OK == ret) && (0 == strm.avail_in) && (0 < strm.avail_out)) // end of stream too
        break;
      if (Z_OK != ret)
      {
        (void)inflateEnd(&strm);
        throw ZlibStreamException();
      }

      data.resize(data.size() + compressedSize);
    }

    (void)inflateEnd(&strm);

    return new EBOOKMemoryStream(&data[0], (unsigned) strm.total_out);
  }
}

}

EBOOKZlibStream::EBOOKZlibStream(librevenge::RVNGInputStream *const stream)
  : m_stream()
{
  assert(stream);

  if (0 != stream->seek(0, librevenge::RVNG_SEEK_SET))
    throw EndOfStreamException();

  m_stream.reset(getInflatedStream(stream));
}

EBOOKZlibStream::~EBOOKZlibStream()
{
}

bool EBOOKZlibStream::isStructured()
{
  return false;
}

unsigned EBOOKZlibStream::subStreamCount()
{
  return 0;
}

const char *EBOOKZlibStream::subStreamName(unsigned)
{
  return nullptr;
}

bool EBOOKZlibStream::existsSubStream(const char *)
{
  return false;
}

librevenge::RVNGInputStream *EBOOKZlibStream::getSubStreamByName(const char *)
{
  return nullptr;
}

librevenge::RVNGInputStream *EBOOKZlibStream::getSubStreamById(unsigned)
{
  return nullptr;
}

const unsigned char *EBOOKZlibStream::read(const unsigned long numBytes, unsigned long &numBytesRead)
{
  return m_stream->read(numBytes, numBytesRead);
}

int EBOOKZlibStream::seek(long offset, const librevenge::RVNG_SEEK_TYPE seekType)
{
  return m_stream->seek(offset, seekType);
}

long EBOOKZlibStream::tell()
{
  return m_stream->tell();
}

bool EBOOKZlibStream::isEnd()
{
  return m_stream->isEnd();
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
