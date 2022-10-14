/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <cassert>
#include <cstring>
#include <deque>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/split.hpp>

#include "libebook_libcss.h"
#include "libebook_utils.h"
#include "XMLTreeWalker.h"

using boost::is_any_of;
using std::shared_ptr;
using boost::split;
using boost::token_compress_on;

using std::bad_alloc;
using std::deque;
using std::find_if;
using std::malloc;
using std::size_t;
using std::string;

namespace libebook
{

namespace
{

bool operator==(const string &lhs, const lwc_string *rhs)
{
  if (!rhs)
    return 0 == lhs.size();
  return (lhs.size() == lwc_string_length(rhs)) && (equal(lhs.begin(), lhs.end(), lwc_string_data(rhs)));
}

bool operator==(const string &lhs, const LWCStringPtr_t &rhs)
{
  return lhs == rhs.get();
}

LWCStringPtr_t makeLWCString(const char *const str, const size_t len)
{
  LWCStringPtr_t result;

  lwc_string *ls = 0;
  if (lwc_error_ok == lwc_intern_string(str, len, &ls))
    result = wrap(ls);

  return result;
}

LWCStringPtr_t makeLWCString(const string &s)
{
  return makeLWCString(s.data(), s.size());
}

const string makeString(const lwc_string *const str)
{
  string result;
  if (bool(str))
    result.assign(lwc_string_data(str), lwc_string_length(str));
  return result;
}

const string makeString(const LWCStringPtr_t &str)
{
  return makeString(str.get());
}

bool matchQName(const XMLTreeNode::QName &lhs, const XMLTreeNode::QName &rhs, const bool caseless, bool &match)
{
  if (caseless)
  {
    try
    {
      match = caselessEqual(lhs, rhs);
    }
    catch (const bad_alloc &)
    {
      return false;
    }
  }
  else
  {
    match = lhs == rhs;
  }

  return true;
}

bool matchLWCString(const LWCStringPtr_t &lhs, const LWCStringPtr_t &rhs, const bool caseless, bool &match)
{
  if (!lhs && !rhs)
  {
    match = true;
    return true;
  }
  else if (!lhs || !rhs)
  {
    match = false;
    return true;
  }

  if (caseless)
  {
    if (lwc_error_ok != lwc_string_caseless_isequal(lhs.get(), rhs.get(), &match))
      return false;
  }
  else
  {
    match = lhs == rhs;
  }

  return true;
}

struct MatchString
{
  MatchString(const string &str, const bool caseless)
    : m_str(str)
    , m_caseless(caseless)
  {
  }

  bool operator()(const string &str)
  {
    if (m_caseless)
      return boost::iequals(m_str, str);
    return m_str == str;
  }

private:
  const string &m_str;
  const bool m_caseless;
};

struct MatchAttribute
{
  MatchAttribute(const XMLTreeNode::QName &qname, const bool caseless)
    : m_qname(qname)
    , m_caseless(caseless)
  {
  }

  bool operator()(const XMLTreeNode::AttributeData &attr)
  {
    if (m_caseless)
      return caselessEqual(m_qname, attr.qname);
    return m_qname == attr.qname;
  }

private:
  const XMLTreeNode::QName &m_qname;
  const bool m_caseless;
};

lwc_string *release(const LWCStringPtr_t &p)
{
  intrusive_ptr_add_ref(p.get());
  return p.get();
}

XMLTreeNodePtr_t parentElement(const XMLTreeNodePtr_t node)
{
  XMLTreeNodePtr_t parent;

  if (bool(node->parent) && (XMLTreeNode::NODE_TYPE_ELEMENT == node->parent->type))
    parent = node->parent;

  return parent;
}

XMLTreeNodePtr_t nextElement(const XMLTreeNodePtr_t node)
{
  XMLTreeNodePtr_t next = node->next;

  while (bool(next) && (XMLTreeNode::NODE_TYPE_ELEMENT != next->type))
    next = next->next;

  return next;
}

XMLTreeNodePtr_t previousElement(const XMLTreeNodePtr_t node)
{
  XMLTreeNodePtr_t prev = node->prev;

  while (bool(prev) && (XMLTreeNode::NODE_TYPE_ELEMENT != prev->type))
    prev = prev->prev;

  return prev;
}

const XMLTreeNode::QName makeQName(const css_qname &qname)
{
  return XMLTreeNode::QName(LWCStringPtr_t(qname.name), LWCStringPtr_t(qname.ns));
}

bool findAttribute(const XMLTreeNodePtr_t &node, const XMLTreeNode::QName &qname, deque<XMLTreeNode::AttributeData>::const_iterator &out) try
{
  out = find_if(node->attributes.begin(), node->attributes.end(), MatchAttribute(qname, node->configuration->caseless));
  return true;
}
catch (const bad_alloc &)
{
  return false;
}

bool matchDash(const XMLTreeNodePtr_t &node, const XMLTreeNode::QName &qname, const LWCStringPtr_t &value, bool &match)
{
  assert(bool(node->configuration));

  match = false;

  deque<XMLTreeNode::AttributeData>::const_iterator it;
  if (!findAttribute(node, qname, it))
    return false;

  if (node->attributes.end() != it)
  {
    deque<string> values;
    split(values, makeString(it->value), is_any_of("-"));

    if (node->configuration->caseless)
      match = boost::iequals(values.front(), makeString(value));
    else
      match = values.front() == value;
  }

  return true;
}

}

namespace
{

extern "C"
{

  // libcss selector callback decls

  css_error css_node_name(void *pw, void *node,
                          css_qname *qname);
  css_error css_node_classes(void *pw, void *node,
                             lwc_string ***classes,
                             uint32_t *n_classes);
  css_error css_node_id(void *pw, void *node,
                        lwc_string **id);

  css_error css_named_ancestor_node(void *pw, void *node,
                                    const css_qname *qname, void **ancestor);
  css_error css_named_parent_node(void *pw, void *node,
                                  const css_qname *qname, void **parent);
  css_error css_named_sibling_node(void *pw, void *node,
                                   const css_qname *qname, void **sibling);
  css_error css_named_generic_sibling_node(void *pw, void *node,
                                           const css_qname *qname, void **sibling);

  css_error css_parent_node(void *pw, void *node, void **parent);
  css_error css_sibling_node(void *pw, void *node, void **sibling);

  css_error css_node_has_name(void *pw, void *node,
                              const css_qname *qname, bool *match);
  css_error css_node_has_class(void *pw, void *node,
                               lwc_string *name, bool *match);
  css_error css_node_has_id(void *pw, void *node,
                            lwc_string *name, bool *match);
  css_error css_node_has_attribute(void *pw, void *node,
                                   const css_qname *qname, bool *match);
  css_error css_node_has_attribute_equal(void *pw, void *node,
                                         const css_qname *qname, lwc_string *value,
                                         bool *match);
  css_error css_node_has_attribute_dashmatch(void *pw, void *node,
                                             const css_qname *qname, lwc_string *value,
                                             bool *match);
  css_error css_node_has_attribute_includes(void *pw, void *node,
                                            const css_qname *qname, lwc_string *value,
                                            bool *match);
  css_error css_node_has_attribute_prefix(void *pw, void *node,
                                          const css_qname *qname, lwc_string *value,
                                          bool *match);
  css_error css_node_has_attribute_suffix(void *pw, void *node,
                                          const css_qname *qname, lwc_string *value,
                                          bool *match);
  css_error css_node_has_attribute_substring(void *pw, void *node,
                                             const css_qname *qname, lwc_string *value,
                                             bool *match);

  css_error css_node_is_root(void *pw, void *node, bool *match);
  css_error css_node_count_siblings(void *pw, void *node,
                                    bool same_name, bool after, int32_t *count);
  css_error css_node_is_empty(void *pw, void *node, bool *match);

  css_error css_node_is_link(void *pw, void *node, bool *match);
  css_error css_node_is_visited(void *pw, void *node, bool *match);
  css_error css_node_is_hover(void *pw, void *node, bool *match);
  css_error css_node_is_active(void *pw, void *node, bool *match);
  css_error css_node_is_focus(void *pw, void *node, bool *match);

  css_error css_node_is_enabled(void *pw, void *node, bool *match);
  css_error css_node_is_disabled(void *pw, void *node, bool *match);
  css_error css_node_is_checked(void *pw, void *node, bool *match);

  css_error css_node_is_target(void *pw, void *node, bool *match);
  css_error css_node_is_lang(void *pw, void *node,
                             lwc_string *lang, bool *match);

  css_error css_node_presentational_hint(void *pw, void *node,
                                         uint32_t *nhints, css_hint **hints);

  css_error css_ua_default_for_property(void *pw, uint32_t property,
                                        css_hint *hint);

  css_error css_compute_font_size(void *pw, const css_hint *parent,
                                  css_hint *size);

}

extern "C"
{

  css_error css_node_name(void *, void *const node, css_qname *const qname)
  {
    const XMLTreeNodePtr_t nd(reinterpret_cast<XMLTreeNode *>(node));

    assert(bool(nd));
    assert(qname);
    assert(XMLTreeNode::NODE_TYPE_ELEMENT == nd->type);

    qname->name = nd->self.qname.name.get();
    qname->ns = nd->self.qname.ns.get();

    return CSS_OK;
  }

  css_error css_node_classes(void *, void *node, lwc_string ***const classes, uint32_t *const n_classes)
  {
    const XMLTreeNodePtr_t nd(reinterpret_cast<XMLTreeNode *>(node));

    assert(bool(nd));
    assert(bool(nd->configuration));
    assert(XMLTreeNode::NODE_TYPE_ELEMENT == nd->type);
    assert(classes);
    assert(n_classes);

    *classes = 0;
    *n_classes = 0;

    if (nd->attributes.empty())
      return CSS_OK;

    for (deque<XMLTreeNode::QName>::const_iterator cls = nd->configuration->classes.begin(); nd->configuration->classes.end() != cls; ++cls)
    {
      deque<XMLTreeNode::AttributeData>::const_iterator it;

      if (!findAttribute(nd, *cls, it))
        return CSS_NOMEM;

      if (nd->attributes.end() != it)
      {
        deque<string> classNames;
        split(classNames, makeString(it->value), is_any_of(" \t\n\r"), token_compress_on);

        deque<LWCStringPtr_t> clsList;

        for (deque<string>::const_iterator cit = classNames.begin(); classNames.end() != cit; ++cit)
        {
          const LWCStringPtr_t s = makeLWCString(*cit);
          if (!s)
            return CSS_NOMEM;
          clsList.push_back(s);
        }

        *classes = reinterpret_cast<lwc_string **>(malloc(sizeof(lwc_string *) * clsList.size()));
        if (!*classes)
          return CSS_NOMEM;

        size_t i = 0;
        for (deque<LWCStringPtr_t>::const_iterator sit = clsList.begin(); clsList.end() != sit; ++sit, ++i)
          (*classes)[i] = release(*sit);
        *n_classes = clsList.size();

        break;
      }
    }

    return CSS_OK;
  }

  css_error css_node_id(void *, void *const node, lwc_string **const id)
  {
    const XMLTreeNodePtr_t nd(reinterpret_cast<XMLTreeNode *>(node));

    assert(bool(nd));
    assert(bool(nd->configuration));
    assert(XMLTreeNode::NODE_TYPE_ELEMENT == nd->type);
    assert(id);

    *id = 0;

    if (nd->attributes.empty())
      return CSS_OK;

    for (deque<XMLTreeNode::QName>::const_iterator it = nd->configuration->ids.begin(); nd->configuration->ids.end() != it; ++it)
    {
      deque<XMLTreeNode::AttributeData>::const_iterator match;

      if (!findAttribute(nd, *it, match))
        return CSS_NOMEM;

      if (nd->attributes.end() != match)
      {
        *id = match->value.get();
        break;
      }
    }

    return CSS_OK;
  }

  css_error css_named_ancestor_node(void *, void *const node, const css_qname *const qname, void **const ancestor)
  {
    const XMLTreeNodePtr_t nd(reinterpret_cast<XMLTreeNode *>(node));

    assert(bool(nd));
    assert(bool(nd->configuration));
    assert(qname);
    assert(ancestor);

    const XMLTreeNodePtr_t p = parentElement(nd);

    if (!p)
      return CSS_BADPARM;

    *ancestor = 0;

    for (XMLTreeNodePtr_t cur = p; bool(cur); cur = parentElement(cur))
    {
      bool match = false;

      if (!matchQName(cur->self.qname, makeQName(*qname), nd->configuration->caseless, match))
        return CSS_NOMEM;

      if (match)
      {
        *ancestor = cur.get();
        break;
      }
    }

    return CSS_OK;
  }

  css_error css_named_parent_node(void *, void *const node, const css_qname *const qname, void **const parent)
  {
    const XMLTreeNodePtr_t nd(reinterpret_cast<XMLTreeNode *>(node));

    assert(bool(nd));
    assert(bool(nd->configuration));
    assert(qname);
    assert(parent);

    const XMLTreeNodePtr_t p = parentElement(nd);

    if (!p)
      return CSS_BADPARM;

    *parent = 0;

    bool match = false;

    if (!matchQName(nd->self.qname, makeQName(*qname), nd->configuration->caseless, match))
      return CSS_NOMEM;

    if (match)
      *parent = p.get();

    return CSS_OK;
  }

  css_error css_named_sibling_node(void *, void *const node, const css_qname *const qname, void **const sibling)
  {
    const XMLTreeNodePtr_t nd(reinterpret_cast<XMLTreeNode *>(node));

    assert(bool(nd));
    assert(bool(nd->configuration));
    assert(qname);
    assert(sibling);

    *sibling = 0;

    const XMLTreeNodePtr_t sib = nextElement(nd);
    if (bool(sib))
    {
      bool match = false;

      if (!matchQName(sib->self.qname, makeQName(*qname), nd->configuration->caseless, match))
        return CSS_NOMEM;

      if (match)
        *sibling = sib.get();
    }

    return CSS_OK;
  }

  css_error css_named_generic_sibling_node(void *, void *const node, const css_qname *const qname, void **const sibling)
  {
    const XMLTreeNodePtr_t nd(reinterpret_cast<XMLTreeNode *>(node));

    assert(bool(nd));
    assert(bool(nd->configuration));
    assert(qname);
    assert(sibling);

    *sibling = 0;

    const XMLTreeNode::QName qname_ = makeQName(*qname);

    for (XMLTreeNodePtr_t cur = nextElement(nd); bool(cur); cur = nextElement(cur))
    {
      bool match = false;

      if (!matchQName(cur->self.qname, qname_, nd->configuration->caseless, match))
        return CSS_NOMEM;

      if (match)
      {
        *sibling = cur.get();
        break;
      }
    }

    return CSS_OK;
  }

  css_error css_parent_node(void *, void *const node, void **const parent)
  {
    const XMLTreeNodePtr_t nd(reinterpret_cast<XMLTreeNode *>(node));

    assert(bool(nd));
    assert(parent);

    const XMLTreeNodePtr_t p = parentElement(nd);

    if (!p)
      return CSS_BADPARM;

    *parent = p.get();

    return CSS_OK;
  }

  css_error css_sibling_node(void *, void *const node, void **const sibling)
  {
    const XMLTreeNodePtr_t nd(reinterpret_cast<XMLTreeNode *>(node));

    assert(bool(nd));
    assert(sibling);
    assert(XMLTreeNode::NODE_TYPE_ELEMENT == nd->type);

    *sibling = nextElement(nd).get();

    return CSS_OK;
  }

  css_error css_node_has_name(void *, void *const node, const css_qname *const qname, bool *const match)
  {
    const XMLTreeNodePtr_t nd(reinterpret_cast<XMLTreeNode *>(node));

    assert(bool(nd));
    assert(bool(nd->configuration));
    assert(XMLTreeNode::NODE_TYPE_ELEMENT == nd->type);
    assert(qname);
    assert(match);

    if (!matchQName(nd->self.qname, makeQName(*qname), nd->configuration->caseless, *match))
      return CSS_NOMEM;

    return CSS_OK;
  }

  css_error css_node_has_class(void *, void *const node, lwc_string *const name, bool *const match)
  {
    const XMLTreeNodePtr_t nd(reinterpret_cast<XMLTreeNode *>(node));

    assert(bool(nd));
    assert(bool(nd->configuration));
    assert(XMLTreeNode::NODE_TYPE_ELEMENT == nd->type);
    assert(name);
    assert(match);

    *match = false;

    for (deque<XMLTreeNode::QName>::const_iterator cls = nd->configuration->classes.begin(); nd->configuration->classes.end() != cls; ++cls)
    {
      deque<XMLTreeNode::AttributeData>::const_iterator it;

      if (!findAttribute(nd, *cls, it))
        return CSS_NOMEM;

      if (nd->attributes.end() != it)
      {
        deque<string> classNames;
        split(classNames, makeString(it->value), is_any_of(" \t\n\r"), token_compress_on);

        const string n(lwc_string_data(name), lwc_string_length(name));
        *match = classNames.end() != find_if(classNames.begin(), classNames.end(),
                                             MatchString(n, nd->configuration->caseless));

        break;
      }
    }

    return CSS_OK;
  }

  css_error css_node_has_id(void *, void *const node, lwc_string *const name, bool *const match)
  {
    const XMLTreeNodePtr_t nd(reinterpret_cast<XMLTreeNode *>(node));

    assert(bool(nd));
    assert(XMLTreeNode::NODE_TYPE_ELEMENT == nd->type);
    assert(name);
    assert(match);

    *match = false;

    for (deque<XMLTreeNode::QName>::const_iterator id = nd->configuration->ids.begin(); nd->configuration->ids.end() != id; ++id)
    {
      deque<XMLTreeNode::AttributeData>::const_iterator it;

      if (!findAttribute(nd, *id, it))
        return CSS_NOMEM;

      if (nd->attributes.end() != it)
      {
        // TODO: caseless?
        *match = it->value.get() == name;
        break;
      }
    }

    return CSS_OK;
  }

  css_error css_node_has_attribute(void *, void *const node, const css_qname *const qname, bool *const match)
  {
    const XMLTreeNodePtr_t nd(reinterpret_cast<XMLTreeNode *>(node));

    assert(bool(nd));
    assert(bool(nd->configuration));
    assert(XMLTreeNode::NODE_TYPE_ELEMENT == nd->type);
    assert(qname);
    assert(match);

    deque<XMLTreeNode::AttributeData>::const_iterator it;

    if (!findAttribute(nd, makeQName(*qname), it))
      return CSS_NOMEM;

    *match = nd->attributes.end() != it;

    return CSS_OK;
  }

  css_error css_node_has_attribute_equal(void *, void *const node, const css_qname *const qname, lwc_string *const value, bool *const match)
  {
    const XMLTreeNodePtr_t nd(reinterpret_cast<XMLTreeNode *>(node));

    assert(bool(nd));
    assert(bool(nd->configuration));
    assert(XMLTreeNode::NODE_TYPE_ELEMENT == nd->type);
    assert(qname);
    assert(value);
    assert(match);

    *match = false;

    deque<XMLTreeNode::AttributeData>::const_iterator it;

    if (!findAttribute(nd, makeQName(*qname), it))
      return CSS_NOMEM;

    if (nd->attributes.end() != it)
    {
      if (!matchLWCString(it->value, LWCStringPtr_t(value), nd->configuration->caseless, *match))
        return CSS_NOMEM;
    }

    return CSS_OK;
  }

  css_error css_node_has_attribute_dashmatch(void *, void *const node,
                                             const css_qname *const qname, lwc_string *const value,
                                             bool *const match)
  {
    const XMLTreeNodePtr_t nd(reinterpret_cast<XMLTreeNode *>(node));

    assert(bool(nd));
    assert(bool(nd->configuration));
    assert(XMLTreeNode::NODE_TYPE_ELEMENT == nd->type);
    assert(qname);
    assert(value);
    assert(match);

    if (!matchDash(nd, makeQName(*qname), LWCStringPtr_t(value), *match))
      return CSS_NOMEM;

    return CSS_OK;
  }

  css_error css_node_has_attribute_includes(void *, void *const node,
                                            const css_qname *const qname, lwc_string *const value,
                                            bool *const match)
  {
    const XMLTreeNodePtr_t nd(reinterpret_cast<XMLTreeNode *>(node));

    assert(bool(nd));
    assert(bool(nd->configuration));
    assert(XMLTreeNode::NODE_TYPE_ELEMENT == nd->type);
    assert(qname);
    assert(value);
    assert(match);

    *match = false;

    deque<XMLTreeNode::AttributeData>::const_iterator it;

    if (!findAttribute(nd, makeQName(*qname), it))
      return CSS_NOMEM;

    if (nd->attributes.end() != it)
    {
      deque<string> values;
      split(values, makeString(it->value), is_any_of(" \t\n\r"), token_compress_on);

      const string v(lwc_string_data(value), lwc_string_length(value));
      *match = values.end() != find_if(values.begin(), values.end(), MatchString(v, nd->configuration->caseless));
    }

    return CSS_OK;
  }
  css_error css_node_has_attribute_prefix(void *, void *const node,
                                          const css_qname *const qname, lwc_string *const value,
                                          bool *const match)
  {
    const XMLTreeNodePtr_t nd(reinterpret_cast<XMLTreeNode *>(node));

    assert(bool(nd));
    assert(bool(nd->configuration));
    assert(XMLTreeNode::NODE_TYPE_ELEMENT == nd->type);
    assert(qname);
    assert(value);
    assert(match);

    *match = false;

    deque<XMLTreeNode::AttributeData>::const_iterator it;

    if (!findAttribute(nd, makeQName(*qname), it))
      return CSS_NOMEM;

    if (nd->attributes.end() != it)
    {
      if (nd->configuration->caseless)
        *match = boost::istarts_with(makeString(it->value), makeString(value));
      else
        *match = boost::starts_with(makeString(it->value), makeString(value));
    }

    return CSS_OK;
  }

  css_error css_node_has_attribute_suffix(void *, void *const node,
                                          const css_qname *const qname, lwc_string *const value,
                                          bool *const match)
  {
    const XMLTreeNodePtr_t nd(reinterpret_cast<XMLTreeNode *>(node));

    assert(bool(nd));
    assert(bool(nd->configuration));
    assert(XMLTreeNode::NODE_TYPE_ELEMENT == nd->type);
    assert(qname);
    assert(value);
    assert(match);

    *match = false;

    deque<XMLTreeNode::AttributeData>::const_iterator it;

    if (!findAttribute(nd, makeQName(*qname), it))
      return CSS_NOMEM;

    if (nd->attributes.end() != it)
    {
      if (nd->configuration->caseless)
        *match = boost::iends_with(makeString(it->value), makeString(value));
      else
        *match = boost::ends_with(makeString(it->value), makeString(value));
    }

    return CSS_OK;
  }

  css_error css_node_has_attribute_substring(void *, void *const node,
                                             const css_qname *const qname, lwc_string *const value,
                                             bool *const match)
  {
    const XMLTreeNodePtr_t nd(reinterpret_cast<XMLTreeNode *>(node));

    assert(bool(nd));
    assert(bool(nd->configuration));
    assert(XMLTreeNode::NODE_TYPE_ELEMENT == nd->type);
    assert(qname);
    assert(value);
    assert(match);

    *match = false;

    deque<XMLTreeNode::AttributeData>::const_iterator it;

    if (!findAttribute(nd, makeQName(*qname), it))
      return CSS_NOMEM;

    if (nd->attributes.end() != it)
    {
      if (nd->configuration->caseless)
        *match = boost::icontains(makeString(it->value), makeString(value));
      else
        *match = boost::contains(makeString(it->value), makeString(value));
    }

    return CSS_OK;
  }

  css_error css_node_is_root(void *, void *const node, bool *const match)
  {
    const XMLTreeNodePtr_t nd(reinterpret_cast<XMLTreeNode *>(node));

    assert(bool(nd));
    assert(XMLTreeNode::NODE_TYPE_ELEMENT == nd->type);
    assert(match);

    if (!nd->parent)
      return CSS_BADPARM;

    *match = XMLTreeNode::NODE_TYPE_DOCUMENT == nd->parent->type;

    return CSS_OK;
  }

  css_error css_node_count_siblings(void *, void *const node, const bool same_name, const bool after, int32_t *const count)
  {
    const XMLTreeNodePtr_t nd(reinterpret_cast<XMLTreeNode *>(node));

    assert(bool(nd));
    assert(XMLTreeNode::NODE_TYPE_ELEMENT == nd->type);
    assert(same_name);
    assert(after);
    assert(count);

    int32_t cnt = 0;

    if (after)
    {
      for (XMLTreeNodePtr_t cur = previousElement(nd); bool(cur); cur = previousElement(cur))
      {
        if (same_name && (nd->self != cur->self))
          continue;
        ++cnt;
      }
    }
    else
    {
      for (XMLTreeNodePtr_t cur = nextElement(nd); bool(cur); cur = nextElement(cur))
      {
        if (same_name && (nd->self != cur->self))
          continue;

        ++cnt;
      }
    }

    *count = cnt;

    return CSS_OK;
  }

  css_error css_node_is_empty(void *, void *const node, bool *const match)
  {
    const XMLTreeNodePtr_t nd(reinterpret_cast<XMLTreeNode *>(node));

    assert(bool(nd));
    assert(XMLTreeNode::NODE_TYPE_ELEMENT == nd->type);
    assert(match);

    *match = !nd->first;

    return CSS_OK;
  }

  css_error css_node_is_link(void *, void *const node, bool *const match)
  {
    const XMLTreeNodePtr_t nd(reinterpret_cast<XMLTreeNode *>(node));

    assert(bool(nd));
    assert(bool(nd->configuration));
    assert(XMLTreeNode::NODE_TYPE_ELEMENT == nd->type);
    assert(match);

    for (deque<XMLTreeNode::LinkId_t>::const_iterator link = nd->configuration->links.begin(); nd->configuration->links.end() != link; ++link)
    {
      assert(bool(link->first.name) || bool(link->second.name));

      if (bool(link->first.name))
      {
        if (!matchQName(link->first, nd->self.qname, nd->configuration->caseless, *match))
          return CSS_NOMEM;

        if (*match && !link->second.name) // there is no attribute -> we found it
          break;
        else if (!*match) // element does not match, no need to look for attribute
          continue;
      }

      // we can only get here if we need to check attribute
      assert(bool(link->second.name));

      deque<XMLTreeNode::AttributeData>::const_iterator it;

      if (!findAttribute(nd, link->second, it))
        return CSS_NOMEM;

      *match = nd->attributes.end() != it;

      if (*match)
        break;
    }

    return CSS_OK;
  }

  css_error css_node_is_visited(void *, void *, bool *const match)
  {
    assert(match);

    *match = false;

    return CSS_OK;
  }

  css_error css_node_is_hover(void *, void *, bool *const match)
  {
    assert(match);

    *match = false;

    return CSS_OK;
  }

  css_error css_node_is_active(void *, void *, bool *const match)
  {
    assert(match);

    *match = false;

    return CSS_OK;
  }

  css_error css_node_is_focus(void *, void *, bool *const match)
  {
    assert(match);

    *match = false;

    return CSS_OK;
  }

  css_error css_node_is_enabled(void *, void *, bool *const match)
  {
    assert(match);

    *match = false;

    return CSS_OK;
  }

  css_error css_node_is_disabled(void *, void *, bool *const match)
  {
    assert(match);

    *match = false;

    return CSS_OK;
  }

  css_error css_node_is_checked(void *, void *, bool *const match)
  {
    assert(match);

    *match = false;

    return CSS_OK;
  }

  css_error css_node_is_target(void *, void *, bool *const match)
  {
    assert(match);

    *match = false;

    return CSS_OK;
  }

  css_error css_node_is_lang(void *, void *const node, lwc_string *const lang, bool *const match)
  {
    const XMLTreeNodePtr_t nd(reinterpret_cast<XMLTreeNode *>(node));

    assert(bool(nd));
    assert(bool(nd->configuration));
    assert(XMLTreeNode::NODE_TYPE_ELEMENT == nd->type);
    assert(lang);
    assert(match);

    *match = false;

    for (XMLTreeNodePtr_t cur = nd; bool(cur); cur = parentElement(cur))
    {
      for (deque<XMLTreeNode::QName>::const_iterator it = nd->configuration->langs.begin(); nd->configuration->langs.end() != it; ++it)
      {
        if (!matchDash(cur, *it, LWCStringPtr_t(lang), *match))
          return CSS_NOMEM;

        if (*match)
          break;
      }

      if (*match)
        break;
    }

    return CSS_OK;
  }

  css_error css_node_presentational_hint(void *pw, void *const node, uint32_t *nhints, css_hint **const hints)
  {
    // TODO: implement me
    (void) pw;
    (void) node;
    (void) nhints;
    (void) hints;

    return CSS_PROPERTY_NOT_SET;
  }

  css_error css_ua_default_for_property(void *pw, const uint32_t property, css_hint *const hint)
  {
    // TODO: implement me
    (void) pw;
    (void) property;
    (void) hint;

    return CSS_PROPERTY_NOT_SET;
  }

  css_error css_compute_font_size(void *, const css_hint *const parent, css_hint *const size)
  {
    static css_hint_length sizes[] =
    {
      { FLTTOFIX(6.75), CSS_UNIT_PT }, // CSS_FONT_SIZE_XX_SMALL
      { FLTTOFIX(7.50), CSS_UNIT_PT }, // CSS_FONT_SIZE_X_SMALL
      { FLTTOFIX(9.75), CSS_UNIT_PT }, // CSS_FONT_SIZE_SMALL
      { FLTTOFIX(12.0), CSS_UNIT_PT }, // CSS_FONT_SIZE_MEDIUM
      { FLTTOFIX(13.5), CSS_UNIT_PT }, // CSS_FONT_SIZE_LARGE
      { FLTTOFIX(18.0), CSS_UNIT_PT }, // CSS_FONT_SIZE_X_LARGE
      { FLTTOFIX(24.0), CSS_UNIT_PT }  // CSS_FONT_SIZE_XX_LARGE
    };

    assert(CSS_FONT_SIZE_INHERIT != size->status);

    const css_hint_length *parentSize = 0;

    if (parent)
    {
      parentSize = &parent->data.length;
      assert(CSS_FONT_SIZE_DIMENSION == parent->status);
    }
    else
    {
      parentSize = &sizes[CSS_FONT_SIZE_MEDIUM - 1];
    }

    switch (size->status)
    {
    // predefined size
    case CSS_FONT_SIZE_XX_SMALL :
    case CSS_FONT_SIZE_X_SMALL :
    case CSS_FONT_SIZE_SMALL :
    case CSS_FONT_SIZE_MEDIUM :
    case CSS_FONT_SIZE_LARGE :
    case CSS_FONT_SIZE_X_LARGE :
    case CSS_FONT_SIZE_XX_LARGE :
      size->data.length = sizes[size->status - 1];
      size->status = CSS_FONT_SIZE_DIMENSION;
      break;

    // relative size
    case CSS_FONT_SIZE_LARGER :
      size->data.length.value = FMUL(parentSize->value, FLTTOFIX(1.2));
      size->data.length.unit = parentSize->unit;
      size->status = CSS_FONT_SIZE_DIMENSION;
      break;
    case CSS_FONT_SIZE_SMALLER :
      size->data.length.value = FDIV(parentSize->value, FLTTOFIX(1.2));
      size->data.length.unit = parentSize->unit;
      size->status = CSS_FONT_SIZE_DIMENSION;
      break;

    // exact size
    case CSS_FONT_SIZE_DIMENSION :
      switch (size->data.length.unit)
      {
      // absolute unit -> nothing needed
      case CSS_UNIT_PX :
      case CSS_UNIT_IN :
      case CSS_UNIT_CM :
      case CSS_UNIT_MM :
      case CSS_UNIT_PT :
      case CSS_UNIT_PC :
        break;

      // relative unit -> compute size relative to parent size
      case CSS_UNIT_EX :
        size->data.length.value = FMUL(FMUL(size->data.length.value, parentSize->value), FLTTOFIX(0.6));
        size->data.length.unit = parentSize->unit;
        break;
      case CSS_UNIT_EM :
        size->data.length.value = FMUL(size->data.length.value, parentSize->value);
        size->data.length.unit = parentSize->unit;
        break;
      case CSS_UNIT_PCT :
        size->data.length.value = FDIV(FMUL(size->data.length.value, parentSize->value), FLTTOFIX(100));
        size->data.length.unit = parentSize->unit;
        break;

      default :
        assert(0);
      }
      break;
    default :
      assert(0);
    }

    return CSS_OK;
  }

  css_error css_set_libcss_node_data(void *, void *, void *)
  {
    return CSS_OK;
  }

  css_error css_get_libcss_node_data(void *, void *, void **)
  {
    return CSS_OK;
  }
}

static css_select_handler cssSelectHandler =
{
  CSS_SELECT_HANDLER_VERSION_1,
  &css_node_name,
  &css_node_classes,
  &css_node_id,
  &css_named_ancestor_node,
  &css_named_parent_node,
  &css_named_sibling_node,
  &css_named_generic_sibling_node,
  &css_parent_node,
  &css_sibling_node,
  &css_node_has_name,
  &css_node_has_class,
  &css_node_has_id,
  &css_node_has_attribute,
  &css_node_has_attribute_equal,
  &css_node_has_attribute_dashmatch,
  &css_node_has_attribute_includes,
  &css_node_has_attribute_prefix,
  &css_node_has_attribute_suffix,
  &css_node_has_attribute_substring,
  &css_node_is_root,
  &css_node_count_siblings,
  &css_node_is_empty,
  &css_node_is_link,
  &css_node_is_visited,
  &css_node_is_hover,
  &css_node_is_active,
  &css_node_is_focus,
  &css_node_is_enabled,
  &css_node_is_disabled,
  &css_node_is_checked,
  &css_node_is_target,
  &css_node_is_lang,
  &css_node_presentational_hint,
  &css_ua_default_for_property,
  &css_compute_font_size,
  &css_set_libcss_node_data,
  &css_get_libcss_node_data
};

}

bool XMLTreeWalker::empty() const
{
  assert(bool(m_impl));
  assert((XMLTreeNode::NODE_TYPE_ELEMENT | XMLTreeNode::NODE_TYPE_DOCUMENT) & m_impl->type);

  return !m_impl->hasChildren();
}

XMLTreeWalker::Iterator XMLTreeWalker::begin() const
{
  assert(bool(m_impl));
  assert((XMLTreeNode::NODE_TYPE_ELEMENT | XMLTreeNode::NODE_TYPE_DOCUMENT) & m_impl->type);

  return Iterator(m_impl->first, m_tok);
}

const XMLTreeWalker::Iterator XMLTreeWalker::end() const
{
  assert(bool(m_impl));
  assert((XMLTreeNode::NODE_TYPE_ELEMENT | XMLTreeNode::NODE_TYPE_DOCUMENT) & m_impl->type);

  return Iterator(0, m_tok);
}

bool XMLTreeWalker::hasParent() const
{
  assert(bool(m_impl));

  return bool(m_impl->parent);
}

const XMLTreeWalker XMLTreeWalker::getParent() const
{
  assert(bool(m_impl));

  return XMLTreeWalker(m_impl->parent, m_tok);
}

bool XMLTreeWalker::isDocument() const
{
  assert(bool(m_impl));

  return XMLTreeNode::NODE_TYPE_DOCUMENT == m_impl->type;
}

bool XMLTreeWalker::isElement() const
{
  assert(bool(m_impl));

  return XMLTreeNode::NODE_TYPE_ELEMENT == m_impl->type;
}

bool XMLTreeWalker::isText() const
{
  assert(bool(m_impl));

  return XMLTreeNode::NODE_TYPE_TEXT == m_impl->type;
}

int XMLTreeWalker::getId() const
{
  assert(bool(m_impl));
  assert(XMLTreeNode::NODE_TYPE_ELEMENT == m_impl->type);

  if (!m_tok)
    return 0;

  if (!m_impl->self.idSet)
  {
    const int nameId = m_tok(lwc_string_data(m_impl->self.qname.name.get()), lwc_string_length(m_impl->self.qname.name.get()));
    int nsId = 0;

    if (bool(m_impl->self.qname.ns))
      nsId = m_tok(lwc_string_data(m_impl->self.qname.ns.get()), lwc_string_length(m_impl->self.qname.ns.get()));

    m_impl->self.id = nameId | nsId;
    m_impl->self.idSet = true;
  }

  return m_impl->self.id;
}

const std::string XMLTreeWalker::getName() const
{
  assert(bool(m_impl));
  assert(XMLTreeNode::NODE_TYPE_ELEMENT == m_impl->type);

  return makeString(m_impl->self.qname.name);
}

const std::string XMLTreeWalker::getNamespace() const
{
  assert(bool(m_impl));
  assert(XMLTreeNode::NODE_TYPE_ELEMENT == m_impl->type);

  return makeString(m_impl->self.qname.ns);
}

const std::string &XMLTreeWalker::getText() const
{
  assert(bool(m_impl));
  assert(XMLTreeNode::NODE_TYPE_TEXT == m_impl->type);

  return m_impl->text;
}

bool XMLTreeWalker::hasAttributes() const
{
  assert(bool(m_impl));
  assert(XMLTreeNode::NODE_TYPE_ELEMENT == m_impl->type);

  return !m_impl->attributes.empty();
}

XMLTreeWalker::AttributeIterator XMLTreeWalker::beginAttributes() const
{
  assert(bool(m_impl));
  assert(XMLTreeNode::NODE_TYPE_ELEMENT == m_impl->type);

  return AttributeIterator(m_impl, m_tok, false);
}

const XMLTreeWalker::AttributeIterator XMLTreeWalker::endAttributes() const
{
  assert(bool(m_impl));
  assert(XMLTreeNode::NODE_TYPE_ELEMENT == m_impl->type);

  return AttributeIterator(m_impl, m_tok, true);
}

XMLTreeWalker::XMLTreeWalker(const XMLTreeNodePtr_t &p, const TokenizerFun_t tok)
  : m_impl(p)
  , m_tok(tok)
{
}

int XMLTreeWalker::Attribute::getId() const
{
  if (!m_tok)
    return 0;

  const XMLTreeNode::AttributeData &attr = m_node->attributes[m_index];

  if (!attr.idSet)
  {
    const int nameId = m_tok(lwc_string_data(attr.qname.name.get()), lwc_string_length(attr.qname.name.get()));
    int nsId = 0;

    if (bool(attr.qname.ns))
      nsId = m_tok(lwc_string_data(attr.qname.ns.get()), lwc_string_length(attr.qname.ns.get()));

    attr.id = nameId | nsId;
    attr.idSet = true;
  }

  return attr.id;
}

int XMLTreeWalker::Attribute::getValueId() const
{
  if (!m_tok)
    return 0;

  const XMLTreeNode::AttributeData &attr = m_node->attributes[m_index];
  return m_tok(lwc_string_data(attr.value.get()), lwc_string_length(attr.value.get()));
}

const std::string XMLTreeWalker::Attribute::getName() const
{
  return makeString(m_node->attributes[m_index].qname.name);
}

const std::string XMLTreeWalker::Attribute::getNamespace() const
{
  return makeString(m_node->attributes[m_index].qname.ns);
}

const std::string XMLTreeWalker::Attribute::getValue() const
{
  return makeString(m_node->attributes[m_index].value);
}

bool XMLTreeWalker::Attribute::operator==(const XMLTreeWalker::Attribute &rhs) const
{
  return m_node == rhs.m_node && m_index == rhs.m_index;
}

XMLTreeWalker::Attribute::Attribute(const XMLTreeNodePtr_t &node, std::size_t index, const TokenizerFun_t tok)
  : m_node(node)
  , m_index(index)
  , m_tok(tok)
{
}

bool operator!=(const XMLTreeWalker::Attribute &lhs, const XMLTreeWalker::Attribute &rhs)
{
  return !(lhs == rhs);
}

XMLTreeWalker::AttributeIterator &XMLTreeWalker::AttributeIterator::operator++()
{
  if (m_index < m_node->attributes.size())
  {
    ++m_index;
    m_current = Attribute(m_node, m_index, m_tok);
  }

  return *this;
}

const XMLTreeWalker::AttributeIterator XMLTreeWalker::AttributeIterator::operator++(int)
{
  const AttributeIterator old(*this);
  ++(*this);
  return old;
}

const XMLTreeWalker::Attribute &XMLTreeWalker::AttributeIterator::operator*() const
{
  assert(m_node->attributes.size() > m_index);

  return m_current;
}

const XMLTreeWalker::Attribute *XMLTreeWalker::AttributeIterator::operator->() const
{
  assert(m_node->attributes.size() > m_index);

  return &m_current;
}

bool XMLTreeWalker::AttributeIterator::operator==(const XMLTreeWalker::AttributeIterator &rhs) const
{
  return m_node == rhs.m_node && m_index == rhs.m_index;
}

XMLTreeWalker::AttributeIterator::AttributeIterator(const XMLTreeNodePtr_t &node, const TokenizerFun_t tok, bool end)
  : m_node(node)
  , m_index(end ? node->attributes.size() : 0)
  , m_current(m_node, m_index, tok)
  , m_tok(tok)
{
  assert(bool(m_node));
}

bool operator!=(const XMLTreeWalker::AttributeIterator &lhs, const XMLTreeWalker::AttributeIterator &rhs)
{
  return !(lhs == rhs);
}

XMLTreeWalker::Iterator &XMLTreeWalker::Iterator::operator++()
{
  if (bool(m_node))
  {
    m_node = m_node->next;
    m_current = XMLTreeWalker(m_node, m_tok);
  }

  return *this;
}

const XMLTreeWalker::Iterator XMLTreeWalker::Iterator::operator++(int)
{
  const Iterator old(*this);
  ++(*this);
  return old;
}

const XMLTreeWalker &XMLTreeWalker::Iterator::operator*() const
{
  assert(bool(m_node));

  return m_current;
}

const XMLTreeWalker *XMLTreeWalker::Iterator::operator->() const
{
  assert(bool(m_node));

  return &m_current;
}

bool XMLTreeWalker::Iterator::operator==(const XMLTreeWalker::Iterator &rhs) const
{
  return m_node == rhs.m_node;
}

XMLTreeWalker::Iterator::Iterator(const XMLTreeNodePtr_t &node, const TokenizerFun_t tok)
  : m_node(node)
  , m_current(node, tok)
  , m_tok(tok)
{
}

bool operator!=(const XMLTreeWalker::Iterator &lhs, const XMLTreeWalker::Iterator &rhs)
{
  return !(lhs == rhs);
}

CSSSelectResultsPtr_t
XMLTreeWalker::getStyle(const XMLStylesheets_t &sheets, const bool quirks,
                        const CSSStylesheetPtr_t &inlineSheet) const
{
  assert(bool(m_impl));
  assert(XMLTreeNode::NODE_TYPE_ELEMENT == m_impl->type);

  css_select_ctx *cSelectCtx = 0;
  if (CSS_OK != css_select_ctx_create(&cSelectCtx))
    throw GenericException();
  const CSSSelectCtxPtr_t selectCtx = wrap(cSelectCtx);

  for (XMLStylesheets_t::const_iterator it = sheets.begin(); sheets.end() != it; ++it)
  {
    // TODO: use libcss origin in XMLStylesheet
    css_origin origin = CSS_ORIGIN_AUTHOR;
    switch (it->getSource())
    {
    case XMLStylesheet::SOURCE_UA :
      origin = CSS_ORIGIN_UA;
      break;
    case XMLStylesheet::SOURCE_USER :
      origin = CSS_ORIGIN_USER;
      break;
    case XMLStylesheet::SOURCE_AUTHOR :
      origin = CSS_ORIGIN_AUTHOR;
      break;
    default :
      assert(0);
    }

    if (it->isQuirks() == quirks)
    {
      // TODO: media should be in XMLStylesheet
      if (CSS_OK != css_select_ctx_append_sheet(selectCtx.get(), it->get().get(), origin, CSS_MEDIA_ALL))
        throw GenericException();
    }
  }

  css_select_results *cSelectResults = 0;
  // TODO: use CSS_MEDIA_PRINT?
  if (CSS_OK != css_select_style(selectCtx.get(), m_impl.get(), CSS_MEDIA_SCREEN, inlineSheet.get(), &cssSelectHandler, 0, &cSelectResults))
    throw GenericException();
  const CSSSelectResultsPtr_t selectResults = wrap(cSelectResults);

  return selectResults;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
