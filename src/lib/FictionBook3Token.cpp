/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "FictionBook3Token.h"

#include <cassert>
#include <cstring>

#include "EBOOKTokenizer.h"

using std::strlen;

namespace libebook
{
namespace
{

#include "FictionBook3Token.inc"

}
}

namespace libebook
{

namespace
{

class Tokenizer : public EBOOKTokenizer
{
public:
  virtual ~Tokenizer();

private:
  int queryId(const char *name) const override;
};

Tokenizer::~Tokenizer()
{
}

int Tokenizer::queryId(const char *const name) const
{
  assert(name);

  const size_t length = strlen(name);
  assert(length > 0);

  const Token *const token = Perfect_Hash::in_word_set(name, length);
  return token ? token->id : 0;
}

}

namespace FictionBook3Token
{

const EBOOKTokenizer &getTokenizer()
{
  static Tokenizer tokenizer;
  return tokenizer;
}
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
