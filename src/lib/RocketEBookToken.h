/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef ROCKETEBOOKTOKEN_H_INCLUDED
#define ROCKETEBOOKTOKEN_H_INCLUDED

#include "EBOOKToken.h"

namespace libebook
{

namespace RocketEBookToken
{

enum
{
  INVALID_TOKEN = 0,
  FIRST_TOKEN = EBOOKToken<RocketEBookParser>::FIRST_TOKEN,

  // keys
  AUTHOR,
  BODY,
  COMMENT,
  COPYRIGHT,
  COPYTITLE,
  COPY_ID,
  GENERATOR,
  ISBN,
  MENUMARK,
  OUTPUT,
  PARSE,
  PUBSERVER_ID,
  PUB_NAME,
  REVISION,
  SuggestedRetailPrice,
  TITLE,
  TITLE_LANGUAGE,
  TYPE,
  URL,
  USERNAME,

  // values
  unknown,

  LAST_TOKEN
};

static_assert(EBOOKToken<RocketEBookParser>::LAST_TOKEN >= LAST_TOKEN, "token number overflow");

}

int getRocketEBookTokenId(const char *name, const char *ns = nullptr);
int getRocketEBookTokenId(const char *name, std::size_t nameLen, const char *ns, std::size_t nsLen);
int getRocketEBookTokenId(const char *name, std::size_t nameLen);

}

#endif // ROCKETEBOOKTOKEN_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
