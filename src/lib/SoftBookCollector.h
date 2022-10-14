/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef SOFTBOOKCOLLECTOR_H_INCLUDED
#define SOFTBOOKCOLLECTOR_H_INCLUDED

#include <string>

#include <librevenge/librevenge.h>

#include "SoftBookTypes.h"

namespace libebook
{

class SoftBookCollector
{
public:
  explicit SoftBookCollector(librevenge::RVNGTextInterface *document);

  void openParagraph();
  void closeParagraph();
  void collectText(const std::string &text);
  void collectLineBreak();

  void collectImage();

  void openTable();
  void closeTable();
  void collectTableCell();

  void openPageHeader();
  void closePageHeader();
  void openPageFooter();
  void closePageFooter();

private:
  librevenge::RVNGTextInterface *const m_document;
};

}

#endif // SOFTBOOKCOLLECTOR_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
