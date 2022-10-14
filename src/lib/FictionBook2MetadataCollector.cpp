/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <librevenge/librevenge.h>

#include "FictionBook2MetadataCollector.h"

namespace libebook
{

FictionBook2MetadataCollector::FictionBook2MetadataCollector(librevenge::RVNGPropertyList &metadata)
  : m_metadata(metadata)
{
}

void FictionBook2MetadataCollector::defineMetadataEntry(const char *name, const char *value)
{
  m_metadata.insert(name, librevenge::RVNGString(value));
}

void FictionBook2MetadataCollector::openMetadataEntry(const char *name)
{
  (void) name;
}

void FictionBook2MetadataCollector::closeMetadataEntry()
{
}

void FictionBook2MetadataCollector::defineID(const char *)
{
}

void FictionBook2MetadataCollector::openPageSpan()
{
}

void FictionBook2MetadataCollector::closePageSpan()
{
}

void FictionBook2MetadataCollector::openBlock()
{
}

void FictionBook2MetadataCollector::closeBlock()
{
}

void FictionBook2MetadataCollector::openParagraph(const FictionBook2BlockFormat &)
{
}

void FictionBook2MetadataCollector::closeParagraph()
{
}

void FictionBook2MetadataCollector::openSpan(const FictionBook2Style &)
{
}

void FictionBook2MetadataCollector::closeSpan()
{
}

void FictionBook2MetadataCollector::insertText(const char *text)
{
  (void) text;
}

void FictionBook2MetadataCollector::openTable(const FictionBook2BlockFormat &)
{
}

void FictionBook2MetadataCollector::closeTable()
{
}

void FictionBook2MetadataCollector::openTableRow(const FictionBook2BlockFormat &)
{
}

void FictionBook2MetadataCollector::closeTableRow()
{
}

void FictionBook2MetadataCollector::openTableCell(int, int)
{
}

void FictionBook2MetadataCollector::closeTableCell()
{
}

void FictionBook2MetadataCollector::insertCoveredTableCell()
{
}

void FictionBook2MetadataCollector::insertFootnote(const char *)
{
}

void FictionBook2MetadataCollector::insertBitmap(const char *)
{
}

void FictionBook2MetadataCollector::insertBitmapData(const char *, const char *)
{
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
