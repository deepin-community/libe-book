/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef FICTIONBOOK3TOKEN_H_INCLUDED
#define FICTIONBOOK3TOKEN_H_INCLUDED

#include "EBOOKToken.h"

#define FictionBook3_TOKEN(ns, name) (FictionBook3Token::ns | FictionBook3Token::NS_#name)

namespace libebook
{

class EBOOKTokenizer;

namespace FictionBook3Token
{

enum
{
  INVALID_TOKEN = 0,
  FIRST_TOKEN = EBOOKToken<FictionBook3Parser>::FIRST_TOKEN,

  // namespaces
  NS_fb3b,
  NS_fb3d,
  NS_xlink,

  // elements

  // attributes

  // attribute values

  LAST_TOKEN
};

static_assert(EBOOKToken<FictionBook3Parser>::LAST_TOKEN >= LAST_TOKEN, "token number overflow");

const EBOOKTokenizer &getTokenizer();

}

}

#endif // FICTIONBOOK3TOKEN_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
