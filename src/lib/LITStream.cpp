/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * For further information visit http://libebook.sourceforge.net
 */

#include "LITStream.h"

using librevenge::RVNGInputStream;

namespace libebook
{

struct LITStream::Impl
{
  const RVNGInputStreamPtr_t stream;

  Impl(const RVNGInputStreamPtr_t &input);
};

LITStream::Impl::Impl(const RVNGInputStreamPtr_t &input)
  : stream(input)
{
}

LITStream::LITStream(const RVNGInputStreamPtr_t &stream)
  : m_impl(new Impl(stream))
{
}

LITStream::~LITStream()
{
}

bool LITStream::isStructured()
{
  return true;
}

unsigned LITStream::subStreamCount()
{
  // TODO: implement me
  return 0;
}

const char *LITStream::subStreamName(const unsigned id)
{
  // TODO: implement me
  (void) id;
  return 0;
}

bool LITStream::existsSubStream(const char *const name)
{
  // TODO: implement me
  (void) name;
  return 0;
}

librevenge::RVNGInputStream *LITStream::getSubStreamByName(const char *name)
{
  // TODO: implement me
  (void) name;
  return 0;
}

librevenge::RVNGInputStream *LITStream::getSubStreamById(const unsigned id)
{
  // TODO: implement me
  (void) id;
  return 0;
}

const unsigned char *LITStream::read(const unsigned long numBytes, unsigned long &numBytesRead)
{
  return m_impl->stream->read(numBytes, numBytesRead);
}

int LITStream::seek(const long offset, const librevenge::RVNG_SEEK_TYPE seekType)
{
  return m_impl->stream->seek(offset, seekType);
}

long LITStream::tell()
{
  return m_impl->stream->tell();
}

bool LITStream::isEnd()
{
  return m_impl->stream->isEnd();
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
