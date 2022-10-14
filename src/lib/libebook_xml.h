/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef LIBEBOOK_XML_H_INCLUDED
#define LIBEBOOK_XML_H_INCLUDED

extern "C"
{

  int ebookXMLReadFromStream(void *context, char *buffer, int len);
  int ebookXMLCloseStream(void *context);

}

namespace libebook
{

const char *char_cast(const char *c);
const char *char_cast(const signed char *c);
const char *char_cast(const unsigned char *c);

const char **char_array_cast(const char **c);
const char **char_array_cast(const signed char **c);
const char **char_array_cast(const unsigned char **c);

}

#endif // LIBEBOOK_XML_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
