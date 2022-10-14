/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef EBOOKCHARSETCONVERTER_H_INCLUDED
#define EBOOKCHARSETCONVERTER_H_INCLUDED

#include <memory>
#include <vector>

#include <unicode/ucnv.h>

namespace libebook
{

class EBOOKCharsetConverter
{
  // disable copying
  EBOOKCharsetConverter(const EBOOKCharsetConverter &other);
  EBOOKCharsetConverter &operator=(const EBOOKCharsetConverter &other);

public:
  explicit EBOOKCharsetConverter(const char *encoding = nullptr);
  ~EBOOKCharsetConverter();

  bool guessEncoding(const char *in, unsigned length);

  bool convertBytes(const char *in, unsigned length, std::vector<char> &out);

private:
  using UConverterPtr_t = std::unique_ptr<UConverter, void (*)(UConverter *)>;
  UConverterPtr_t m_converterToUnicode;
  UConverterPtr_t m_converterToUTF8;
};

}

#endif // EBOOKCHARSETCONVERTER_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
