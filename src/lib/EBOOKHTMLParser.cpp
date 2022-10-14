/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <cassert>
#include <functional>
#include <stack>
#include <string>

#include "libebook_hubbub.h"
#include "libebook_utils.h"
#include "libebook_xml.h"
#include "EBOOKCharsetConverter.h"
#include "EBOOKHTMLToken.h"
#include "EBOOKHTMLTypes.h"
#include "EBOOKHTMLParser.h"
#include "EBOOKOutputElements.h"
#include "EBOOKUTF8Stream.h"
#include "XMLCollector.h"
#include "XMLStylesheet.h"

using std::shared_ptr;

using librevenge::RVNGPropertyList;

using std::bad_alloc;
using std::deque;
using std::stack;
using std::string;

#define STR_LIT(s) (s), (sizeof(s) - 1)

namespace libebook
{

namespace
{

struct ConstString
{
  const char *str;
  size_t len;
};

static const ConstString NS_NAME_MAP[] =
{
  { "", 0 }, // HUBBUB_NS_NULL
  { STR_LIT("http://www.w3.org/1999/xhtml") }, // HUBBUB_NS_HTML
  { STR_LIT("http://www.w3.org/1998/Math/MathML") }, // HUBBUB_NS_MATHML
  { STR_LIT("http://www.w3.org/2000/svg") }, // HUBBUB_NS_SVG
  { STR_LIT("http://www.w3.org/1999/xlink") }, // HUBBUB_NS_XLINK
  { STR_LIT("http://www.w3.org/XML/1998/namespace") }, // HUBBUB_NS_XML
  { STR_LIT("http://www.w3.org/2000/xmlns/") } // HUBBUB_NS_XMLNS
};

LWCStringPtr_t makeLWCString(const char *const str, const size_t len)
{
  LWCStringPtr_t result;

  lwc_string *ls = 0;
  if (lwc_error_ok == lwc_intern_string(str, len, &ls))
    result = wrap(ls);

  return result;
}

const LWCStringPtr_t makeLWCString(const hubbub_string &str)
{
  return makeLWCString(char_cast(str.ptr), str.len);
}

LWCStringPtr_t getNamespaceString(const hubbub_ns ns)
{
  LWCStringPtr_t uri;

  if ((EBOOK_NUM_ELEMENTS(NS_NAME_MAP) > ns) && (0 != NS_NAME_MAP[ns].len))
    uri = makeLWCString(NS_NAME_MAP[ns].str, NS_NAME_MAP[ns].len);

  return uri;
}

const XMLTreeNode::ElementData makeElement(const hubbub_tag &tag)
{
  const LWCStringPtr_t name = makeLWCString(tag.name);

  if (HUBBUB_NS_NULL == tag.ns)
    return XMLTreeNode::ElementData(name);

  const LWCStringPtr_t ns = getNamespaceString(tag.ns);
  return XMLTreeNode::ElementData(name, ns);
}

const XMLTreeNode::AttributeData makeAttribute(const hubbub_attribute &attr)
{
  const LWCStringPtr_t name = makeLWCString(attr.name);
  const LWCStringPtr_t value = makeLWCString(attr.value);

  if (HUBBUB_NS_NULL == attr.ns)
    return XMLTreeNode::AttributeData(name, value);

  const LWCStringPtr_t ns = getNamespaceString(attr.ns);

  return XMLTreeNode::AttributeData(name, ns, value);
}

const deque<XMLTreeNode::AttributeData> makeAttributes(const hubbub_attribute *attrs, const size_t count)
{
  if (0 != count)
    assert(attrs);

  deque<XMLTreeNode::AttributeData> attributes;

  for (size_t i = 0; count != i; ++i)
    attributes.push_back(makeAttribute(attrs[i]));

  return attributes;
}

}

namespace
{

extern "C"
{

  void *allocator(void *ptr, size_t size, void *pw);

// hubbub treebuilder callback decls

  hubbub_error html_create_comment(void *ctx, const hubbub_string *data, void **result);
  hubbub_error html_create_doctype(void *ctx, const hubbub_doctype *doctype, void **result);
  hubbub_error html_create_element(void *ctx, const hubbub_tag *tag, void **result);
  hubbub_error html_create_text(void *ctx, const hubbub_string *data, void **result);
  hubbub_error html_ref_node(void *ctx, void *node);
  hubbub_error html_unref_node(void *ctx, void *node);
  hubbub_error html_append_child(void *ctx, void *parent, void *child, void **result);
  hubbub_error html_insert_before(void *ctx, void *parent, void *child, void *ref_child, void **result);
  hubbub_error html_remove_child(void *ctx, void *parent, void *child, void **result);
  hubbub_error html_clone_node(void *ctx, void *node, bool deep, void **result);
  hubbub_error html_reparent_children(void *ctx, void *node, void *new_parent);
  hubbub_error html_get_parent(void *ctx, void *node, bool element_only, void **result);
  hubbub_error html_has_children(void *ctx, void *node, bool *result);
  hubbub_error html_form_associate(void *ctx, void *form, void *node);
  hubbub_error html_add_attributes(void *ctx, void *node, const hubbub_attribute *attributes, uint32_t n_attributes);
  hubbub_error html_set_quirks_mode(void *ctx, hubbub_quirks_mode mode);
  hubbub_error html_encoding_change(void *ctx, const char *encname);
  hubbub_error html_complete_script(void *ctx, void *script);

}

// hubbub parser context

struct ParserContext
{
  explicit ParserContext(bool xhtml, const char *enc = 0);

  XMLTreeNodePtr_t document;
  hubbub_quirks_mode mode;

  const char *encoding;

  hubbub_tree_handler handler;

  HUBBUBParserPtr_t parser;

  LWCStringPtr_t nsMap[HUBBUB_NS_XMLNS];

  XMLTreeNode::ConfigurationPtr_t htmlConfig;
  XMLTreeNode::ConfigurationPtr_t xhtmlConfig;

  XMLTreeNode::ConfigurationPtr_t config;

private:
  // disable copying
  ParserContext(const ParserContext &);
  ParserContext &operator=(const ParserContext &);
};

const LWCStringPtr_t &getNs(const ParserContext &context, const hubbub_ns ns)
{
  assert(HUBBUB_NS_XMLNS >= ns);

  return context.nsMap[ns];
}

ParserContext::ParserContext(const bool xhtml, const char *const enc)
  : document()
  , mode(HUBBUB_QUIRKS_MODE_NONE)
  , encoding(enc)
  , handler()
  , parser()
  , nsMap()
  , htmlConfig(new XMLTreeNode::Configuration(true))
  , xhtmlConfig(new XMLTreeNode::Configuration())
  , config(xhtml ? xhtmlConfig : htmlConfig)
{
  handler.create_comment = &html_create_comment;
  handler.create_doctype = &html_create_doctype;
  handler.create_element = &html_create_element;
  handler.create_text = &html_create_text;
  handler.ref_node = &html_ref_node;
  handler.unref_node = &html_unref_node;
  handler.append_child = &html_append_child;
  handler.insert_before = &html_insert_before;
  handler.remove_child = &html_remove_child;
  handler.clone_node = &html_clone_node;
  handler.reparent_children = &html_reparent_children;
  handler.get_parent = &html_get_parent;
  handler.has_children = &html_has_children;
  handler.form_associate = &html_form_associate;
  handler.add_attributes = &html_add_attributes;
  handler.set_quirks_mode = &html_set_quirks_mode;
  handler.encoding_change = &html_encoding_change;
  handler.complete_script = &html_complete_script;
  handler.ctx = this;

  {
    hubbub_parser *p = 0;
    // FIXME: how do I attach an external data (i.e., this) to the parser now?
    if (HUBBUB_OK != hubbub_parser_create(enc, true, &p))
      throw GenericException();
    parser = wrap(p);
  }

  hubbub_parser_optparams params;

  params.tree_handler = &handler;
  hubbub_parser_setopt(parser.get(), HUBBUB_PARSER_TREE_HANDLER, &params);

  document.reset(new XMLTreeNode(XMLTreeNode::NODE_TYPE_DOCUMENT, config));

  intrusive_ptr_add_ref(document.get());
  params.document_node = document.get();
  hubbub_parser_setopt(parser.get(), HUBBUB_PARSER_DOCUMENT_NODE, &params);

  for (int n = HUBBUB_NS_NULL; HUBBUB_NS_XMLNS != n; ++n)
  {
    if (0 == NS_NAME_MAP[n].len)
    {
      nsMap[n] = LWCStringPtr_t();
    }
    else
    {
      nsMap[n] = makeLWCString(NS_NAME_MAP[n].str, NS_NAME_MAP[n].len);
      if (!nsMap[n])
        throw bad_alloc();
    }
  }

  const LWCStringPtr_t a = makeLWCString(STR_LIT("a"));
  const LWCStringPtr_t cls = makeLWCString(STR_LIT("class"));
  const LWCStringPtr_t href = makeLWCString(STR_LIT("href"));
  const LWCStringPtr_t id = makeLWCString(STR_LIT("id"));
  const LWCStringPtr_t lang = makeLWCString(STR_LIT("lang"));

  if (!a || !cls || !href || !id || !lang)
    throw bad_alloc();

  const LWCStringPtr_t &html = getNs(*this, HUBBUB_NS_HTML);
  const LWCStringPtr_t &xml = getNs(*this, HUBBUB_NS_XML);

  htmlConfig->classes.push_back(XMLTreeNode::QName(cls));
  htmlConfig->ids.push_back(XMLTreeNode::QName(id));
  htmlConfig->langs.push_back(XMLTreeNode::QName(lang, xml));
  htmlConfig->links.push_back(XMLTreeNode::LinkId_t(XMLTreeNode::QName(a, html), XMLTreeNode::QName(href)));

  xhtmlConfig->classes.push_back(XMLTreeNode::QName(cls));
  xhtmlConfig->ids.push_back(XMLTreeNode::QName(id));
  xhtmlConfig->ids.push_back(XMLTreeNode::QName(id, xml));
  xhtmlConfig->langs.push_back(XMLTreeNode::QName(lang, xml));
  xhtmlConfig->links.push_back(XMLTreeNode::LinkId_t(XMLTreeNode::QName(a, html), XMLTreeNode::QName(href)));
}

#if 0 // for future use
bool operator==(const string &lhs, const hubbub_string &rhs)
{
  return (lhs.size() == rhs.len) && (equal(lhs.begin(), lhs.end(), char_cast(rhs.ptr)));
}

bool operator==(const hubbub_string &lhs, const string &rhs)
{
  return rhs == lhs;
}
#endif

XMLTreeNode *release(const XMLTreeNodePtr_t &p)
{
  intrusive_ptr_add_ref(p.get());
  return p.get();
}

extern "C"
{

  void *allocator(void *const ptr, const size_t size, void *)
  {
    return std::realloc(ptr, size);
  }

// hubbub treebuilder callbacks

  hubbub_error html_create_comment(void *, const hubbub_string *, void **const result)
  {
    assert(result);

    // NOTE: comments are not put into the tree, so we do not bother to use the data...
    const XMLTreeNodePtr_t node(new XMLTreeNode(XMLTreeNode::NODE_TYPE_COMMENT));

    *result = release(node);

    assert(2 == node->refcount);

    return HUBBUB_OK;
  }

  hubbub_error html_create_doctype(void *, const hubbub_doctype *, void **const result)
  {
    assert(result);

    // dummy node that we ignore in the tree builder
    const XMLTreeNodePtr_t node(new XMLTreeNode(XMLTreeNode::NODE_TYPE_DOCTYPE));

    *result = release(node);

    assert(2 == node->refcount);

    return HUBBUB_OK;
  }

  hubbub_error html_create_element(void *const ctx, const hubbub_tag *const tag, void **const result)
  {
    ParserContext *const context = reinterpret_cast<ParserContext *>(ctx);

    assert(context);
    assert(tag);
    assert(result);

    if (EBOOKHTMLToken::html == getHTMLTokenId(char_cast(tag->name.ptr), tag->name.len))
    {
      // TODO: check that it matches the preset
      context->config = tag->ns ? context->xhtmlConfig : context->htmlConfig;
      context->document->configuration = context->config;
    }
    assert(context->config);

    const XMLTreeNodePtr_t node(new XMLTreeNode(XMLTreeNode::NODE_TYPE_ELEMENT, context->config));
    node->self = makeElement(*tag);
    node->addAttributes(makeAttributes(tag->attributes, tag->n_attributes));

    *result = release(node);

    assert(2 == node->refcount);

    return HUBBUB_OK;
  }

  hubbub_error html_create_text(void *, const hubbub_string *const data, void **const result)
  {
    assert(data);
    assert(result);

    const XMLTreeNodePtr_t node(new XMLTreeNode(XMLTreeNode::NODE_TYPE_TEXT));
    node->text.assign(char_cast(data->ptr), data->len);

    *result = release(node);

    assert(2 == node->refcount);

    return HUBBUB_OK;
  }

  hubbub_error html_ref_node(void *, void *const node)
  {
    XMLTreeNode *const nd = reinterpret_cast<XMLTreeNode *>(node);
    assert(nd);

    intrusive_ptr_add_ref(nd);

    return HUBBUB_OK;
  }

  hubbub_error html_unref_node(void *, void *const node)
  {
    XMLTreeNode *const nd = reinterpret_cast<XMLTreeNode *>(node);
    assert(nd);

    intrusive_ptr_release(nd);

    return HUBBUB_OK;
  }

  hubbub_error html_append_child(void *, void *const parent, void *const child, void **const result)
  {
    const XMLTreeNodePtr_t p(reinterpret_cast<XMLTreeNode *>(parent));
    const XMLTreeNodePtr_t c(reinterpret_cast<XMLTreeNode *>(child));

    assert(bool(p));
    assert(bool(c));
    assert(result);

    const XMLTreeNodePtr_t inserted = p->appendChild(c);

    *result = release(inserted);

    return HUBBUB_OK;
  }

  hubbub_error html_insert_before(void *, void *const parent, void *const child, void *const ref_child, void **const result)
  {
    const XMLTreeNodePtr_t p(reinterpret_cast<XMLTreeNode *>(parent));
    const XMLTreeNodePtr_t c(reinterpret_cast<XMLTreeNode *>(child));
    const XMLTreeNodePtr_t r(reinterpret_cast<XMLTreeNode *>(ref_child));

    assert(bool(p));
    assert(bool(c));
    assert(bool(r));
    assert(result);

    const XMLTreeNodePtr_t inserted = p->insertChildBefore(c, r);

    *result = release(inserted);

    return HUBBUB_OK;
  }

  hubbub_error html_remove_child(void *, void *const parent, void *const child, void **const result)
  {
    const XMLTreeNodePtr_t p(reinterpret_cast<XMLTreeNode *>(parent));
    const XMLTreeNodePtr_t c(reinterpret_cast<XMLTreeNode *>(child));

    assert(bool(p));
    assert(bool(c));
    assert(result);

    p->removeChild(c);

    *result = release(c);

    return HUBBUB_OK;
  }

  hubbub_error html_clone_node(void *, void *const node, bool deep, void **const result)
  {
    const XMLTreeNodePtr_t nd(reinterpret_cast<XMLTreeNode *>(node));

    assert(bool(nd));
    assert(result);

    const XMLTreeNodePtr_t c = nd->clone(deep);

    *result = release(c);

    return HUBBUB_OK;
  }

  hubbub_error html_reparent_children(void *, void *const node, void *const new_parent)
  {
    const XMLTreeNodePtr_t source(reinterpret_cast<XMLTreeNode *>(node));
    const XMLTreeNodePtr_t target(reinterpret_cast<XMLTreeNode *>(new_parent));

    assert(bool(source));
    assert(bool(target));

    source->transferChildren(target);

    return HUBBUB_OK;
  }

  hubbub_error html_get_parent(void *, void *const node, bool, void **const result)
  {
    const XMLTreeNodePtr_t nd(reinterpret_cast<XMLTreeNode *>(node));

    assert(bool(nd));
    assert(result);

    const XMLTreeNodePtr_t parent = nd->parent;

    *result = 0;
    if (bool(parent))
      *result = release(parent);

    return HUBBUB_OK;
  }

  hubbub_error html_has_children(void *, void *const node, bool *const result)
  {
    const XMLTreeNodePtr_t nd(reinterpret_cast<XMLTreeNode *>(node));

    assert(bool(nd));
    assert(result);

    *result = nd->hasChildren();

    return HUBBUB_OK;
  }

  hubbub_error html_form_associate(void *, void *, void *)
  {
    return HUBBUB_OK;
  }

  hubbub_error html_add_attributes(void *, void *const node, const hubbub_attribute *const attributes, const uint32_t n_attributes)
  {
    // NOTE: I assume this is an error in hubbub. But let's continue as
    // if nothing happened.
    if (0 == n_attributes)
      return HUBBUB_OK;

    const XMLTreeNodePtr_t nd(reinterpret_cast<XMLTreeNode *>(node));

    assert(bool(nd));
    assert(attributes);

    nd->addAttributes(makeAttributes(attributes, n_attributes));

    return HUBBUB_OK;
  }

  hubbub_error html_set_quirks_mode(void *const ctx, const hubbub_quirks_mode mode)
  {
    ParserContext *const context = reinterpret_cast<ParserContext *>(ctx);

    assert(context);

    context->mode = mode;

    return HUBBUB_OK;
  }

  hubbub_error html_encoding_change(void *const ctx, const char *const encname)
  {
    ParserContext *const context = reinterpret_cast<ParserContext *>(ctx);

    assert(context);
    assert(encname);

    if (0 != context->encoding)
      return HUBBUB_OK;

    hubbub_charset_source source;
    const char *const name = hubbub_parser_read_charset(context->parser.get(), &source);

    if (HUBBUB_CHARSET_CONFIDENT == source)
    {
      context->encoding = encname;
      return HUBBUB_OK;
    }

    context->encoding = encname;

    return (context->encoding == name) ? HUBBUB_OK : HUBBUB_ENCODINGCHANGE;
  }

  hubbub_error html_complete_script(void *, void *)
  {
    return HUBBUB_OK;
  }

}

}

namespace
{

enum ParseContext
{
  PARSE_CONTEXT_UNKNOWN,
  PARSE_CONTEXT_BLOCK,
  PARSE_CONTEXT_BODY,
  PARSE_CONTEXT_CHARACTER,
  PARSE_CONTEXT_HEAD,
  PARSE_CONTEXT_HTML,
  PARSE_CONTEXT_LIST,
  PARSE_CONTEXT_LIST_ELEMENT,
  PARSE_CONTEXT_OBJECT,
  PARSE_CONTEXT_SCRIPT,
  PARSE_CONTEXT_STYLESHEET,
  PARSE_CONTEXT_TABLE,
  PARSE_CONTEXT_TABLE_CELL,
  PARSE_CONTEXT_TABLE_ROW,
  PARSE_CONTEXT_TITLE
};

template<class T>
class AttributeHandler
{
  typedef AttributeHandler<T> Self_t;

  // assignment is not possible
  Self_t operator=(const Self_t &);

public:
  typedef std::function<bool(int, const char *, T &)> Func_t;

public:
  AttributeHandler(T &attrs, const Func_t &handler);
  AttributeHandler(T &attrs, const Func_t &handler, const Self_t &chained);
  AttributeHandler(const Self_t &other);

  bool operator()(int id, const char *value) const;

private:
  T &m_attrs;
  const Func_t m_handler;
  const Self_t *const m_chained;
};

template<class T>
AttributeHandler<T>::AttributeHandler(T &attrs, const Func_t &handler)
  : m_attrs(attrs)
  , m_handler(handler)
  , m_chained(0)
{
}

template<class T>
AttributeHandler<T>::AttributeHandler(T &attrs, const Func_t &handler, const Self_t &chained)
  : m_attrs(attrs)
  , m_handler(handler)
  , m_chained(&chained)
{
}

template<class T>
AttributeHandler<T>::AttributeHandler(const Self_t &other)
  : m_attrs(other.m_attrs)
  , m_handler(other.m_handler)
  , m_chained(other.m_chained)
{
}

template<class T>
bool AttributeHandler<T>::operator()(const int id, const char *value) const
{
  if (m_handler(id, value, m_attrs))
    return true;
  if (m_chained)
    return (*m_chained)(id, value);
  return false;
}

template<class T>
AttributeHandler<T>
makeAttributeHandler(T &attrs, const typename AttributeHandler<T>::Func_t &handler)
{
  return AttributeHandler<T>(attrs, handler);
}

template<class T>
AttributeHandler<T>
makeAttributeHandler(T &attrs, const typename AttributeHandler<T>::Func_t &handler, const AttributeHandler<T> &chained)
{
  return AttributeHandler<T>(attrs, handler, chained);
}

template<class T>
void parseAttributes(const XMLTreeWalker &node, const AttributeHandler<T> &handler)
{
  for (XMLTreeWalker::AttributeIterator it = node.beginAttributes(); it != node.endAttributes(); ++it)
  {
    const int id = it->getId();
    if (EBOOKHTMLToken::INVALID_TOKEN != id)
      handler(id, it->getValue().c_str());
  }
}

}

namespace
{

#if 0
unsigned char hex2dec(const char *const hex)
{
  unsigned char dec = 0;

  for (int i = 0; i != 2; ++i)
  {
    dec <<= 4;

    const char c = hex[i];

    if (('0' <= c) && ('9' >= c))
      dec |= c - '0';
    else if (('a' <= c) && ('f' >= c))
      dec |= c - 'a';
    else if (('A' <= c) && ('F' >= c))
      dec |= c - 'A';
  }

  return dec;
}

EBOOKHTMLColor parseColor(const char *c)
{
  assert(c);

  EBOOKHTMLColor color;

  if ('#' == c[0])
  {
    ++c;
    if (6 == std::strlen(c) && (string::npos == string(c).find_first_not_of("0-9a-fA-F")))
    {
      color.red = hex2dec(c);
      color.green = hex2dec(c + 2);
      color.blue = hex2dec(c + 4);
    }
  }
  else if (c[0])
  {
    string col(c);
    boost::to_lower(col);
    switch (getHTMLTokenId(col.c_str()))
    {
    case EBOOKHTMLToken::aqua :
      color = EBOOKHTMLColor(0, 0xff, 0xff);
      break;
    case EBOOKHTMLToken::black :
      color = EBOOKHTMLColor(0, 0, 0);
      break;
    case EBOOKHTMLToken::blue :
      color = EBOOKHTMLColor(0, 0, 0xff);
      break;
    case EBOOKHTMLToken::fuchsia :
      color = EBOOKHTMLColor(0xff, 0, 0xff);
      break;
    case EBOOKHTMLToken::gray :
      color = EBOOKHTMLColor(0x80, 0x80, 0x80);
      break;
    case EBOOKHTMLToken::green :
      color = EBOOKHTMLColor(0, 0x80, 0);
      break;
    case EBOOKHTMLToken::lime :
      color = EBOOKHTMLColor(0, 0xff, 0);
      break;
    case EBOOKHTMLToken::maroon :
      color = EBOOKHTMLColor(0x80, 0, 0);
      break;
    case EBOOKHTMLToken::navy :
      color = EBOOKHTMLColor(0, 0, 0x80);
      break;
    case EBOOKHTMLToken::olive :
      color = EBOOKHTMLColor(0x80, 0x80, 0);
      break;
    case EBOOKHTMLToken::purple :
      color = EBOOKHTMLColor(0x80, 0, 0x80);
      break;
    case EBOOKHTMLToken::red :
      color = EBOOKHTMLColor(0xff, 0, 0);
      break;
    case EBOOKHTMLToken::silver :
      color = EBOOKHTMLColor(0xc0, 0xc0, 0xc0);
      break;
    case EBOOKHTMLToken::teal :
      color = EBOOKHTMLColor(0, 0x80, 0x80);
      break;
    case EBOOKHTMLToken::white :
      color = EBOOKHTMLColor(0xff, 0xff, 0xff);
      break;
    case EBOOKHTMLToken::yellow :
      color = EBOOKHTMLColor(0xff, 0xff, 0);
      break;
    default :
      EBOOK_DEBUG_MSG(("unknown named color: %s\n", col.c_str()));
      break;
    }
  }

  return color;
}

EBOOKHTMLValue parseValue(const char *const v)
{
  EBOOKHTMLValue value;

  const char *const percent = std::strchr(v, '%');
  if (percent)
  {
    value.value = lexical_cast<unsigned>(v, percent - v);
    value.percent = true;
  }
  else
  {
    value.value = lexical_cast<unsigned>(v);
  }

  return value;
}
#endif

}

namespace
{

#if 0 // for future use
RVNGPropertyList makeMetadataProps(const EBOOKHTMLMetadata &metadata)
{
  RVNGPropertyList props;

  if (bool(metadata.title))
    props.insert("dc:title", get(metadata.title).c_str());

  return props;
}
#endif

RVNGPropertyList makeParagraphProps(const CSSSelectResultsPtr_t &style)
{
  // TODO: implement me
  (void) style;

  RVNGPropertyList props;
  return props;
}

RVNGPropertyList makeSpanProps(const CSSSelectResultsPtr_t &style)
{
  // TODO: implement me
  (void) style;

  RVNGPropertyList props;
  return props;
}

RVNGPropertyList makeTableCellProps(const CSSSelectResultsPtr_t &style)
{
  // TODO: implement me
  (void) style;

  RVNGPropertyList props;
  return props;
}

#if 0 // for future use
RVNGPropertyList makeTableRowProps(const CSSSelectResultsPtr_t &style)
{
  // TODO: implement me
  (void) style;

  RVNGPropertyList props;
  return props;
}
#endif

#if 0 // for future use
RVNGPropertyList makeTableProps(const CSSSelectResultsPtr_t &style)
{
  // TODO: implement me
  (void) style;

  RVNGPropertyList props;
  return props;
}
#endif

RVNGPropertyList makeListElementProps(const CSSSelectResultsPtr_t &style)
{
  // TODO: implement me
  (void) style;

  RVNGPropertyList props;
  return props;
}

RVNGPropertyList makeListProps(const CSSSelectResultsPtr_t &style)
{
  // TODO: implement me
  (void) style;

  RVNGPropertyList props;
  return props;
}

}

namespace
{

struct BlockState
{
  bool isPre;

  BlockState();
};

BlockState::BlockState()
  : isPre(false)
{
}

}

struct EBOOKHTMLParser::State
{
  stack<ParseContext> contextStack;
  BlockState blockState;
  EBOOKHTMLMetadata metadata;
  XMLStylesheets_t sheets;

  State();
};

EBOOKHTMLParser::State::State()
  : contextStack()
  , blockState()
  , metadata()
  , sheets()
{
}

EBOOKHTMLParser::EBOOKHTMLParser(librevenge::RVNGInputStream *const input, const Dialect /* dialect */, librevenge::RVNGTextInterface *const document)
  : m_collector(new XMLCollector(document))
  , m_state(new State())
  , m_input(input, EBOOKDummyDeleter())
  , m_workingInput(m_input)
{
}

EBOOKHTMLParser::~EBOOKHTMLParser()
{
}

void EBOOKHTMLParser::parse()
{
  const XMLTreeNodePtr_t tree = readTree();

  if (!tree)
    throw GenericException();

  const XMLTreeWalker root(tree, getHTMLTokenId);
  using std::placeholders::_1;
  std::for_each(root.begin(), root.end(), std::bind(&EBOOKHTMLParser::processNode, this, _1));

  m_collector->finish();
}

const XMLTreeNodePtr_t EBOOKHTMLParser::readTree()
{
  XMLTreeNodePtr_t tree;

  string encoding;
  bool encodingChange;
  tree = readTreeImpl(encoding, encodingChange);

  if (!tree)
  {
    EBOOKCharsetConverter conv(encoding.c_str());
    m_workingInput.reset(new EBOOKUTF8Stream(m_input.get(), &conv));
    tree = readTreeImpl(encoding, encodingChange);
  }

  return tree;
}

const XMLTreeNodePtr_t EBOOKHTMLParser::readTreeImpl(std::string &encoding, bool &encodingChange)
{
  const unsigned len = getRemainingLength(m_workingInput);
  const unsigned char *const data = readNBytes(m_workingInput, len);

  encodingChange = false;

  if (!data || (0 == len))
    return XMLTreeNodePtr_t();

  ParserContext context(encoding.empty() ? 0 : encoding.c_str());

  hubbub_error error = hubbub_parser_parse_chunk(context.parser.get(), data, len);
  switch (error)
  {
  case HUBBUB_OK :
    break;
  case HUBBUB_ENCODINGCHANGE :
    encoding.assign(context.encoding);
    encodingChange = true;
    return XMLTreeNodePtr_t();
  default :
    return XMLTreeNodePtr_t();
  }

  error = hubbub_parser_completed(context.parser.get());
  if (HUBBUB_OK != error)
    return XMLTreeNodePtr_t();

  return context.document;
}

void EBOOKHTMLParser::processNode(const XMLTreeWalker &node)
{
  if (node.isElement())
  {
    startElement(node);
    using std::placeholders::_1;
    std::for_each(node.begin(), node.end(), std::bind(&EBOOKHTMLParser::processNode, this, _1));
    endElement(node);
  }
  else
  {
    text(node);
  }
}

void EBOOKHTMLParser::startElement(const XMLTreeWalker &node)
{
  assert(node.isElement());

  bool isSpan = false;

  switch (node.getId())
  {
  // top level elements
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::body :
    m_state->contextStack.push(PARSE_CONTEXT_BODY);
    break;
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::head :
    m_state->contextStack.push(PARSE_CONTEXT_HEAD);
    break;
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::html :
    m_state->contextStack.push(PARSE_CONTEXT_HTML);
    break;

  // header elements
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::base :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::link :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::meta :
    // TODO: implement me
    break;
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::script :
    m_state->contextStack.push(PARSE_CONTEXT_SCRIPT);
    break;
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::style :
    m_state->contextStack.push(PARSE_CONTEXT_STYLESHEET);
    break;
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::title :
    m_state->contextStack.push(PARSE_CONTEXT_TITLE);
    break;

  // block elements
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::address :
    m_state->contextStack.push(PARSE_CONTEXT_BLOCK);
    m_collector->openParagraph(makeParagraphProps(node.getStyle(m_state->sheets)));
    break;
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::blockquote :
  {
    sendParagraph(node);
    break;
  }
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::center :
  {
    sendParagraph(node);
    break;
  }
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::div :
    // nothing needed
    break;
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::h1 :
    sendHeading(node, 1);
    break;
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::h2 :
    sendHeading(node, 2);
    break;
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::h3 :
    sendHeading(node, 3);
    break;
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::h4 :
    sendHeading(node, 4);
    break;
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::h5 :
    sendHeading(node, 5);
    break;
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::h6 :
    sendHeading(node, 6);
    break;
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::hr :
    // ignore
    break;
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::p :
  {
    sendParagraph(node);
    break;
  }
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::pre :
  {
    m_state->blockState.isPre = true;
    sendParagraph(node);
    break;
  }

  // lists
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::dd :
    // TODO: implement me
    break;
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::dir :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::menu :
  {
    m_state->contextStack.push(PARSE_CONTEXT_LIST);
    m_collector->openUnorderedList(makeListProps(node.getStyle(m_state->sheets)));
    break;
  }
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::dl :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::dt :
    // TODO: implement me
    break;
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::li :
  {
    m_state->contextStack.push(PARSE_CONTEXT_LIST_ELEMENT);
    m_collector->openListElement(makeListElementProps(node.getStyle(m_state->sheets)));
    break;
  }
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::ol :
  {
    m_state->contextStack.push(PARSE_CONTEXT_LIST);
    m_collector->openOrderedList(makeListProps(node.getStyle(m_state->sheets)));
    break;
  }
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::ul :
  {
    m_state->contextStack.push(PARSE_CONTEXT_LIST);
    m_collector->openUnorderedList(makeListProps(node.getStyle(m_state->sheets)));
    break;
  }

  // tables
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::caption :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::col :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::colgroup :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::table :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::tbody :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::td :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::tfoot :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::th :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::thead :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::tr :
    // TODO: handle
    break;

  // character elements
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::a :
    // TODO: handle
    break;
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::abbr :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::acronym :
    isSpan = true;
    break;
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::b :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::strong :
    isSpan = true;
    break;
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::bdo :
    isSpan = true;
    break;
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::big :
    isSpan = true;
    break;
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::br :
    m_collector->collectLineBreak();
    break;
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::cite :
    isSpan = true;
    break;
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::code :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::dfn :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::kbd :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::samp :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::tt :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::var :
    isSpan = true;
    break;
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::del :
    isSpan = true;
    break;
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::em :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::i :
    isSpan = true;
    break;
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::font :
    break;
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::ins :
    isSpan = true;
    break;
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::q :
    isSpan = true;
    break;
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::s :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::strike :
    isSpan = true;
    break;
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::small :
    isSpan = true;
    break;
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::span :
    isSpan = true;
    break;
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::sub :
    isSpan = true;
    break;
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::sup :
    isSpan = true;
    break;
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::u :
    isSpan = true;
    break;

  // objects
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::applet :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::iframe :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::img :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::map :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::object :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::param :
    // TODO: handle
    break;

  // forms - ignored
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::button :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::fieldset :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::form :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::input :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::label :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::legend :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::optgroup :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::option :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::select :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::textarea :
    break;

  // various other elements
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::area :
    // TODO: implement me
    break;
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::basefont :
    // TODO: implement me
    break;
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::isindex :
    // TODO: implement me
    break;
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::noframes :
    // TODO: implement me
    break;
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::noscript :
    // TODO: implement me
    break;

  default :
    EBOOK_DEBUG_MSG(("unknown element %s\n", node.getName().c_str()));
    break;
  }

  if (isSpan)
    sendSpan(node);
}

void EBOOKHTMLParser::endElement(const XMLTreeWalker &node)
{
  assert(node.isElement());
  assert(!m_state->contextStack.empty());

  switch (node.getId())
  {
  // top level elements
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::body :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::head :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::html :
    m_state->contextStack.pop();
    break;

  // header elements
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::base :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::link :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::meta :
    // these are empty
    break;
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::script :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::style :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::title :
    m_state->contextStack.pop();
    break;

  // block elements
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::pre :
    m_state->blockState.isPre = false;
  // fallthrough intended
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::address :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::blockquote :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::center :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::h1 :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::h2 :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::h3 :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::h4 :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::h5 :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::h6 :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::p :
    m_collector->closeParagraph();
    m_state->contextStack.pop();
    break;
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::div :
    // nothing needed
    break;
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::hr :
    // this is empty
    break;

  // lists
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::dd :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::dl :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::dt :
    // TODO: implement me
    break;
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::dir :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::menu :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::ul :
    m_collector->closeUnorderedList();
    m_state->contextStack.pop();
    break;
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::li :
    m_collector->closeListElement();
    m_state->contextStack.pop();
    break;
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::ol :
    m_collector->closeOrderedList();
    m_state->contextStack.pop();
    break;

  // tables
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::caption :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::col :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::colgroup :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::table :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::tbody :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::td :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::tfoot :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::th :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::thead :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::tr :
    // TODO: implement me
    break;

  // character elements
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::a :
    // TODO: implement me
    break;
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::abbr :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::acronym :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::b :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::bdo :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::big :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::cite :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::code :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::del :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::dfn :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::em :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::font :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::i :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::ins :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::kbd :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::q :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::samp :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::small :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::s :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::span :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::strike :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::strong :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::sub :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::sup :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::tt :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::u :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::var :
    m_collector->closeSpan();
    m_state->contextStack.pop();
    break;
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::br :
    // this is empty
    break;

  // objects
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::applet :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::iframe :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::img :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::map :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::object :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::param :
    // TODO: implement me
    break;

  // forms - ignored
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::button :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::fieldset :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::form :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::input :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::label :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::legend :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::optgroup :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::option :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::select :
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::textarea :
    break;

  // various other elements
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::area :
    // TODO: implement me
    break;
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::basefont :
    // TODO: implement me
    break;
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::isindex :
    // TODO: implement me
    break;
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::noframes :
    // TODO: implement me
    break;
  case EBOOKHTMLToken::NS_html | EBOOKHTMLToken::noscript :
    // TODO: implement me
    break;

  default :
    EBOOK_DEBUG_MSG(("unknown element %s\n", node.getName().c_str()));
    break;
  }
}

void EBOOKHTMLParser::text(const XMLTreeWalker &node)
{
  assert(node.isText());
  assert(!m_state->contextStack.empty());

  switch (m_state->contextStack.top())
  {
  case PARSE_CONTEXT_BLOCK :
  case PARSE_CONTEXT_BODY :
  case PARSE_CONTEXT_CHARACTER :
  case PARSE_CONTEXT_LIST_ELEMENT :
  case PARSE_CONTEXT_TABLE_CELL :
    if (m_state->blockState.isPre)
    {
      const string t = node.getText();
      sendPreformattedText(t.c_str(), t.size());
    }
    else
      m_collector->collectText(node.getText());
    break;
  case PARSE_CONTEXT_STYLESHEET :
    // TODO: handle
    break;
  case PARSE_CONTEXT_TITLE :
    m_state->metadata.title = node.getText();
    break;
  default :
    break;
  }
}

void EBOOKHTMLParser::startExternalElement(const XMLTreeWalker &node)
{
  // TODO: what to do here?
  (void) node;
}

void EBOOKHTMLParser::endExternalElement(const XMLTreeWalker &node)
{
  // TODO: what to do here?
  (void) node;
}

void EBOOKHTMLParser::sendParagraph(const XMLTreeWalker &node)
{
  m_state->contextStack.push(PARSE_CONTEXT_BLOCK);
  m_collector->openParagraph(makeParagraphProps(node.getStyle(m_state->sheets)));
}

void EBOOKHTMLParser::sendSpan(const XMLTreeWalker &node)
{
  m_state->contextStack.push(PARSE_CONTEXT_CHARACTER);
  m_collector->openSpan(makeSpanProps(node.getStyle(m_state->sheets)));
}

void EBOOKHTMLParser::sendTableCell(const XMLTreeWalker &node)
{
  m_state->contextStack.push(PARSE_CONTEXT_TABLE_CELL);
  m_collector->openTableCell(makeTableCellProps(node.getStyle(m_state->sheets)));
}

void EBOOKHTMLParser::sendPreformattedText(const char *const ch, const int len)
{
  assert(0 <= len);

  string current;

  bool cr = false;
  for (int i = 0; i != len; ++i)
  {
    switch (ch[i])
    {
    case '\n' :
      if (cr) // in case line break is CRLF
        cr = false;
      m_collector->collectText(current);
      m_collector->collectLineBreak();
      break;
    case '\r' :
      if (cr) // in case line break is CR
      {
        m_collector->collectText(current);
        m_collector->collectLineBreak();
      }
      cr = true;
      break;
    default :
      if (cr) // unprocessed CR, in case line break is CR
      {
        m_collector->collectText(current);
        m_collector->collectLineBreak();
        cr = false;
      }
      else
      {
        current.push_back(ch[i]);
      }
      break;
    }
  }

  if (!current.empty())
    m_collector->collectText(current);
}

void EBOOKHTMLParser::sendHeading(const XMLTreeWalker &node, int level)
{
  (void) node;
  (void) level;
  assert((1 <= level) && (6 >= level));

  sendParagraph(node);
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
