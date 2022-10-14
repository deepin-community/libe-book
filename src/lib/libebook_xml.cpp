/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <cstring>

#include <librevenge-stream/librevenge-stream.h>

#include "libebook_xml.h"

extern "C"
{

  int ebookXMLReadFromStream(void *context, char *buffer, int len)
  {
    if (len<0)
      return -1;
    try
    {
      auto *const input = reinterpret_cast<librevenge::RVNGInputStream *>(context);

      unsigned long bytesRead = 0;
      const unsigned char *const bytes = input->read((unsigned long) len, bytesRead);

      if (!bytes || !bytesRead)
        return 0; // CHECKME: do we want to return -1 here ?

      std::memcpy(buffer, bytes, size_t(bytesRead));
      return static_cast<int>(bytesRead);
    }
    catch (...)
    {
    }

    return -1;
  }

  int ebookXMLCloseStream(void *)
  {
    return 0;
  }

}

namespace libebook
{

const char *char_cast(const char *const c)
{
  return c;
}

const char *char_cast(const signed char *const c)
{
  return reinterpret_cast<const char *>(c);
}

const char *char_cast(const unsigned char *const c)
{
  return reinterpret_cast<const char *>(c);
}

const char **char_array_cast(const char **const c)
{
  return c;
}

const char **char_array_cast(const signed char **const c)
{
  return reinterpret_cast<const char **>(c);
}

const char **char_array_cast(const unsigned char **const c)
{
  return reinterpret_cast<const char **>(c);
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
