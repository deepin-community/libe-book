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

#include <librevenge-stream/librevenge-stream.h>

#include "libebook_utils.h"
#include "SoftBookHeader.h"

namespace libebook
{

namespace
{

const char *const SOFTBOOK_SIGNATURE = "BOOKDOUG";

struct InvalidHeaderException
{
  InvalidHeaderException();
};

InvalidHeaderException::InvalidHeaderException()
{
  EBOOK_DEBUG_MSG(("Throwing InvalidHeaderException\n"));
}

void checkOrThrow(const bool condition)
{
  if (!condition)
    throw InvalidHeaderException();
}

}

SoftBookHeader::SoftBookHeader(librevenge::RVNGInputStream *const input)
  : m_version(0)
  , m_colorMode(SOFTBOOK_COLOR_MODE_UNKNOWN)
  , m_files(0)
  , m_dirNameLength(0)
  , m_remainingBytes(0)
  , m_compressed(false)
  , m_encrypted(false)
  , m_metadata()
{
  readHeader(input);
  readBookProperties(input);
}

std::shared_ptr<SoftBookHeader> SoftBookHeader::create(librevenge::RVNGInputStream *const input)
{
  std::shared_ptr<SoftBookHeader> header;
  try
  {
    header.reset(new SoftBookHeader(input));
  }
  catch (const InvalidHeaderException &)
  {
    // no action necessary
  }

  return header;
}

unsigned SoftBookHeader::getVersion() const
{
  return m_version;
}

SoftBookColorMode SoftBookHeader::getColorMode() const
{
  return m_colorMode;
}

unsigned SoftBookHeader::getFileCount() const
{
  return m_files;
}

bool SoftBookHeader::getCompressed() const
{
  return m_compressed;
}

bool SoftBookHeader::getEncrypted() const
{
  return m_encrypted;
}

const SoftBookMetadata &SoftBookHeader::getMetadata() const
{
  return m_metadata;
}

unsigned SoftBookHeader::getTOCOffset() const
{
  return 24 + m_remainingBytes + m_dirNameLength;
}

void SoftBookHeader::readHeader(librevenge::RVNGInputStream *const input)
{
  assert(0 == input->tell());

  m_version = readU16(input, true);
  checkOrThrow((1 == m_version) || (2 == m_version));

  const unsigned char *const signature = readNBytes(input, 8);
  checkOrThrow(std::equal(signature, signature + 8, SOFTBOOK_SIGNATURE));

  skip(input, 8);
  m_files = readU16(input, true);
  m_dirNameLength = readU16(input, true);
  m_remainingBytes = readU16(input, true);
  skip(input, 8);

  const uint32_t compression = readU32(input, true);
  switch (compression)
  {
  case 0 :
    break;
  case 1 :
    m_compressed = true;
    break;
  default :
    throw InvalidHeaderException();
  }

  const uint32_t encryption = readU32(input, true);
  switch (encryption)
  {
  case 0 :
    break;
  case 2 :
    m_encrypted = true;
    break;
  default :
    throw InvalidHeaderException();
  }

  const uint32_t flags = readU32(input, true);
  m_colorMode = static_cast<SoftBookColorMode>((flags & (0x3 << 4)) >> 4);

  skip(input, 4);

  assert(48 == input->tell());
}

void SoftBookHeader::readBookProperties(librevenge::RVNGInputStream *const input)
{
  m_metadata.id = readCString(input);
  m_metadata.category = readCString(input);
  m_metadata.subcategory = readCString(input);
  m_metadata.title = readCString(input);
  m_metadata.lastName = readCString(input);
  m_metadata.middleName = readCString(input);
  m_metadata.firstName = readCString(input);

  checkOrThrow(input->tell() == 24 + static_cast<long>(m_remainingBytes));
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
