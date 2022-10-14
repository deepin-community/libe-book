/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * For further information visit http://libebook.sourceforge.net
 */

#ifndef ROCKETEBOOKSTREAM_H_INCLUDED
#define ROCKETEBOOKSTREAM_H_INCLUDED

#include <librevenge-stream/librevenge-stream.h>

#include "libebook_utils.h"

namespace libebook
{

class RocketEBookHeader;

class RocketEBookStream : public librevenge::RVNGInputStream
{
// disable copying
  RocketEBookStream(const RocketEBookStream &other);
  RocketEBookStream &operator=(const RocketEBookStream &other);

  struct Impl;

public:
  RocketEBookStream(const RVNGInputStreamPtr_t &input, const RocketEBookHeader &header);
  virtual ~RocketEBookStream();

  virtual bool isStructured();
  virtual unsigned subStreamCount();
  virtual const char *subStreamName(unsigned id);
  virtual bool existsSubStream(const char *name);
  virtual librevenge::RVNGInputStream *getSubStreamByName(const char *name);
  virtual librevenge::RVNGInputStream *getSubStreamById(unsigned id);

  virtual const unsigned char *read(unsigned long numBytes, unsigned long &numBytesRead);
  virtual int seek(long offset, librevenge::RVNG_SEEK_TYPE seekType);
  virtual long tell();
  virtual bool isEnd();

private:
  std::unique_ptr<Impl> m_impl;
};

}

#endif // ROCKETEBOOKSTREAM_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
