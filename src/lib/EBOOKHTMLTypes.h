/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef EBOOKHTMLTYPES_H_INCLUDED
#define EBOOKHTMLTYPES_H_INCLUDED

#include <string>

#include <boost/optional.hpp>

namespace libebook
{

struct EBOOKHTMLMetadata
{
  boost::optional<std::string> title;

  EBOOKHTMLMetadata();
};

}

#endif // EBOOKHTMLTYPES_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
