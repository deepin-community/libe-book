/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef EBOOKTOKEN_H_INCLUDED
#define EBOOKTOKEN_H_INCLUDED

namespace libebook
{

template<class Parser>
struct EBOOKToken
{
};

class EBOOKHTMLParser;
class EBOOKOPFParser;
class EPubParser;
class FictionBook2Parser;
class FictionBook3Parser;
class RocketEBookParser;

template<>
struct EBOOKToken<EBOOKHTMLParser>
{
  static const int FIRST_TOKEN = 1;
  static const int LAST_TOKEN = FIRST_TOKEN + 1000;
};

template<>
struct EBOOKToken<EBOOKOPFParser>
{
  static const int FIRST_TOKEN = EBOOKToken<EBOOKHTMLParser>::LAST_TOKEN + 1;
  static const int LAST_TOKEN = FIRST_TOKEN + 1000;
};

template<>
struct EBOOKToken<EPubParser>
{
  static const int FIRST_TOKEN = EBOOKToken<EBOOKOPFParser>::LAST_TOKEN + 1;
  static const int LAST_TOKEN = FIRST_TOKEN + 1000;
};

template<>
struct EBOOKToken<FictionBook2Parser>
{
  static const int FIRST_TOKEN = EBOOKToken<EPubParser>::LAST_TOKEN + 1;
  static const int LAST_TOKEN = FIRST_TOKEN + 1000;
};

template<>
struct EBOOKToken<RocketEBookParser>
{
  static const int FIRST_TOKEN = EBOOKToken<FictionBook2Parser>::LAST_TOKEN + 1;
  static const int LAST_TOKEN = FIRST_TOKEN + 1000;
};

template<>
struct EBOOKToken<FictionBook3Parser>
{
  static const int FIRST_TOKEN = EBOOKToken<RocketEBookParser>::LAST_TOKEN + 1;
  static const int LAST_TOKEN = FIRST_TOKEN + 1000;
};

}

#endif // EBOOKTOKEN_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
