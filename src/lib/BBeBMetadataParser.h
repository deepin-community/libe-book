/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef BBEBMETADATAPARSER_H_INCLUDED
#define BBEBMETADATAPARSER_H_INCLUDED

#include <librevenge-stream/librevenge-stream.h>

#include "BBeBTypes.h"

namespace libebook
{

class BBeBMetadataParser
{
  // Weffc++
  BBeBMetadataParser(const BBeBMetadataParser &other);
  BBeBMetadataParser &operator=(const BBeBMetadataParser &other);

public:
  explicit BBeBMetadataParser(librevenge::RVNGInputStream *input);

  void parse();

  const BBeBMetadata &getMetadata() const;

private:
  BBeBMetadata m_metadata;
  librevenge::RVNGInputStream *const m_input;
};

}

#endif // BBEBMETADATAPARSER_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
