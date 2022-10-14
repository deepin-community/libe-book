/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef FICTIONBOOK2PARSER_H_INCLUDED
#define FICTIONBOOK2PARSER_H_INCLUDED


namespace libebook
{

class FictionBook2XMLParserContext;

class FictionBook2Parser
{
  // no copying
  FictionBook2Parser(const FictionBook2Parser &other);
  FictionBook2Parser &operator=(const FictionBook2Parser &other);

public:
  explicit FictionBook2Parser(librevenge::RVNGInputStream *input);

  bool parse(FictionBook2XMLParserContext *context) const;
  bool parse(librevenge::RVNGTextInterface *document) const;

private:
  librevenge::RVNGInputStream *const m_input;
};

}

#endif // FICTIONBOOK2PARSER_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
