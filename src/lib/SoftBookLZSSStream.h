/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * For further information visit http://libebook.sourceforge.net
 */

#ifndef SOFTBOOKLZSSSTREAM_H_INCLUDED
#define SOFTBOOKLZSSSTREAM_H_INCLUDED

#include <memory>

#include <librevenge-stream/librevenge-stream.h>

namespace libebook
{

class SoftBookLZSSStream : public librevenge::RVNGInputStream
{
// disable copying
  SoftBookLZSSStream(const SoftBookLZSSStream &other);
  SoftBookLZSSStream &operator=(const SoftBookLZSSStream &other);

public:
  struct Configuration
  {
    unsigned offsetBits;
    unsigned lengthBits;
    unsigned uncompressedLength;
    unsigned fillPos;
    char fillChar;
    bool allowOverflow;
    bool bigEndian;

    Configuration();
  };

public:
  explicit SoftBookLZSSStream(librevenge::RVNGInputStream *stream, const Configuration &configuration = Configuration());
  ~SoftBookLZSSStream() override;

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
  std::unique_ptr<librevenge::RVNGInputStream> m_stream;
};

}

#endif // SOFTBOOKLZSSSTREAM_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
