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

#include "libebook_utils.h"
#include "EBOOKMemoryStream.h"
#include "PDBLZ77Stream.h"

using std::vector;

namespace libebook
{

namespace
{

void unpack(librevenge::RVNGInputStream *const stream, vector<unsigned char> &buffer)
{
  while (!stream->isEnd())
  {
    const unsigned char c = readU8(stream);

    if ((c == 0x0) || ((c >= 0x9) && (c <= 0x7f)))
    {
      buffer.push_back(c);
    }
    else if ((c >= 0x1) && (c <= 0x8))
    {
      if (stream->isEnd()) // there is not enough bytes remaining
        throw GenericException();         // in the current record

      for (unsigned char i = 0; i != c; ++i)
        buffer.push_back(readU8(stream));
    }
    else if ((c >= 0x80) && (c <= 0xbf))
    {
      if (stream->isEnd()) // it's not possible to read another byte
        throw GenericException();    // from the current record

      const unsigned byte1 = c & 0x3f; // drop the leftmost 2 bits
      const unsigned byte2 = readU8(stream);

      const unsigned combined = (byte1 << 8) | byte2;
      // combined contains 14 valid bits. Split them to 11 bits of
      // distance and 3 bits of length.
      const unsigned distance = (combined & 0xfff8) >> 3;
      const unsigned length = (combined & 0x7) + 3;

      // TODO: It's probably better idea to just ignore this and
      // continue reading with the next byte. The worst that can
      // happen is a missing piece of text.
      if (distance > buffer.size()) // reading before this record
        throw GenericException();
      if (0 == distance)
        throw GenericException();

      if (length <= distance)
      {
        for (vector<unsigned char>::size_type i = buffer.size() - distance, last = i + length; i != last; ++i)
          buffer.push_back(buffer[i]);
      }
      else
      {
        // Apparently this is sometimes misused to construct sequences
        // of repeated characters, like ....
        const char repeated = (char) *(buffer.end() - (int) distance);
        buffer.insert(buffer.end(), length, (unsigned char) repeated);
      }
    }
    else
    {
      buffer.push_back(' ');
      buffer.push_back(c ^ 0x80);
    }
  }
}

}

PDBLZ77Stream::PDBLZ77Stream(librevenge::RVNGInputStream *const stream)
  : m_stream()
{
  assert(stream);

  if (0 != stream->seek(0, librevenge::RVNG_SEEK_SET))
    throw EndOfStreamException();

  vector<unsigned char> unpacked;
  unpack(stream, unpacked);

  if (unpacked.empty())
    throw GenericException();

  m_stream.reset(new EBOOKMemoryStream(&unpacked[0], (unsigned) unpacked.size()));
}

PDBLZ77Stream::~PDBLZ77Stream()
{
}

bool PDBLZ77Stream::isStructured()
{
  return false;
}

unsigned PDBLZ77Stream::subStreamCount()
{
  return 0;
}

const char *PDBLZ77Stream::subStreamName(unsigned)
{
  return nullptr;
}

bool PDBLZ77Stream::existsSubStream(const char *)
{
  return false;
}

librevenge::RVNGInputStream *PDBLZ77Stream::getSubStreamByName(const char *)
{
  return nullptr;
}

librevenge::RVNGInputStream *PDBLZ77Stream::getSubStreamById(unsigned)
{
  return nullptr;
}

const unsigned char *PDBLZ77Stream::read(const unsigned long numBytes, unsigned long &numBytesRead)
{
  return m_stream->read(numBytes, numBytesRead);
}

int PDBLZ77Stream::seek(long offset, const librevenge::RVNG_SEEK_TYPE seekType)
{
  return m_stream->seek(offset, seekType);
}

long PDBLZ77Stream::tell()
{
  return m_stream->tell();
}

bool PDBLZ77Stream::isEnd()
{
  return m_stream->isEnd();
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
