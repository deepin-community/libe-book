/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "EBOOKTokenizer.h"

namespace libebook
{

EBOOKTokenizer::~EBOOKTokenizer()
{
}

int EBOOKTokenizer::getId(const char *const name) const
{
  if (!name || (0 == name[0]))
    return 0;

  return queryId(name);
}

int EBOOKTokenizer::getQualifiedId(const char *const name, const char *const ns) const
{
  if (!name || (0 == name[0]))
    return 0;

  if (!ns || (0 == ns[0]))
    return getId(name);

  const int nameId = queryId(name);
  const int nsId = queryId(ns);

  return ((0 == nameId) || (0 == nsId)) ? 0 : (nsId | nameId);
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
