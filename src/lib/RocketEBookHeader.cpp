/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * For further information visit http://libebook.sourceforge.net
 */

#include <cassert>

#include "libebook_xml.h"
#include "RocketEBookHeader.h"

#define ROCKETEBOOK_CODE(s) ((s[0] << 24) | (s[1] << 16) | (s[2] << 8) | s[3])

using std::size_t;

namespace libebook
{

namespace
{

static const unsigned SIGNATURE = 0x0cb00cb0;
static const unsigned SIGNATURE2 = ROCKETEBOOK_CODE("NUVO");

size_t strnlen(const char *s, const size_t n)
{
  size_t len = 0;
  for (; (n != len) && s; ++len, ++s)
    ;
  return len;
}

}

RocketEBookHeader::Entry::Entry()
  : name()
  , length(0)
  , offset(0)
  , flags(0)
{
}

RocketEBookHeader::RocketEBookHeader(const RVNGInputStreamPtr_t &input)
  : m_version(0)
  , m_tocOffset(0)
  , m_length(0)
  , m_dir()
  , m_info(0)
{
  assert(bool(input));
  assert(0 == input->tell());

  readHeader(input);
  readDirectory(input);
}

unsigned RocketEBookHeader::getInfoID() const
{
  return m_info;
}

const RocketEBookHeader::Directory_t &RocketEBookHeader::getDirectory() const
{
  return m_dir;
}

void RocketEBookHeader::readHeader(const RVNGInputStreamPtr_t &input)
{
  const unsigned sig = readU32(input);
  if (SIGNATURE != sig)
    throw UnsupportedFormat();

  m_version = readU16(input);
  if (2 != m_version)
    throw UnsupportedFormat();

  const unsigned sig2 = readU32(input);
  if ((SIGNATURE2 != sig2) && (0 != sig2)) // some files have 0 here
    throw UnsupportedFormat();

  skip(input, 4 + 4 + 6);
  assert(0x18 == input->tell());

  m_tocOffset = readU32(input);
  m_length = readU32(input);

  if ((getRemainingLength(input) != m_length) || (m_length < m_tocOffset))
    throw ParseError();
}

void RocketEBookHeader::readDirectory(const RVNGInputStreamPtr_t &input)
{
  seek(input, m_tocOffset);

  const unsigned entries = readU32(input);
  if (0 == entries)
    throw PackageError();

  for (unsigned i = 0; entries != i; ++i)
  {
    Entry entry;
    const char *const name = char_cast(readNBytes(input, 32));
    entry.name.assign(name, strnlen(name, 32));
    entry.length = readU32(input);
    entry.offset = readU32(input);
    entry.flags = readU32(input);

    if ((entry.offset + entry.length) >= m_length)
      throw PackageError();
    if (entry.flags & FLAG_ENCRYPTED)
      throw UnsupportedEncryption();

    m_dir.push_back(entry);
  }
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
