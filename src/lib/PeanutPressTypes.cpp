/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "PeanutPressTypes.h"

namespace libebook
{

PeanutPressAttributes::PeanutPressAttributes()
  : pageBreak(false)
  , chapter(false)
  , center(false)
  , right(false)
  , italic(false)
  , underline(false)
  , overstrike(false)
  , indent(false)
  , indentPercent(false)
  , font(PEANUTPRESS_FONT_TYPE_STD)
  , bold(false)
  , superscript(false)
  , subscript(false)
  , smallcaps(false)
{
}

} // namespace libebook

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
