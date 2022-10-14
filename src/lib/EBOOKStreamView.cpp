/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <cassert>

#include "libebook_utils.h"
#include "EBOOKStreamView.h"

namespace libebook
{

EBOOKStreamView::EBOOKStreamView(librevenge::RVNGInputStream *const stream, const long begin, const long end)
  : m_stream(stream)
  , m_begin(begin)
  , m_end(end)
{
  if (m_end < m_begin)
    throw EndOfStreamException();
  // better to let the stream die unborn than to be inconsistent
  if ((0 != m_stream->seek(m_end, librevenge::RVNG_SEEK_SET)) || (m_stream->tell() != m_end))
    throw EndOfStreamException();
  if ((0 != m_stream->seek(m_begin, librevenge::RVNG_SEEK_SET)) || (m_stream->tell() != m_begin))
    throw EndOfStreamException();
}

EBOOKStreamView::~EBOOKStreamView()
{
}

bool EBOOKStreamView::isStructured()
{
  return false;
}

unsigned EBOOKStreamView::subStreamCount()
{
  return 0;
}

const char *EBOOKStreamView::subStreamName(unsigned)
{
  return nullptr;
}

bool EBOOKStreamView::existsSubStream(const char *)
{
  return false;
}

librevenge::RVNGInputStream *EBOOKStreamView::getSubStreamByName(const char *)
{
  return nullptr;
}

librevenge::RVNGInputStream *EBOOKStreamView::getSubStreamById(unsigned)
{
  return nullptr;
}

const unsigned char *EBOOKStreamView::read(unsigned long numBytes, unsigned long &numBytesRead)
{
  const long pos = m_stream->tell();
  assert((m_begin <= pos) && (pos <= m_end));

  if ((pos + static_cast<long>(numBytes)) > m_end)
    numBytes = static_cast<unsigned long>(m_end - pos);

  if (0 == numBytes)
  {
    numBytesRead = 0;
    return nullptr;
  }

  return m_stream->read(numBytes, numBytesRead);
}

int EBOOKStreamView::seek(long offset, const librevenge::RVNG_SEEK_TYPE seekType)
{
  const long pos = m_stream->tell();
  assert((m_begin <= pos) && (pos <= m_end));

  int retval = 0;
  switch (seekType)
  {
  case librevenge::RVNG_SEEK_SET :
    offset += m_begin;
    if ((offset < m_begin) || (offset > m_end))
      retval = 1;
    break;
  case librevenge::RVNG_SEEK_CUR :
    offset += pos;
    if ((offset < m_begin) || (offset > m_end))
      retval = 1;
    break;
  case librevenge::RVNG_SEEK_END :
    offset += m_end;
    if ((offset < m_begin) || offset > m_end)
      retval = 1;
    break;
  default :
    retval = -1;
  }

  if (retval != 0)
    return retval;

  return m_stream->seek(offset, librevenge::RVNG_SEEK_SET);
}

long EBOOKStreamView::tell()
{
  return m_stream->tell() - m_begin;
}

bool EBOOKStreamView::isEnd()
{
  return m_stream->tell() >= m_end;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
