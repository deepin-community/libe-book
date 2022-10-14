/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef SOFTBOOKTYPES_H_INCLUDED
#define SOFTBOOKTYPES_H_INCLUDED

#include <string>

namespace libebook
{

enum SoftBookColorMode
{
  SOFTBOOK_COLOR_MODE_UNKNOWN,
  SOFTBOOK_COLOR_MODE_COLOR,
  SOFTBOOK_COLOR_MODE_GRAYSCALE
};

struct SoftBookMetadata
{
  SoftBookMetadata();

  std::string id;
  std::string category;
  std::string subcategory;
  std::string title;
  std::string lastName;
  std::string middleName;
  std::string firstName;
};

}

#endif // SOFTBOOKTYPES_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
