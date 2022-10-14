/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "SoftBookCollector.h"

namespace libebook
{

SoftBookCollector::SoftBookCollector(librevenge::RVNGTextInterface *const document)
  : m_document(document)
{
}

void SoftBookCollector::openParagraph()
{
  // TODO: implement me
  if (!m_document)
    return;
}

void SoftBookCollector::closeParagraph()
{
  // TODO: implement me
  if (!m_document)
    return;
}

void SoftBookCollector::collectText(const std::string &text)
{
  // TODO: implement me
  (void) text;
  if (!m_document)
    return;
}

void SoftBookCollector::collectLineBreak()
{
  // TODO: implement me
  if (!m_document)
    return;
}


void SoftBookCollector::collectImage()
{
  // TODO: implement me
  if (!m_document)
    return;
}


void SoftBookCollector::openTable()
{
  // TODO: implement me
  if (!m_document)
    return;
}

void SoftBookCollector::closeTable()
{
  // TODO: implement me
  if (!m_document)
    return;
}

void SoftBookCollector::collectTableCell()
{
  // TODO: implement me
  if (!m_document)
    return;
}


void SoftBookCollector::openPageHeader()
{
  // TODO: implement me
  if (!m_document)
    return;
}

void SoftBookCollector::closePageHeader()
{
  // TODO: implement me
  if (!m_document)
    return;
}

void SoftBookCollector::openPageFooter()
{
  // TODO: implement me
  if (!m_document)
    return;
}

void SoftBookCollector::closePageFooter()
{
  // TODO: implement me
  if (!m_document)
    return;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
