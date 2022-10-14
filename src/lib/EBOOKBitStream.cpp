/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <cassert>

#include <librevenge-stream/librevenge-stream.h>

#include "EBOOKBitStream.h"

namespace libebook
{

const uint8_t EBOOKBitStream::s_masks[] = { 0x1, 0x3, 0x7, 0xf, 0x1f, 0x3f, 0x7f, 0xff };

EBOOKBitStream::EBOOKBitStream(librevenge::RVNGInputStream *const stream)
  : m_stream(stream)
  , m_current(0)
  , m_available(0)
{
}

uint32_t EBOOKBitStream::read(uint8_t numberOfBits, const bool bigEndian)
{
  assert(numberOfBits <= 8 * sizeof(uint32_t));

  if (numberOfBits == 0)
    return 0;

  uint8_t p[] = { 0, 0, 0, 0 };

  if (bigEndian)
  {
    int i = (numberOfBits - 1) / 8;

    const uint8_t over = numberOfBits % 8;
    if (0 < over)
    {
      p[i] = readBits(over);
      numberOfBits = uint8_t(numberOfBits-over);
      --i;
    }
    assert(0 == numberOfBits % 8);

    while (8 <= numberOfBits)
    {
      p[i] = readByte();
      numberOfBits = uint8_t(numberOfBits-8);
      --i;
    }
    assert(0 == numberOfBits);
  }
  else
  {
    int i = 0;

    while (8 <= numberOfBits)
    {
      p[i] = readByte();
      numberOfBits = uint8_t(numberOfBits-8);
      ++i;
    }
    assert(8 > numberOfBits);

    if (0 < numberOfBits)
      p[i] = readBits(numberOfBits);
  }

  return (uint32_t)p[0]|((uint32_t)p[1]<<8)|((uint32_t)p[2]<<16)|((uint32_t)p[3]<<24);
}

bool EBOOKBitStream::isEnd() const
{
  return atLastByte() && (0 == m_available);
}

bool EBOOKBitStream::atLastByte() const
{
  if (m_stream->isEnd())
    return true;

  const_cast<EBOOKBitStream *>(this)->fill();
  return m_stream->isEnd();
}

void EBOOKBitStream::fill()
{
  if (0 == m_available)
  {
    m_current = readU8(m_stream);
    m_available = 8;
  }
  assert(m_available > 0);
}

uint8_t EBOOKBitStream::readByte()
{
  return readBits(8);
}

uint8_t EBOOKBitStream::readBits(uint8_t numberOfBits)
{
  assert(numberOfBits <= 8);

  if (numberOfBits == 0)
    return 0;

  uint8_t value = 0;

  fill();

  if (numberOfBits <= m_available)
    value = readAvailableBits(numberOfBits);
  else
  {
    numberOfBits = uint8_t(numberOfBits-m_available);
    value = readAvailableBits(m_available);
    fill();
    value = uint8_t(value<<numberOfBits);
    value = uint8_t(value|readAvailableBits(numberOfBits));
  }

  return value;
}

uint8_t EBOOKBitStream::readAvailableBits(const uint8_t numberOfBits)
{
  assert(numberOfBits <= m_available);

  uint8_t current = m_current;
  if (numberOfBits < m_available)
    current =  uint8_t (current >> (m_available - numberOfBits));
  m_available = uint8_t(m_available - numberOfBits);

  return s_masks[numberOfBits - 1] & current;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
