/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "BBeBMetadataParser.h"

namespace libebook
{

BBeBMetadataParser::BBeBMetadataParser(librevenge::RVNGInputStream *const input)
  : m_metadata()
  , m_input(input)
{
}

void BBeBMetadataParser::parse()
{
  // TODO: implement me
}

const BBeBMetadata &BBeBMetadataParser::getMetadata() const
{
  return m_metadata;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
