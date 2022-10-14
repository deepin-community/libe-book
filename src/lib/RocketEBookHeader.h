/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * For further information visit http://libebook.sourceforge.net
 */

#ifndef ROCKETEBOOKHEADER_H_INCLUDED
#define ROCKETEBOOKHEADER_H_INCLUDED

#include <deque>
#include <string>

#include "libebook_utils.h"

namespace libebook
{

class RocketEBookHeader
{
public:
  enum Flag
  {
    FLAG_ENCRYPTED = 1,
    FLAG_INFO_PAGE = 1 << 1,
    FLAG_RESERVED = 1 << 2,
    FLAG_DEFLATED = 1 << 3
  };

  struct Entry
  {
    std::string name;
    unsigned length;
    unsigned offset;
    unsigned flags;

    Entry();
  };

  typedef std::deque<Entry> Directory_t;

public:
  explicit RocketEBookHeader(const RVNGInputStreamPtr_t &input);

  unsigned getInfoID() const;
  const Directory_t &getDirectory() const;

private:
  void readHeader(const RVNGInputStreamPtr_t &input);
  void readDirectory(const RVNGInputStreamPtr_t &input);

private:
  unsigned m_version;
  unsigned m_tocOffset;
  unsigned m_length;
  Directory_t m_dir;
  unsigned m_info;
};

}

#endif // ROCKETEBOOKHEADER_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
