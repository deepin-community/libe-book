/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <cstring>

#include "EPubToken.h"

namespace libebook
{
namespace
{

#include "EPubToken.inc"

}
}

namespace libebook
{

int getEPubTokenId(const char *const name, const char *const ns)
{
  if (ns)
    return getEPubTokenId(name, strlen(name), ns, strlen(ns));
  return getEPubTokenId(name, strlen(name));
}

int getEPubTokenId(const char *const name, const std::size_t nameLen, const char *const ns, const std::size_t nsLen)
{
  return getEPubTokenId(name, nameLen) | (ns ? getEPubTokenId(ns, nsLen) : 0);
}

int getEPubTokenId(const char *const name, const std::size_t nameLen)
{
  if (!name)
    return EPubToken::INVALID_TOKEN;
  const Token *const token = Perfect_Hash::in_word_set(name, (unsigned) nameLen);
  return token ? token->id : EPubToken::INVALID_TOKEN;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
