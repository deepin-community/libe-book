/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef EBOOKTOKENIZER_H_INCLUDED
#define EBOOKTOKENIZER_H_INCLUDED

namespace libebook
{

class EBOOKTokenizer
{
public:
  int getId(const char *name) const;
  int getQualifiedId(const char *name, const char *ns) const;

protected:
  ~EBOOKTokenizer();

private:
  /** Try to get an ID for token \c name.
    *
    * @arg[in] name The token name. It is never empty or null.
    *
    * @return The ID of the token or 0.
    */
  virtual int queryId(const char *name) const = 0;
};

}

#endif // EBOOKTOKENIZERBASE_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
