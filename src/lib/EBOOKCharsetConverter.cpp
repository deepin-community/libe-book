/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <cassert>
#include <string>

#include <unicode/ucsdet.h>

#include "EBOOKCharsetConverter.h"

using std::string;

namespace libebook
{

namespace
{

class ConverterException
{
};

/** Guess character set of the text.

  @param[in] text the text
  @param[out] charset the character set
  @param[out] confidence Confidence of the guess, in range [0, 100]; 100 means certainty.

  @returns false if detection failed, true otherwise
 */
bool guessCharacterSet(const char *const text, const unsigned length, string &charset, int32_t &confidence)
{
  // reset output
  charset.clear();
  confidence = 0;

  UErrorCode status = U_ZERO_ERROR;

  UCharsetDetector *detector = ucsdet_open(&status);
  if (status != U_ZERO_ERROR)
    return false;

  ucsdet_setText(detector, text, static_cast<int32_t>(length), &status);
  if (status == U_ZERO_ERROR)
  {
    const UCharsetMatch *const match = ucsdet_detect(detector, &status);
    if (!match)
      status = U_BUFFER_OVERFLOW_ERROR;
    if (status == U_ZERO_ERROR)
    {
      charset = ucsdet_getName(match, &status);
      if (status == U_ZERO_ERROR)
        confidence = ucsdet_getConfidence(match, &status);
    }
  }

  ucsdet_close(detector);

  return status == U_ZERO_ERROR;
}

}

EBOOKCharsetConverter::EBOOKCharsetConverter(const char *const encoding)
  : m_converterToUnicode(nullptr, ucnv_close)
  , m_converterToUTF8(nullptr, ucnv_close)
{
  UErrorCode status = U_ZERO_ERROR;
  m_converterToUTF8.reset(ucnv_open("utf-8", &status));
  if (status != U_ZERO_ERROR)
    throw ConverterException();

  if (encoding)
  {
    m_converterToUnicode.reset(ucnv_open(encoding, &status));
    if (status != U_ZERO_ERROR)
      throw ConverterException();
  }
}

EBOOKCharsetConverter::~EBOOKCharsetConverter()
{
}

bool EBOOKCharsetConverter::guessEncoding(const char *const in, const unsigned length)
{
  if (m_converterToUnicode)
    return true;

  string charset;
  int32_t confidence = 0;
  if (guessCharacterSet(in, length, charset, confidence))
  {
    UErrorCode status = U_ZERO_ERROR;
    m_converterToUnicode.reset(ucnv_open(charset.c_str(), &status));
    if (status == U_ZERO_ERROR)
      return true;
  }

  return false;
}

bool EBOOKCharsetConverter::convertBytes(const char *const in, const unsigned length, std::vector<char> &out)
{
  assert(m_converterToUnicode);
  assert(m_converterToUTF8);
  assert(0 != length); // the caller likely needs to check this anyway

  if (out.empty())
    out.resize(length);

  while (true)
  {
    const char *inText = in;
    char *outText = &out[0];
    UErrorCode status = U_ZERO_ERROR;
    ucnv_convertEx(
      m_converterToUTF8.get(), m_converterToUnicode.get(),
      &outText, outText + out.size(), &inText, inText + length,
      nullptr, nullptr, nullptr, nullptr,
      TRUE, TRUE, &status)
    ;
    if (status==U_BUFFER_OVERFLOW_ERROR)
    {
      out.resize(out.size() + length);
      continue;
    }
    if (status!=U_STRING_NOT_TERMINATED_WARNING && status!=U_ZERO_ERROR)
      return false;
    auto outLength = static_cast<int>(outText - &out[0]);
    out.erase(out.begin() + outLength, out.end());
    break;
  }

  return true;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
