/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef SOFTBOOKHEADER_H_INCLUDED
#define SOFTBOOKHEADER_H_INCLUDED

#include "SoftBookTypes.h"

namespace libebook
{

class SoftBookHeader
{
public:
  explicit SoftBookHeader(librevenge::RVNGInputStream *input);

  static std::shared_ptr<SoftBookHeader> create(librevenge::RVNGInputStream *input);

  unsigned getVersion() const;
  SoftBookColorMode getColorMode() const;
  unsigned getFileCount() const;
  bool getCompressed() const;
  bool getEncrypted() const;
  const SoftBookMetadata &getMetadata() const;

  unsigned getTOCOffset() const;

private:
  void readHeader(librevenge::RVNGInputStream *input);
  void readBookProperties(librevenge::RVNGInputStream *input);

private:
  unsigned m_version;
  SoftBookColorMode m_colorMode;
  unsigned m_files;
  unsigned m_dirNameLength;
  unsigned m_remainingBytes;
  bool m_compressed;
  bool m_encrypted;
  SoftBookMetadata m_metadata;
};

}

#endif // SOFTBOOKHEADER_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
