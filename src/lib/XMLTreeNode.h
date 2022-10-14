/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef XMLTREENODE_H_INCLUDED
#define XMLTREENODE_H_INCLUDED

#include <deque>
#include <string>
#include <utility>

#include <boost/intrusive_ptr.hpp>

#include "libebook_libcss.h"

namespace libebook
{

struct XMLTreeNode;
typedef boost::intrusive_ptr<XMLTreeNode> XMLTreeNodePtr_t;

struct XMLTreeNode
{
  enum NodeType
  {
    NODE_TYPE_DOCUMENT = 1 << 1,
    NODE_TYPE_ELEMENT = 1 << 2,
    NODE_TYPE_TEXT = 1 << 3,
    NODE_TYPE_COMMENT = 1 << 4,
    NODE_TYPE_DOCTYPE = 1 << 5
  };

  struct QName
  {
    LWCStringPtr_t name;
    LWCStringPtr_t ns;

    QName();
    explicit QName(const LWCStringPtr_t &name, const LWCStringPtr_t &ns = LWCStringPtr_t());
  };

  typedef std::pair<QName, QName> LinkId_t;

  struct Configuration
  {
    bool caseless;

    std::deque<QName> classes; //< A list of attributes representing a "class".
    std::deque<QName> ids; //< a list of attributes representing an "ID".
    std::deque<QName> langs; //< a list of attributes representing a "lang".

    /** A list of element / attribute pairs representing a "link".
      *
      * Either element or attribute name can be empty, matching
      * anything. E.g., xlink:href attribute is always a link.
      */
    std::deque<LinkId_t> links;

    explicit Configuration(bool caseless_ = false);
  };

  typedef std::shared_ptr<Configuration> ConfigurationPtr_t;

  struct ElementData
  {
    QName qname;
    mutable int id;
    mutable bool idSet;

    ElementData();
    explicit ElementData(const LWCStringPtr_t &name, const LWCStringPtr_t &ns = LWCStringPtr_t());
  };

  struct AttributeData
  {
    QName qname;
    LWCStringPtr_t value;
    mutable int id;
    mutable bool idSet;

    AttributeData();
    AttributeData(const LWCStringPtr_t &name, const LWCStringPtr_t &ns, const LWCStringPtr_t &val);
    AttributeData(const LWCStringPtr_t &name, const LWCStringPtr_t &val);
  };

public:
  explicit XMLTreeNode(NodeType nodeType, const ConfigurationPtr_t &config = ConfigurationPtr_t());
  ~XMLTreeNode();

  NodeType type;
  ConfigurationPtr_t configuration;

  ElementData self;
  std::deque<AttributeData> attributes;
  std::string text;

  XMLTreeNodePtr_t parent;
  // siblings
  XMLTreeNodePtr_t prev;
  XMLTreeNodePtr_t next;
  // children
  XMLTreeNodePtr_t first;
  XMLTreeNodePtr_t last;

  size_t elements;

  mutable int refcount;

  XMLTreeNodePtr_t clone(bool deep = false) const;

  XMLTreeNodePtr_t appendChild(const XMLTreeNodePtr_t &child);
  XMLTreeNodePtr_t insertChildBefore(const XMLTreeNodePtr_t &child, const XMLTreeNodePtr_t &selected);
  void removeChild(const XMLTreeNodePtr_t &child);
  void transferChildren(const XMLTreeNodePtr_t &newParent);

  void addAttributes(const std::deque<AttributeData> &attrs);

  bool hasChildren(bool ignoreText = false) const;

private:
  // disable copying
  XMLTreeNode(const XMLTreeNode &);
  XMLTreeNode &operator=(const XMLTreeNode &);
};

bool operator==(const XMLTreeNode::QName &lhs, const XMLTreeNode::QName &rhs);
bool operator!=(const XMLTreeNode::QName &lhs, const XMLTreeNode::QName &rhs);
bool caselessEqual(const XMLTreeNode::QName &lhs, const XMLTreeNode::QName &rhs);

bool operator==(const XMLTreeNode::AttributeData &lhs, const XMLTreeNode::AttributeData &rhs);
bool operator!=(const XMLTreeNode::AttributeData &lhs, const XMLTreeNode::AttributeData &rhs);

bool operator==(const XMLTreeNode::ElementData &lhs, const XMLTreeNode::ElementData &rhs);
bool operator!=(const XMLTreeNode::ElementData &lhs, const XMLTreeNode::ElementData &rhs);

void intrusive_ptr_add_ref(XMLTreeNode *p);
void intrusive_ptr_release(XMLTreeNode *p);

}

#endif // XMLTREENODE_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
