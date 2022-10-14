/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef EBOOKSTREAMVIEW_H_INCLUDED
#define EBOOKSTREAMVIEW_H_INCLUDED

#include <librevenge-stream/librevenge-stream.h>

namespace libebook
{

/** A stream representing a view into an another stream.
  *
  * The purpose of this class is only to simplify reading of chunks of
  * data from the original stream. All reading operations change the
  * original stream. That means that it is not advisable to use the
  * original stream while a view into it is in use too.
  */
class EBOOKStreamView : public librevenge::RVNGInputStream
{
  // -Weffc++
  EBOOKStreamView(const EBOOKStreamView &other);
  EBOOKStreamView &operator=(const EBOOKStreamView &other);

public:
  EBOOKStreamView(librevenge::RVNGInputStream *stream, long begin, long end);
  ~EBOOKStreamView() override;

  bool isStructured() override;
  unsigned subStreamCount() override;
  const char *subStreamName(unsigned id) override;
  bool existsSubStream(const char *name) override;
  librevenge::RVNGInputStream *getSubStreamByName(const char *name) override;
  RVNGInputStream *getSubStreamById(unsigned id) override;

  const unsigned char *read(unsigned long numBytes, unsigned long &numBytesRead) override;
  int seek(long offset, librevenge::RVNG_SEEK_TYPE seekType) override;
  long tell() override;
  bool isEnd() override;

private:
  librevenge::RVNGInputStream *const m_stream;
  const long m_begin;
  const long m_end;
};

}

#endif // EBOOKSTREAMVIEW_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
