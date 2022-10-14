/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "libebook_libcss.h"

namespace libebook
{

CSSStylesheetPtr_t wrap(css_stylesheet *const p)
{
  return CSSStylesheetPtr_t(p, css_stylesheet_destroy);
}

CSSSelectCtxPtr_t wrap(css_select_ctx *const p)
{
  return CSSSelectCtxPtr_t(p, css_select_ctx_destroy);
}

CSSSelectResultsPtr_t wrap(css_select_results *const p)
{
  return CSSSelectResultsPtr_t(p, css_select_results_destroy);
}

LWCStringPtr_t wrap(lwc_string *const p)
{
  return LWCStringPtr_t(p, false);
}

}

void intrusive_ptr_add_ref(lwc_string *const p)
{
  lwc_string_ref(p);
}

void intrusive_ptr_release(lwc_string *const p)
{
  lwc_string_unref(p);
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
