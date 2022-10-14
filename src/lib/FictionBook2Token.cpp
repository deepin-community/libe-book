/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <cstring>

#include "FictionBook2Token.h"

using std::strlen;

namespace libebook
{
namespace
{

#include "FictionBook2Token.inc"

}
}

namespace libebook
{

const FictionBook2TokenData *getFictionBook2Token(const char *const str, const unsigned length)
{
  if (!str)
    return nullptr;
  return reinterpret_cast<const FictionBook2TokenData *>(Perfect_Hash::in_word_set(str, length));
}

const FictionBook2TokenData *getFictionBook2Token(const char *const str)
{
  if (!str)
    return nullptr;
  return getFictionBook2Token(str, (unsigned) strlen(str));
}

const FictionBook2TokenData *getFictionBook2Token(const unsigned char *const str)
{
  if (!str)
    return nullptr;
  return getFictionBook2Token(reinterpret_cast<const char *>(str));
}

int getFictionBook2TokenID(const FictionBook2TokenData &token)
{
  return getFictionBook2TokenID(&token);
}

int getFictionBook2TokenID(const FictionBook2TokenData *const token)
{
  return token ? token->id : INVALID_TOKEN;
}

int getFictionBook2TokenID(const char *str, const unsigned length)
{
  return getFictionBook2TokenID(getFictionBook2Token(str, length));
}

int getFictionBook2TokenID(const char *str)
{
  if (!str)
    return 0;
  return getFictionBook2TokenID(str, (unsigned) strlen(str));
}

int getFictionBook2TokenID(const unsigned char *str)
{
  return getFictionBook2TokenID(reinterpret_cast<const char *>(str));
}

const char *getFictionBook2TokenName(const FictionBook2TokenData &token)
{
  return getFictionBook2TokenName(&token);
}

const char *getFictionBook2TokenName(const FictionBook2TokenData *const token)
{
  return token ? token->name : nullptr;
}

const char *getFictionBook2TokenName(const char *str, const unsigned length)
{
  return getFictionBook2TokenName(getFictionBook2Token(str, length));
}

const char *getFictionBook2TokenName(const char *str)
{
  return getFictionBook2TokenName(str, (unsigned) strlen(str));
}

const char *getFictionBook2TokenName(const unsigned char *str)
{
  return getFictionBook2TokenName(reinterpret_cast<const char *>(str));
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
