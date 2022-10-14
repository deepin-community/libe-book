/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef BBEBCOLLECTOR_H_INCLUDED
#define BBEBCOLLECTOR_H_INCLUDED

#include <string>
#include <stack>

#include "BBeBTypes.h"
#include "libebook_utils.h"

namespace libebook
{

class BBeBCollector
{
  // -Weffc++
  BBeBCollector(const BBeBCollector &other);
  BBeBCollector &operator=(const BBeBCollector &other);

  struct ImageStreamData
  {
    ImageStreamData();

    RVNGInputStreamPtr_t image;
    BBeBImageType type;
  };
  typedef std::map<unsigned, ImageStreamData> ImageDataMap_t;

  struct ImageData
  {
    unsigned dataId;
    unsigned width;
    unsigned height;
  };
  typedef std::map<unsigned, ImageData> ImageMap_t;

public:
  explicit BBeBCollector(librevenge::RVNGTextInterface *document);
  ~BBeBCollector();

  void startDocument();
  void endDocument();

  void openPage(unsigned pageAtrID, const BBeBAttributes &attributes);
  void closePage();

  void openBlock(unsigned atrID, const BBeBAttributes &attributes);
  void closeBlock();

  void openTextBlock(unsigned atrID, const BBeBAttributes &attributes);
  void closeTextBlock();

  void openParagraph(unsigned atrID, const BBeBAttributes &attributes);
  void closeParagraph();

  void collectText(const std::string &text, const BBeBAttributes &attributes);
  void insertLineBreak();

  void insertImage(unsigned id);

  void collectMetadata(const BBeBMetadata &metadata);

  void collectBookAttributes(const BBeBAttributes &attributes);
  void collectTextAttributes(unsigned id, const BBeBAttributes &attributes);
  void collectBlockAttributes(unsigned id, const BBeBAttributes &attributes);
  void collectPageAttributes(unsigned id, const BBeBAttributes &attributes);
  void collectParagraphAttributes(unsigned id, const BBeBAttributes &attributes);

  void collectImage(unsigned id, unsigned dataId, unsigned width, unsigned height);
  void collectImageData(unsigned id, BBeBImageType type, const RVNGInputStreamPtr_t &image);

  void setDPI(double dpi);

private:
  void collectAttributes(unsigned id, const BBeBAttributes &attributes, BBeBAttributeMap_t &attributeMap);
  void openBlock(unsigned atrID, const BBeBAttributes &attributes, const BBeBAttributeMap_t *attributeMap);

private:
  BBeBAttributes m_bookAttributes;
  BBeBAttributeMap_t m_textAttributeMap;
  BBeBAttributeMap_t m_blockAttributeMap;
  BBeBAttributeMap_t m_pageAttributeMap;
  BBeBAttributeMap_t m_paragraphAttributeMap;
  librevenge::RVNGTextInterface *const m_document;
  std::stack<BBeBAttributes> m_currentAttributes;
  ImageDataMap_t m_imageDataMap;
  ImageMap_t m_imageMap;
  double m_dpi;
  bool m_firstParaInBlock;
};

}

#endif // BBEBCOLLECTOR_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
