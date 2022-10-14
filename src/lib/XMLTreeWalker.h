/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef XMLTREEWALKER_H_INCLUDED
#define XMLTREEWALKER_H_INCLUDED

#include <iterator>
#include <string>

#include <boost/intrusive_ptr.hpp>

#include <librevenge-stream/librevenge-stream.h>

#include "libebook_libcss.h"
#include "XMLStylesheet.h"
#include "XMLTreeNode.h"

namespace libebook
{

class XMLTreeWalker
{
public:
  class Attribute;
  class AttributeIterator;
  class Iterator;

  typedef int (*TokenizerFun_t)(const char *, std::size_t);

public:
  explicit XMLTreeWalker(const XMLTreeNodePtr_t &p, TokenizerFun_t tok = 0);

  bool empty() const;

  Iterator begin() const;
  const Iterator end() const;

  bool hasParent() const;
  const XMLTreeWalker getParent() const;

  bool isDocument() const;
  bool isElement() const;
  bool isText() const;

  int getId() const;

  const std::string getName() const;
  const std::string getNamespace() const;

  const std::string &getText() const;

  bool hasAttributes() const;

  AttributeIterator beginAttributes() const;
  const AttributeIterator endAttributes() const;

  CSSSelectResultsPtr_t getStyle(const XMLStylesheets_t &sheets, bool quirks = false,
                                 const CSSStylesheetPtr_t &inlineSheet = CSSStylesheetPtr_t()) const;

private:
  XMLTreeNodePtr_t m_impl;
  TokenizerFun_t m_tok;
};

class XMLTreeWalker::Attribute
{
  // need access to ctor
  friend class AttributeIterator;
  friend class XMLTreeWalker;

public:
  int getId() const;
  int getValueId() const;

  const std::string getName() const;
  const std::string getNamespace() const;
  const std::string getValue() const;

  bool operator==(const XMLTreeWalker::Attribute &rhs) const;

private:
  Attribute(const XMLTreeNodePtr_t &node, std::size_t index, TokenizerFun_t tok);

private:
  XMLTreeNodePtr_t m_node;
  std::size_t m_index;
  TokenizerFun_t m_tok;
};

bool operator!=(const XMLTreeWalker::Attribute &lhs, const XMLTreeWalker::Attribute &rhs);

class XMLTreeWalker::AttributeIterator
{
  friend class XMLTreeWalker; // needs access to ctor

public:
  AttributeIterator &operator++();
  const AttributeIterator operator++(int);

  const Attribute &operator*() const;
  const Attribute *operator->() const;

  bool operator==(const XMLTreeWalker::AttributeIterator &rhs) const;

private:
  explicit AttributeIterator(const XMLTreeNodePtr_t &node, TokenizerFun_t tok, bool end = false);

private:
  const XMLTreeNodePtr_t m_node;
  std::size_t m_index;
  Attribute m_current;
  TokenizerFun_t m_tok;
};

bool operator!=(const XMLTreeWalker::AttributeIterator &lhs, const XMLTreeWalker::AttributeIterator &rhs);

class XMLTreeWalker::Iterator
{
  friend class XMLTreeWalker; // needs access to ctor

public:
  Iterator &operator++();
  const Iterator operator++(int);

  const XMLTreeWalker &operator*() const;
  const XMLTreeWalker *operator->() const;

  bool operator==(const XMLTreeWalker::Iterator &rhs) const;

private:
  Iterator(const XMLTreeNodePtr_t &node, TokenizerFun_t tok);

private:
  XMLTreeNodePtr_t m_node;
  XMLTreeWalker m_current;
  TokenizerFun_t m_tok;
};

bool operator!=(const XMLTreeWalker::Iterator &lhs, const XMLTreeWalker::Iterator &rhs);

}

namespace std
{

// NOTE: I use specialization of std::iterator_traits because -Weffc++
// is too eager and does not let me derive from std::iterator .

template<>
struct iterator_traits<libebook::XMLTreeWalker::AttributeIterator>
{
  typedef libebook::XMLTreeWalker::Attribute value_type;
  typedef std::ptrdiff_t difference_type;
  typedef std::forward_iterator_tag iterator_category;
  typedef libebook::XMLTreeWalker::Attribute *pointer_type;
  typedef libebook::XMLTreeWalker::Attribute &reference_type;
};

template<>
struct iterator_traits<libebook::XMLTreeWalker::Iterator>
{
  typedef libebook::XMLTreeWalker value_type;
  typedef std::ptrdiff_t difference_type;
  typedef std::forward_iterator_tag iterator_category;
  typedef libebook::XMLTreeWalker *pointer_type;
  typedef libebook::XMLTreeWalker &reference_type;
};

}

#endif // XMLTREEWALKER_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
