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
#include <memory>
#include <vector>

#include "libebook_utils.h"
#include "EBOOKBitStream.h"
#include "EBOOKMemoryStream.h"
#include "SoftBookLZSSStream.h"

using std::vector;

namespace libebook
{

namespace
{

class SlidingWindow
{
  // -Weffc++
  SlidingWindow(const SlidingWindow &other);
  SlidingWindow &operator=(const SlidingWindow &other);

public:
  SlidingWindow(size_t size, unsigned char fill, unsigned fillPos, bool allowOverflow);

  ~SlidingWindow();

  void push(unsigned char byte);

  void copyOut(size_t offset, size_t length, vector<unsigned char> &out);

private:
  void push(unsigned char *bytes, size_t length);

  void advance();
  void advance(size_t &pos, size_t increment = 1);

private:
  const std::unique_ptr<unsigned char[]> m_data;
  const size_t m_size;
  size_t m_begin;
  size_t m_end;
  bool m_growing;
  bool m_allowOverflow;
};

SlidingWindow::SlidingWindow(const size_t size, const unsigned char fill, const unsigned fillPos, const bool allowOverflow)
  : m_data(new unsigned char[size])
  , m_size(size)
  , m_begin(0)
  , m_end(fillPos)
  , m_growing(true)
  , m_allowOverflow(allowOverflow)
{
  std::fill_n(m_data.get(), m_size, fill);
}

SlidingWindow::~SlidingWindow()
{
}

void SlidingWindow::push(const unsigned char byte)
{
  m_data[m_end] = byte;
  advance();
}

void SlidingWindow::copyOut(const size_t offset, const size_t length, vector<unsigned char> &out)
{
  size_t pos = m_begin;
  advance(pos, offset);

  if (m_growing && !m_allowOverflow && ((pos + length) > m_end))
  {
    for (size_t i = 0; i != length; ++i)
      out.push_back(m_data[pos]);
  }
  else
  {
    for (size_t i = 0; i != length; ++i)
    {
      out.push_back(m_data[pos]);
      advance(pos);
    }
  }

  const size_t begin = out.size() - length;
  push(&out[begin], length);
}

void SlidingWindow::push(unsigned char *const bytes, const size_t length)
{
  for (size_t i = 0; i != length; ++i)
  {
    m_data[m_end] = bytes[i];
    advance();
  }
}

void SlidingWindow::advance()
{
  advance(m_end, 1);
  if (m_end == m_begin)
    m_growing = false;
  if (!m_growing)
    advance(m_begin, 1);
}

void SlidingWindow::advance(size_t &pos, const size_t increment)
{
  if (0 == increment)
    return;

  pos = (pos + increment) % m_size;
}

}

namespace
{

struct LZSSCompressionException {};

struct LZSSException {};

void unpack(librevenge::RVNGInputStream *const stream, const SoftBookLZSSStream::Configuration &configuration, vector<unsigned char> &buffer)
{
  // the window size can normally be represented in 16 bits or less
  assert(24 > configuration.offsetBits);

  EBOOKBitStream bitStream(stream);

  SlidingWindow window(1 << configuration.offsetBits, (unsigned char)configuration.fillChar, configuration.fillPos, configuration.allowOverflow);

  const unsigned maxLength = configuration.uncompressedLength;
  const bool bigEndian = configuration.bigEndian;

  while ((0 < maxLength) ? (buffer.size() < maxLength) : !bitStream.atLastByte())
  {
    const bool encoded = 0 == bitStream.read(1, bigEndian);
    if (encoded)
    {
      const uint32_t offset = bitStream.read((uint8_t) configuration.offsetBits, bigEndian);
      const uint32_t length = bitStream.read((uint8_t) configuration.lengthBits, bigEndian) + 3;
      window.copyOut(offset, length, buffer);
    }
    else
    {
      const auto c = static_cast<unsigned char>(bitStream.read(8, bigEndian));
      buffer.push_back(c);
      window.push(c);
    }
  }
}

}

SoftBookLZSSStream::Configuration::Configuration()
  : offsetBits(12)
  , lengthBits(4)
  , uncompressedLength(0)
  , fillPos(0)
  , fillChar(' ')
  , allowOverflow(true)
  , bigEndian(false)
{
}

SoftBookLZSSStream::SoftBookLZSSStream(librevenge::RVNGInputStream *const stream, const Configuration &configuration)
  : m_stream()
{
  assert(stream);

  vector<unsigned char> unpacked;
  unpack(stream, configuration, unpacked);

  if (unpacked.empty())
    throw LZSSException();

  m_stream.reset(new EBOOKMemoryStream(&unpacked[0], (unsigned) unpacked.size()));
}

SoftBookLZSSStream::~SoftBookLZSSStream()
{
}

bool SoftBookLZSSStream::isStructured()
{
  return false;
}

unsigned SoftBookLZSSStream::subStreamCount()
{
  return 0;
}

const char *SoftBookLZSSStream::subStreamName(unsigned)
{
  return nullptr;
}

bool SoftBookLZSSStream::existsSubStream(const char *)
{
  return false;
}

librevenge::RVNGInputStream *SoftBookLZSSStream::getSubStreamByName(const char *)
{
  return nullptr;
}

librevenge::RVNGInputStream *SoftBookLZSSStream::getSubStreamById(unsigned)
{
  return nullptr;
}

const unsigned char *SoftBookLZSSStream::read(const unsigned long numBytes, unsigned long &numBytesRead)
{
  return m_stream->read(numBytes, numBytesRead);
}

int SoftBookLZSSStream::seek(long offset, const librevenge::RVNG_SEEK_TYPE seekType)
{
  return m_stream->seek(offset, seekType);
}

long SoftBookLZSSStream::tell()
{
  return m_stream->tell();
}

bool SoftBookLZSSStream::isEnd()
{
  return m_stream->isEnd();
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
