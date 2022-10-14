/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * For further information visit http://libebook.sourceforge.net
 */

#ifndef HTMLHELPSTREAM_H_INCLUDED
#define HTMLHELPSTREAM_H_INCLUDED

#include <memory>

#include <librevenge-stream/librevenge-stream.h>

namespace libebook
{

class HTMLHelpStream : public librevenge::RVNGInputStream
{
// disable copying
  HTMLHelpStream(const HTMLHelpStream &other);
  HTMLHelpStream &operator=(const HTMLHelpStream &other);

  struct Impl;

public:
  explicit HTMLHelpStream(const RVNGInputStreamPtr_t &input);
  virtual ~HTMLHelpStream();

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

#endif // HTMLHELPSTREAM_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
