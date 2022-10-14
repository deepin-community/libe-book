/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <cassert>
#include <string>
#include <vector>

#include <librevenge-stream/librevenge-stream.h>

#include "libebook_utils.h"
#include "EBOOKStreamView.h"
#include "PDBParser.h"

using std::unique_ptr;

namespace libebook
{

namespace
{

struct HeaderData
{
  HeaderData();

  std::string m_name;
  unsigned m_version;
  unsigned m_appInfoID;
  unsigned m_sortInfoID;
  unsigned m_type;
  unsigned m_creator;
  unsigned m_nextRecordListID;
  unsigned m_numberOfRecords;
  std::vector<unsigned> m_recordOffsets;
};

}

namespace
{

HeaderData::HeaderData()
  : m_name()
  , m_version(0)
  , m_appInfoID(0)
  , m_sortInfoID(0)
  , m_type(0)
  , m_creator(0)
  , m_nextRecordListID(0)
  , m_numberOfRecords(0)
  , m_recordOffsets()
{
}

}

struct PDBParserImpl
{
  PDBParserImpl(librevenge::RVNGInputStream *input, librevenge::RVNGTextInterface *document);

  HeaderData m_header;
  librevenge::RVNGInputStream *m_input;
  librevenge::RVNGTextInterface *m_document;

private:
// disable copying
  PDBParserImpl(const PDBParserImpl &other);
  PDBParserImpl &operator=(const PDBParserImpl &other);
};

PDBParserImpl::PDBParserImpl(librevenge::RVNGInputStream *const input, librevenge::RVNGTextInterface *const document)
  : m_header()
  , m_input(input)
  , m_document(document)
{
}

PDBParser::PDBParser(librevenge::RVNGInputStream *const input, librevenge::RVNGTextInterface *const document,
                     const unsigned type, const unsigned creator)
  : m_impl(new PDBParserImpl(input, document))
{
  readHeader();

  if ((type != m_impl->m_header.m_type) || (creator != m_impl->m_header.m_creator))
    throw UnsupportedFormat();

  if ((0 == m_impl->m_header.m_numberOfRecords)
      || (m_impl->m_header.m_recordOffsets.size() != m_impl->m_header.m_numberOfRecords))
    throw UnsupportedFormat();
}

PDBParser::~PDBParser()
{
}

bool PDBParser::parse()
{
  if (m_impl->m_header.m_appInfoID)
  {
    // TODO: implement me
  }
  if (m_impl->m_header.m_sortInfoID)
  {
    // TODO: implement me
  }

  {
    std::unique_ptr<librevenge::RVNGInputStream> input(getRecordStream(0));
    readIndexRecord(input.get());
  }

  readDataRecords();

  return true;
}

librevenge::RVNGTextInterface *PDBParser::getDocument() const
{
  return m_impl->m_document;
}

const char *PDBParser::getName() const
{
  return m_impl->m_header.m_name.c_str();
}

librevenge::RVNGInputStream *PDBParser::getAppInfoRecord() const
{
  // TODO: implement me
  return nullptr;
}

librevenge::RVNGInputStream *PDBParser::getIndexRecord() const
{
  return getRecordStream(0);
}

unsigned PDBParser::getDataRecordCount() const
{
  return m_impl->m_header.m_numberOfRecords - 1;
}

librevenge::RVNGInputStream *PDBParser::getDataRecord(unsigned n) const
{
  return getRecordStream(n + 1);
}

librevenge::RVNGInputStream *PDBParser::getDataRecords() const
{
  const auto begin = (long) m_impl->m_header.m_recordOffsets[1];
  m_impl->m_input->seek(0, librevenge::RVNG_SEEK_END);
  const long end = m_impl->m_input->tell();

  return new EBOOKStreamView(m_impl->m_input, begin, end);
}

librevenge::RVNGInputStream *PDBParser::getDataRecords(unsigned first, unsigned last) const
{
  if (first >= last)
    return nullptr;
  if ((m_impl->m_header.m_numberOfRecords - 1) < last)
    return nullptr;

  const auto begin = (long) m_impl->m_header.m_recordOffsets[first + 1];
  long end = 0;
  if ((m_impl->m_header.m_numberOfRecords - 1) == last) // ends with the last record
  {
    m_impl->m_input->seek(0, librevenge::RVNG_SEEK_END);
    end = m_impl->m_input->tell();
  }
  else
    end = (long) m_impl->m_header.m_recordOffsets[last + 1];

  return new EBOOKStreamView(m_impl->m_input, begin, end);
}

void PDBParser::readDataRecords()
{
  for (unsigned i = 1; i != m_impl->m_header.m_numberOfRecords; ++i)
  {
    unique_ptr<librevenge::RVNGInputStream> record(getRecordStream(i));
    readDataRecord(record.get(), m_impl->m_header.m_numberOfRecords - 1 == i);
  }
}

void PDBParser::readHeader()
{
  m_impl->m_input->seek(0, librevenge::RVNG_SEEK_SET);

  char name[32];
  unsigned nameLen = 0;
  while (nameLen != 32)
  {
    unsigned char c = readU8(m_impl->m_input);
    name[nameLen++] = (char) c;
    if (0 == c)
      break;
  }
  // We read the whole name, but there was no terminating 0. Either a
  // broken file or not a PDB file at all...
  if (nameLen == 32)
    --nameLen;
  name[nameLen] = 0;
  m_impl->m_header.m_name.assign(name, nameLen);

  // skip the unused bytes of name and ignore 2 bytes of attributes
  m_impl->m_input->seek(34, librevenge::RVNG_SEEK_SET);
  m_impl->m_header.m_version = readU16(m_impl->m_input, true);
  // ignore dates, modification number and IDs; 6*4 = 24 bytes
  skip(m_impl->m_input, 24);
  m_impl->m_header.m_type = readU32(m_impl->m_input, true);
  m_impl->m_header.m_creator = readU32(m_impl->m_input, true);
  skip(m_impl->m_input, 4); // skip uniqueIDseed
  m_impl->m_header.m_nextRecordListID = readU32(m_impl->m_input, true);
  assert(m_impl->m_input->tell() == 76);
  m_impl->m_header.m_numberOfRecords = readU16(m_impl->m_input, true);
  if (m_impl->m_header.m_numberOfRecords > getRemainingLength(m_impl->m_input) / 8)
    m_impl->m_header.m_numberOfRecords = getRemainingLength(m_impl->m_input) / 8;

  // read records
  for (unsigned i = 0; i != m_impl->m_header.m_numberOfRecords; ++i)
  {
    m_impl->m_header.m_recordOffsets.push_back(readU32(m_impl->m_input, true));
    skip(m_impl->m_input, 4); // skip the uninteresting remainder
  }
}

librevenge::RVNGInputStream *PDBParser::getRecordStream(const unsigned n) const
{
  if (n >= m_impl->m_header.m_numberOfRecords)
    return nullptr;

  const auto begin = (long) m_impl->m_header.m_recordOffsets[n];
  long end = 0;
  if ((m_impl->m_header.m_numberOfRecords - 1) == n) // the last record
  {
    m_impl->m_input->seek(0, librevenge::RVNG_SEEK_END);
    end = m_impl->m_input->tell();
  }
  else
    end = (long) m_impl->m_header.m_recordOffsets[n + 1];

  return new EBOOKStreamView(m_impl->m_input, begin, end);
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
