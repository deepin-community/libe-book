/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <sstream>

#include "libebook_utils.h"
#include "libebook_xml.h"
#include "XMLStylesheet.h"

using std::string;

namespace libebook
{

XMLStylesheet::XMLStylesheet(const XMLStylesheet::Source source, const bool quirks, librevenge::RVNGInputStream *const input)
  : m_data()
  , m_source(source)
  , m_quirks(quirks)
  , m_changed(true)
  , m_compiled()
{
  const unsigned len = getRemainingLength(input);
  const unsigned char *const data = readNBytes(input, len);
  m_data.push_back(string(char_cast(data), len));
}

void XMLStylesheet::append(const std::string &selector, const std::string &rule)
{
  std::ostringstream os;
  os << selector << " { " << rule << " };";
  m_data.push_back(os.str());
  m_changed = true;
}

bool XMLStylesheet::isQuirks() const
{
  return m_quirks;
}

XMLStylesheet::Source XMLStylesheet::getSource() const
{
  return m_source;
}

CSSStylesheetPtr_t XMLStylesheet::get() const
{
  if (m_changed)
  {
    css_stylesheet_params params;
    params.params_version = CSS_STYLESHEET_PARAMS_VERSION_1;
    params.level = CSS_LEVEL_DEFAULT;
    // TODO: this must come from outside or be guessed
    params.charset = "UTF-8";
    params.url = "";
    params.title = 0;
    params.allow_quirks = m_quirks;
    // TODO: this must be from outside
    params.inline_style = false;
    // TODO: set
    params.resolve = 0;
    params.resolve_pw = 0;
    params.import = 0;
    params.import_pw = 0;
    params.color = 0;
    params.color_pw = 0;
    params.font = 0;
    params.font_pw = 0;

    css_stylesheet *cSheet = 0;
    if (CSS_OK != css_stylesheet_create(&params, &cSheet))
      throw GenericException();
    const CSSStylesheetPtr_t sheet = wrap(cSheet);

    for (std::deque<string>::const_iterator it = m_data.begin(); m_data.end() != it; ++it)
    {
      const uint8_t *const data = reinterpret_cast<const uint8_t *>(it->data());
      const css_error code = css_stylesheet_append_data(sheet.get(), data, it->size());
      if ((CSS_OK != code) && (CSS_NEEDDATA != code))
        throw GenericException();
    }
    if (CSS_OK != css_stylesheet_data_done(sheet.get()))
      throw GenericException();

    m_compiled = sheet;
    m_changed = false;
  }

  return m_compiled;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
