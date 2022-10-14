/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef SOFTBOOKRESOURCEDIR_H_INCLUDED
#define SOFTBOOKRESOURCEDIR_H_INCLUDED

namespace libebook
{

class SoftBookHeader;
class SoftBookResourceDirImpl;

/** Read the resource tree and provide access to it as an OLE stream.
  */
class SoftBookResourceDir
{
public:
  SoftBookResourceDir(librevenge::RVNGInputStream *input, const SoftBookHeader &header);

  /** Get OLE stream for this resource directory.
    *
    * The "files" in the stream are referenced by their names (i.e.,
    * random four-letter strings)..
    *
    * @return OLE stream
    */
  std::shared_ptr<librevenge::RVNGInputStream> getNameStream() const;

  /** Get OLE stream for this resource directory.
    *
    * The "files" in the stream are referenced by their type.
    *
    * @return OLE stream
    */
  std::shared_ptr<librevenge::RVNGInputStream> getTypeStream() const;

private:
  std::shared_ptr<SoftBookResourceDirImpl> m_impl;
};

}

#endif // SOFTBOOKRESOURCEDIR_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
