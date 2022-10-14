/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef EBOOKBITSTREAM_H_INCLUDED
#define EBOOKBITSTREAM_H_INCLUDED

#include "libebook_utils.h"


namespace libebook
{

class EBOOKBitStream
{
public:
  explicit EBOOKBitStream(librevenge::RVNGInputStream *stream);

  uint32_t read(uint8_t numberOfBits, bool bigEndian = false);

  bool isEnd() const;
  bool atLastByte() const;

private:
  void fill();

  /** Reads a byte from the stream.
    *
    * @return a byte
    */
  uint8_t readByte();

  /** Reads a given amount of bits from the stream.
    *
    * @arg[in] numberOfBits the number of bits to read. Has to be less
    * than or equal to 8.
    *
    * @return a byte containing the bits
    */
  uint8_t readBits(uint8_t numberOfBits);

  uint8_t readAvailableBits(uint8_t numberOfBits);

private:
  static const uint8_t s_masks[8];

private:
  librevenge::RVNGInputStream *const m_stream;
  uint8_t m_current;
  uint8_t m_available;
};

}

#endif // EBOOKBITSTREAM_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
