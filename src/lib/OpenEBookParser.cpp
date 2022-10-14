/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <cassert>

#include "EBOOKOPFParser.h"
#include "OpenEBookParser.h"

namespace libebook
{

OpenEBookParser::OpenEBookParser(librevenge::RVNGInputStream *input, librevenge::RVNGTextInterface *document)
  : m_input(input)
  , m_document(document)
{
  assert(m_input);
}

void OpenEBookParser::parse()
{
  if (m_document)
  {
    const RVNGInputStreamPtr_t package(m_input, EBOOKDummyDeleter());

    unsigned opfId = 0;
    if (!EBOOKOPFParser::findOPFStream(package, opfId))
      throw PackageError();

    const RVNGInputStreamPtr_t opf(package->getSubStreamById(opfId));

    EBOOKOPFParser parser(opf, package, EBOOKOPFParser::TYPE_OEB, m_document);
    parser.parse();
  }
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
