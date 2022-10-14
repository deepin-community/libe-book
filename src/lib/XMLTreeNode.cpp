/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <algorithm>
#include <cassert>
#include <deque>

#include "libebook_utils.h"
#include "XMLTreeNode.h"

using std::shared_ptr;

using std::deque;
using std::find_if;
using std::size_t;
using std::string;

namespace libebook
{

namespace
{

struct MatchAttribute
{
  explicit MatchAttribute(const XMLTreeNode::AttributeData &attribute) : m_qname(attribute.qname) {}

  bool operator()(const XMLTreeNode::AttributeData &attr)
  {
    return m_qname == attr.qname;
  }

private:
  const XMLTreeNode::QName &m_qname;
};

// queries

bool doHasChildren(const XMLTreeNodePtr_t &node, bool ignoreText)
{
  assert(bool(node));
  assert((XMLTreeNode::NODE_TYPE_ELEMENT | XMLTreeNode::NODE_TYPE_DOCUMENT) & node->type);

  if (!node->first)
    return false;

  if (!ignoreText)
    return true;

  for (XMLTreeNodePtr_t current = node->first; bool(current); current = current->next)
  {
    if (XMLTreeNode::NODE_TYPE_ELEMENT == current->type)
      return true;
  }

  return false;
}

// modifiers

XMLTreeNodePtr_t doAppendChild(const XMLTreeNodePtr_t &node, const XMLTreeNodePtr_t &child)
{
  assert(bool(node));
  assert(bool(child));
  assert((XMLTreeNode::NODE_TYPE_ELEMENT | XMLTreeNode::NODE_TYPE_DOCUMENT) & node->type);
  assert((XMLTreeNode::NODE_TYPE_ELEMENT | XMLTreeNode::NODE_TYPE_TEXT | XMLTreeNode::NODE_TYPE_COMMENT | XMLTreeNode::NODE_TYPE_DOCTYPE) & child->type);

  XMLTreeNodePtr_t inserted = child;

  if ((XMLTreeNode::NODE_TYPE_COMMENT | XMLTreeNode::NODE_TYPE_DOCTYPE) & child->type)
    return inserted;

  if ((XMLTreeNode::NODE_TYPE_TEXT == child->type) && bool(node->last) && (XMLTreeNode::NODE_TYPE_TEXT == node->last->type))
  {
    // merge into existing text node
    inserted = node->last;
    inserted->text.insert(inserted->text.end(), child->text.begin(), child->text.end());
  }
  else
  {
    child->parent = node;
    if (bool(node->last))
      node->last->next = child;
    child->prev = node->last;
    child->next.reset();
    node->last = child;
    if (!node->first)
      node->first = child;

    if (XMLTreeNode::NODE_TYPE_ELEMENT == child->type)
      ++node->elements;
  }

  assert(bool(inserted));
  assert(bool(node->first) && bool(node->last));

  return inserted;
}

XMLTreeNodePtr_t doInsertChildBefore(const XMLTreeNodePtr_t &node, const XMLTreeNodePtr_t &child, const XMLTreeNodePtr_t &selected)
{
  assert(bool(node));
  assert(bool(child));
  assert(bool(selected));
  assert((XMLTreeNode::NODE_TYPE_ELEMENT | XMLTreeNode::NODE_TYPE_DOCUMENT) & node->type);
  assert((XMLTreeNode::NODE_TYPE_ELEMENT | XMLTreeNode::NODE_TYPE_TEXT | XMLTreeNode::NODE_TYPE_COMMENT | XMLTreeNode::NODE_TYPE_DOCTYPE) & child->type);
  assert(selected->parent == node);

  XMLTreeNodePtr_t inserted = child;

  if ((XMLTreeNode::NODE_TYPE_COMMENT | XMLTreeNode::NODE_TYPE_DOCTYPE) & child->type)
    return inserted;

  if ((XMLTreeNode::NODE_TYPE_TEXT == child->type) && bool(selected->prev) && (XMLTreeNode::NODE_TYPE_TEXT == selected->prev->type))
  {
    // merge into preceding text node
    inserted = selected->prev;
    inserted->text.insert(inserted->text.end(), child->text.begin(), child->text.end());
  }
  else if ((XMLTreeNode::NODE_TYPE_TEXT == child->type) && bool(selected->next) && (XMLTreeNode::NODE_TYPE_TEXT == selected->next->type))
  {
    // merge into following text node
    inserted = selected->next;
    inserted->text.insert(inserted->text.begin(), child->text.begin(), child->text.end());
  }
  else
  {
    // insert a regular node
    child->parent = node;

    if (node->first == selected)
    {
      node->first->prev = child;
      child->next = node->first;
      child->prev.reset();
      node->first = child;
    }
    else
    {
      selected->prev->next = child;
      child->prev = selected->prev;
      selected->prev = child;
      child->next = selected;
    }

    if (XMLTreeNode::NODE_TYPE_ELEMENT == child->type)
      ++node->elements;
  }

  assert(bool(inserted));
  assert(bool(node->first) && bool(node->last));

  return inserted;
}

void unlink(XMLTreeNode *const node)
{
  assert(bool(node));

  node->parent.reset();
  node->prev.reset();
  node->next.reset();
}

void unlink(const XMLTreeNodePtr_t &node)
{
  assert(bool(node));

  unlink(node.get());
}

void doRemoveChild(const XMLTreeNodePtr_t &node, const XMLTreeNodePtr_t &child)
{
  assert(bool(node));
  assert(bool(child));
  assert((XMLTreeNode::NODE_TYPE_ELEMENT | XMLTreeNode::NODE_TYPE_TEXT | XMLTreeNode::NODE_TYPE_COMMENT | XMLTreeNode::NODE_TYPE_DOCTYPE) & child->type);

  if ((XMLTreeNode::NODE_TYPE_COMMENT | XMLTreeNode::NODE_TYPE_DOCTYPE) & child->type)
    return;

  assert(child->parent == node);

  if (XMLTreeNode::NODE_TYPE_ELEMENT == child->type)
    --node->elements;

  if (child->prev)
    child->prev->next = child->next;
  if (child->next)
    child->next->prev = child->prev;

  if (node->first == child)
    node->first = child->next;
  if (node->last == child)
    node->last = child->prev;

  assert((bool(node->first) && bool(node->last)) || (!node->first && !node->last));

  unlink(child);
}

void doReparentChildren(const XMLTreeNodePtr_t &source, const XMLTreeNodePtr_t &target)
{
  assert(bool(source));
  assert(bool(target));
  assert(XMLTreeNode::NODE_TYPE_ELEMENT == source->type);
  assert((XMLTreeNode::NODE_TYPE_ELEMENT | XMLTreeNode::NODE_TYPE_DOCUMENT) & target->type);

  if (!source->first) // no children
    return;

  if (bool(target->last))
    target->last->next = source->first;
  else
    target->first = source->first;
  source->first->prev = target->last;
  target->last = source->last;

  target->elements += source->elements;

  for (XMLTreeNodePtr_t current = source->first; bool(current); current = current->next)
    current->parent = target;

  source->first.reset();
  source->last.reset();

  source->elements = 0;

  assert(bool(target->first) && bool(target->last));
  assert(!source->first && !source->last);
}

void doAddAttributes(const XMLTreeNodePtr_t &node, const deque<XMLTreeNode::AttributeData> &attributes)
{
  assert(bool(node));
  assert(XMLTreeNode::NODE_TYPE_ELEMENT == node->type);

  if (attributes.empty())
    return;

  for (deque<XMLTreeNode::AttributeData>::const_iterator it = attributes.begin(); attributes.end() != it; ++it)
  {
    if (node->attributes.end() == find_if(node->attributes.begin(), node->attributes.end(), MatchAttribute(*it)))
      node->attributes.push_back(*it);
  }
}

// copying

XMLTreeNodePtr_t doClone(const XMLTreeNodePtr_t &node, bool deep = false)
{
  assert(bool(node));

  XMLTreeNodePtr_t copy(new XMLTreeNode(node->type));

  // NOTE: there is nothing to do for comments
  if ((XMLTreeNode::NODE_TYPE_ELEMENT | XMLTreeNode::NODE_TYPE_DOCUMENT) & node->type)
  {
    if (XMLTreeNode::NODE_TYPE_ELEMENT == node->type)
    {
      copy->self = node->self;
      copy->attributes = node->attributes;
    }
    copy->configuration = node->configuration;

    if (deep)
    {
      for (XMLTreeNodePtr_t current = node->first; bool(current); current = current->next)
        doAppendChild(copy, doClone(current, deep));
    }
  }
  else if (XMLTreeNode::NODE_TYPE_TEXT == node->type)
  {
    copy->text = node->text;
  }

  return copy;
}

// destroying

void destroyTree(XMLTreeNode *const root);

void destroyTree(const XMLTreeNodePtr_t &root)
{
  assert(bool(root));

  destroyTree(root.get());
}

void destroyTree(XMLTreeNode *const root)
{
  assert(bool(root));

  unlink(root);

  if ((XMLTreeNode::NODE_TYPE_ELEMENT | XMLTreeNode::NODE_TYPE_DOCUMENT) & root->type)
  {
    XMLTreeNodePtr_t current = root->first;
    root->first.reset();
    root->last.reset();

    root->elements = 0;

    for (; bool(current); current = current->next)
      destroyTree(current);
  }
}

}

XMLTreeNode::QName::QName()
  : name()
  , ns()
{
}

XMLTreeNode::QName::QName(const LWCStringPtr_t &name_, const LWCStringPtr_t &ns_)
  : name(name_)
  , ns(ns_)
{
}

XMLTreeNode::Configuration::Configuration(const bool caseless_)
  : caseless(caseless_)
  , classes()
  , ids()
  , langs()
  , links()
{
}

XMLTreeNode::ElementData::ElementData()
  : qname()
  , id(0)
  , idSet(false)
{
}

XMLTreeNode::ElementData::ElementData(const LWCStringPtr_t &name, const LWCStringPtr_t &ns)
  : qname(name, ns)
  , id(0)
  , idSet(false)
{
}

XMLTreeNode::AttributeData::AttributeData()
  : qname()
  , value()
  , id(0)
  , idSet(false)
{
}

XMLTreeNode::AttributeData::AttributeData(const LWCStringPtr_t &name, const LWCStringPtr_t &ns, const LWCStringPtr_t &val)
  : qname(name, ns)
  , value(val)
  , id(0)
  , idSet(false)
{
}

XMLTreeNode::AttributeData::AttributeData(const LWCStringPtr_t &name, const LWCStringPtr_t &val)
  : qname(name, LWCStringPtr_t())
  , value(val)
  , id(0)
  , idSet(false)
{
}

XMLTreeNode::XMLTreeNode(const NodeType nodeType, const ConfigurationPtr_t &config)
  : type(nodeType)
  , configuration(config)
  , self()
  , attributes()
  , text()
  , parent()
  , prev()
  , next()
  , first()
  , last()
  , elements(0)
  , refcount(0)
{
}

XMLTreeNode::~XMLTreeNode()
{
  if (XMLTreeNode::NODE_TYPE_DOCUMENT == type)
    destroyTree(this);
}

XMLTreeNodePtr_t XMLTreeNode::clone(bool deep) const
{
  const XMLTreeNodePtr_t that(const_cast<XMLTreeNode *>(this));
  return doClone(that, deep);
}

XMLTreeNodePtr_t XMLTreeNode::appendChild(const XMLTreeNodePtr_t &child)
{
  const XMLTreeNodePtr_t that(this);
  return doAppendChild(that, child);
}

XMLTreeNodePtr_t XMLTreeNode::insertChildBefore(const XMLTreeNodePtr_t &child, const XMLTreeNodePtr_t &selected)
{
  const XMLTreeNodePtr_t that(this);
  return doInsertChildBefore(that, child, selected);
}

void XMLTreeNode::removeChild(const XMLTreeNodePtr_t &child)
{
  const XMLTreeNodePtr_t that(this);
  doRemoveChild(that, child);
}

void XMLTreeNode::transferChildren(const XMLTreeNodePtr_t &newParent)
{
  const XMLTreeNodePtr_t that(this);
  doReparentChildren(that, newParent);
}

void XMLTreeNode::addAttributes(const std::deque<XMLTreeNode::AttributeData> &attrs)
{
  const XMLTreeNodePtr_t that(this);
  doAddAttributes(that, attrs);
}

bool XMLTreeNode::hasChildren(const bool ignoreText) const
{
  const XMLTreeNodePtr_t that(const_cast<XMLTreeNode *>(this));
  return doHasChildren(that, ignoreText);
}

bool operator==(const XMLTreeNode::QName &lhs, const XMLTreeNode::QName &rhs)
{
  return (lhs.name == rhs.name) && (lhs.ns == rhs.ns);
}

bool operator!=(const XMLTreeNode::QName &lhs, const XMLTreeNode::QName &rhs)
{
  return !(lhs == rhs);
}

bool caselessEqual(const XMLTreeNode::QName &lhs, const XMLTreeNode::QName &rhs)
{
  bool matchName = false;
  const lwc_error errName = lwc_string_caseless_isequal(lhs.name.get(), rhs.name.get(), &matchName);
  bool matchNs = false;
  const lwc_error errNs = lwc_string_caseless_isequal(lhs.ns.get(), rhs.ns.get(), &matchNs);

  if ((lwc_error_ok != errName) || (lwc_error_ok != errNs))
    throw std::bad_alloc();

  return matchName && matchNs;
}

bool operator==(const XMLTreeNode::AttributeData &lhs, const XMLTreeNode::AttributeData &rhs)
{
  return (lhs.qname == rhs.qname) && (lhs.value == rhs.value);
}

bool operator!=(const XMLTreeNode::AttributeData &lhs, const XMLTreeNode::AttributeData &rhs)
{
  return !(lhs == rhs);
}

bool operator==(const XMLTreeNode::ElementData &lhs, const XMLTreeNode::ElementData &rhs)
{
  return lhs.qname == rhs.qname;
}

bool operator!=(const XMLTreeNode::ElementData &lhs, const XMLTreeNode::ElementData &rhs)
{
  return !(lhs == rhs);
}

void intrusive_ptr_add_ref(XMLTreeNode *const p)
{
  ++p->refcount;
}

void intrusive_ptr_release(XMLTreeNode *const p)
{
  if (0 == --p->refcount)
    delete p;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
