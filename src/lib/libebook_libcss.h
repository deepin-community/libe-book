/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef LIBEBOOK_LIBCSS_H_INCLUDED
#define LIBEBOOK_LIBCSS_H_INCLUDED

#include <memory>

#include <boost/intrusive_ptr.hpp>

#pragma GCC diagnostic ignored "-Wpedantic"
#include <libcss/libcss.h>

namespace libebook
{

typedef std::shared_ptr<css_stylesheet> CSSStylesheetPtr_t;
typedef std::shared_ptr<css_select_ctx> CSSSelectCtxPtr_t;
typedef std::shared_ptr<css_select_results> CSSSelectResultsPtr_t;

typedef boost::intrusive_ptr<lwc_string> LWCStringPtr_t;

CSSStylesheetPtr_t wrap(css_stylesheet *p);
CSSSelectCtxPtr_t wrap(css_select_ctx *p);
CSSSelectResultsPtr_t wrap(css_select_results *p);

LWCStringPtr_t wrap(lwc_string *p);

}

void intrusive_ptr_add_ref(lwc_string *p);
void intrusive_ptr_release(lwc_string *p);

#endif // LIBEBOOK_LIBCSS_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
