/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "FictionBook2Collector.h"

namespace libebook
{

FictionBook2Collector::Binary::Binary(const std::string &contentType, const std::string &base64Data)
  : m_contentType(contentType)
  , m_data(base64Data)
{
}

FictionBook2Collector::Span::Span(const FictionBook2Style &style)
  : m_style(style)
  , m_text()
{
}

FictionBook2Collector::Paragraph::Paragraph(const FictionBook2BlockFormat &format)
  : m_format(format)
  , m_spans()
{
}

FictionBook2Collector::Note::Note()
  : m_title()
  , m_paras()
{
}

FictionBook2Collector::~FictionBook2Collector()
{
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
