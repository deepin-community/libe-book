/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef EBOOKMEMORYSTREAM_H_INCLUDED
#define EBOOKMEMORYSTREAM_H_INCLUDED

#include <memory>

#include <librevenge-stream/librevenge-stream.h>

namespace libebook
{

class EBOOKMemoryStream : public librevenge::RVNGInputStream
{
// disable copying
  EBOOKMemoryStream(const EBOOKMemoryStream &other);
  EBOOKMemoryStream &operator=(const EBOOKMemoryStream &other);

public:
  EBOOKMemoryStream();
  EBOOKMemoryStream(const unsigned char *data, unsigned length);
  ~EBOOKMemoryStream() override;

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
  std::unique_ptr<unsigned char[]> m_data;
  const long m_length;
  long m_pos;
};

}

#endif // EBOOKMEMORYSTREAM_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
