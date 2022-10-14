/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef EBOOKXMLPARSER_H_INCLUDED
#define EBOOKXMLPARSER_H_INCLUDED

#include "libebook_utils.h"

namespace libebook
{

class EBOOKTokenizer;
class EBOOKXMLContext;

class EBOOKXMLParser
{
  // disable copying
  EBOOKXMLParser(const EBOOKXMLParser &);
  EBOOKXMLParser &operator=(const EBOOKXMLParser &);

public:
  /** Determines whether or not the parser should quit when a context
      returns an empty context for a child element.
   */
  enum RunStyle
  {
    RunToEnd, /// Continue, skipping the child element
    AllowEarlyExit /// Exit
  };

public:
  explicit EBOOKXMLParser(RunStyle runStyle = RunToEnd);

  bool parse(const RVNGInputStreamPtr_t &input);

protected:
  ~EBOOKXMLParser();

private:
  virtual std::shared_ptr<EBOOKXMLContext> createDocumentContext() = 0;

  virtual const EBOOKTokenizer &getTokenizer() const = 0;

private:
  const RunStyle m_runStyle;
};

}

#endif

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
