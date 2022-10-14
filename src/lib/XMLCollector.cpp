/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <boost/optional.hpp>

#include <stack>

#include "libebook_utils.h"
#include "XMLCollector.h"
#include "EBOOKOutputElements.h"

using boost::optional;
using std::shared_ptr;

using librevenge::RVNGPropertyList;
using librevenge::RVNGString;
using librevenge::RVNGTextInterface;

using std::stack;
using std::string;

namespace libebook
{

namespace
{

class OutputHandler;
typedef shared_ptr<OutputHandler> OutputHandlerPtr_t;

/** Handle output for one (and only one) context level.
  *
  * This class makes sure that:
  * * output calls are always balanced
  * * there are no empty blocks caused by "linearization" of multiple
  *   nested elements of the same type.
  */
class OutputHandler
{
  // disable copying
  OutputHandler(const OutputHandler &);
  OutputHandler &operator=(const OutputHandler &);

  typedef void (EBOOKOutputElements::* OpenFun_t)(const RVNGPropertyList &);
  typedef void (EBOOKOutputElements::* CloseFun_t)();

public:
  explicit OutputHandler(EBOOKOutputElements &output, const OutputHandlerPtr_t &parent = OutputHandlerPtr_t());
  ~OutputHandler();

  /** Close and set the state to before open.
    *
    * This is needed when an opened context needs to be popped from the
    * main stack temporarily (because we are entering another context of
    * the same type, e.g., a nested span). The current context must be
    * closed (if it was opened) and then possibly opened again when the
    * context is restored on the main stack.
    *
    * Example:
    * The following sequence of calls to XMLCollector leads to the
    * (intermingled) calls to EBOOKOutputElements
    * <pre>
    * openSpan() // span context 1
    * insertSpace()
    *   --> addOpenSpan()
    *   --> addInsertSpace()
    * openSpan()
    *   --> closeSpan() // reset() call here, context 1 is saved
    * insertTab()
    *   --> addOpenSpan() // span context 2
    *   --> addInsertTab()
    * closeSpan()
    *   --> closeSpan() // context 1 is restored
    * insertLineBreak()
    *   --> addOpenSpan() // context 1 again
    *   --> addInsertLineBreak()
    * closeSpan()
    *   --> closeSpan()
    * </pre>
    */
  void reset();

  void openDocument(const RVNGPropertyList &propList);
  void closeDocument();

  void openPageSpan(const RVNGPropertyList &propList);
  void closePageSpan();
  void openHeader(const RVNGPropertyList &propList);
  void closeHeader();
  void openFooter(const RVNGPropertyList &propList);
  void closeFooter();
  void openFootnote(const RVNGPropertyList &propList);
  void closeFootnote();

  void openLink(const RVNGPropertyList &propList);
  void closeLink();
  void openParagraph(const RVNGPropertyList &propList);
  void closeParagraph();
  void openSection(const RVNGPropertyList &propList);
  void closeSection();
  void openSpan(const RVNGPropertyList &propList);
  void closeSpan();

  void openOrderedList(const RVNGPropertyList &propList);
  void closeOrderedList();
  void openUnorderedList(const RVNGPropertyList &propList);
  void closeUnorderedList();
  void openListElement(const RVNGPropertyList &propList);
  void closeListElement();

  void openTable(const RVNGPropertyList &propList);
  void closeTable();
  void openTableCell(const RVNGPropertyList &propList);
  void closeTableCell();
  void openTableRow(const RVNGPropertyList &propList);
  void closeTableRow();

  void insertBinaryObject(const librevenge::RVNGPropertyList &propList);
  void insertCoveredTableCell(const librevenge::RVNGPropertyList &propList);
  void insertLineBreak();
  void insertSpace();
  void insertTab();
  void insertText(const librevenge::RVNGString &text);

private:
  void closeAnything();

  void open();
  void close();

private:
  EBOOKOutputElements &m_output;
  const OutputHandlerPtr_t m_parent;
  OpenFun_t m_openFun;
  CloseFun_t m_closeFun;
  RVNGPropertyList m_propList;
  bool m_dummy;
  bool m_open;
  bool m_closed;
  bool m_continued;
};

OutputHandler::OutputHandler(EBOOKOutputElements &output, const OutputHandlerPtr_t &parent)
  : m_output(output)
  , m_parent(parent)
  , m_openFun(0)
  , m_closeFun(0)
  , m_propList()
  , m_dummy(false)
  , m_open(false)
  , m_closed(false)
  , m_continued(false)
{
}

OutputHandler::~OutputHandler()
{
  if (m_open && !m_closed)
    close();
}

void OutputHandler::reset()
{
  assert(m_openFun);
  assert(m_closeFun);

  if (m_open)
    close();
  m_open = false;
  m_closed = false;
  m_continued = true;
}

void OutputHandler::openDocument(const RVNGPropertyList &)
{
  assert(!m_openFun);
  assert(!m_closeFun);
  assert(!m_dummy);

  m_dummy = true;
}

void OutputHandler::closeDocument()
{
}

void OutputHandler::openPageSpan(const RVNGPropertyList &propList)
{
  assert(!m_openFun);
  assert(!m_closeFun);

  m_openFun = &EBOOKOutputElements::addOpenPageSpan;
  m_closeFun = &EBOOKOutputElements::addClosePageSpan;
  m_propList = propList;
}

void OutputHandler::closePageSpan()
{
  closeAnything();
}

void OutputHandler::openHeader(const RVNGPropertyList &propList)
{
  assert(!m_openFun);
  assert(!m_closeFun);

  // TODO: implement me
  // m_openFun = &EBOOKOutputElements::addOpenHeader;
  // m_closeFun = &EBOOKOutputElements::addCloseHeader;
  m_propList = propList;
}

void OutputHandler::closeHeader()
{
  // TODO: implement me
  // closeAnything();
}

void OutputHandler::openFooter(const RVNGPropertyList &propList)
{
  assert(!m_openFun);
  assert(!m_closeFun);

  // TODO: implement me
  // m_openFun = &EBOOKOutputElements::addOpenFooter;
  // m_closeFun = &EBOOKOutputElements::addCloseFooter;
  m_propList = propList;
}

void OutputHandler::closeFooter()
{
  // TODO: implement me
  // closeAnything();
}

void OutputHandler::openFootnote(const RVNGPropertyList &propList)
{
  assert(!m_openFun);
  assert(!m_closeFun);

  m_openFun = &EBOOKOutputElements::addOpenFootnote;
  m_closeFun = &EBOOKOutputElements::addCloseFootnote;
  m_propList = propList;
}

void OutputHandler::closeFootnote()
{
  closeAnything();
}

void OutputHandler::openLink(const RVNGPropertyList &propList)
{
  assert(!m_openFun);
  assert(!m_closeFun);

  m_openFun = &EBOOKOutputElements::addOpenLink;
  m_closeFun = &EBOOKOutputElements::addCloseLink;
  m_propList = propList;
}

void OutputHandler::closeLink()
{
  closeAnything();
}

void OutputHandler::openParagraph(const RVNGPropertyList &propList)
{
  assert(!m_openFun);
  assert(!m_closeFun);

  m_openFun = &EBOOKOutputElements::addOpenParagraph;
  m_closeFun = &EBOOKOutputElements::addCloseParagraph;
  m_propList = propList;
}

void OutputHandler::closeParagraph()
{
  closeAnything();
}

void OutputHandler::openSection(const RVNGPropertyList &propList)
{
  assert(!m_openFun);
  assert(!m_closeFun);

  m_openFun = &EBOOKOutputElements::addOpenSection;
  m_closeFun = &EBOOKOutputElements::addCloseSection;
  m_propList = propList;
}

void OutputHandler::closeSection()
{
  closeAnything();
}

void OutputHandler::openSpan(const RVNGPropertyList &propList)
{
  assert(!m_openFun);
  assert(!m_closeFun);

  m_openFun = &EBOOKOutputElements::addOpenSpan;
  m_closeFun = &EBOOKOutputElements::addCloseSpan;
  m_propList = propList;
}

void OutputHandler::closeSpan()
{
  closeAnything();
}

void OutputHandler::openOrderedList(const RVNGPropertyList &propList)
{
  assert(!m_openFun);
  assert(!m_closeFun);

  m_openFun = &EBOOKOutputElements::addOpenOrderedListLevel;
  m_closeFun = &EBOOKOutputElements::addCloseOrderedListLevel;
  m_propList = propList;
}

void OutputHandler::closeOrderedList()
{
  closeAnything();
}

void OutputHandler::openUnorderedList(const RVNGPropertyList &propList)
{
  assert(!m_openFun);
  assert(!m_closeFun);

  m_openFun = &EBOOKOutputElements::addOpenUnorderedListLevel;
  m_closeFun = &EBOOKOutputElements::addCloseUnorderedListLevel;
  m_propList = propList;
}

void OutputHandler::closeUnorderedList()
{
  closeAnything();
}

void OutputHandler::openListElement(const RVNGPropertyList &propList)
{
  assert(!m_openFun);
  assert(!m_closeFun);

  m_openFun = &EBOOKOutputElements::addOpenListElement;
  m_closeFun = &EBOOKOutputElements::addCloseListElement;
  m_propList = propList;
}

void OutputHandler::closeListElement()
{
  closeAnything();
}

void OutputHandler::openTable(const RVNGPropertyList &propList)
{
  assert(!m_openFun);
  assert(!m_closeFun);

  m_openFun = &EBOOKOutputElements::addOpenTable;
  m_closeFun = &EBOOKOutputElements::addCloseTable;
  m_propList = propList;
}

void OutputHandler::closeTable()
{
  closeAnything();
}

void OutputHandler::openTableCell(const RVNGPropertyList &propList)
{
  assert(!m_openFun);
  assert(!m_closeFun);

  m_openFun = &EBOOKOutputElements::addOpenTableCell;
  m_closeFun = &EBOOKOutputElements::addCloseTableCell;
  m_propList = propList;
}

void OutputHandler::closeTableCell()
{
  closeAnything();
}

void OutputHandler::openTableRow(const RVNGPropertyList &propList)
{
  assert(!m_openFun);
  assert(!m_closeFun);

  m_openFun = &EBOOKOutputElements::addOpenTableRow;
  m_closeFun = &EBOOKOutputElements::addCloseTableRow;
  m_propList = propList;
}

void OutputHandler::closeTableRow()
{
  closeAnything();
}

void OutputHandler::insertBinaryObject(const librevenge::RVNGPropertyList &propList)
{
  if (!m_open)
    open();
  m_output.addInsertBinaryObject(propList);
}

void OutputHandler::insertCoveredTableCell(const librevenge::RVNGPropertyList &propList)
{
  if (!m_open)
    open();
  m_output.addInsertCoveredTableCell(propList);
}

void OutputHandler::insertLineBreak()
{
  if (!m_open)
    open();
  m_output.addInsertLineBreak();
}

void OutputHandler::insertSpace()
{
  if (!m_open)
    open();
  m_output.addInsertSpace();
}

void OutputHandler::insertTab()
{
  if (!m_open)
    open();
  m_output.addInsertTab();
}

void OutputHandler::insertText(const librevenge::RVNGString &text)
{
  if (!m_open)
    open();
  m_output.addInsertText(text);
}

void OutputHandler::closeAnything()
{
  assert(!m_closed);

  if (!m_open && !m_continued)
    open();

  if (m_open)
    close();
}

void OutputHandler::open()
{
  if (!m_dummy)
  {
    assert(m_openFun);

    if (bool(m_parent) && !m_parent->m_open)
      m_parent->open();

    (m_output.*m_openFun)(m_propList);
    m_open = true;
  }
}

void OutputHandler::close()
{
  if (!m_dummy)
  {
    assert(m_closeFun);

    (m_output.*m_closeFun)();
    m_closed = true;
  }
}

}

namespace
{

// NOTE: if changed, update the arrays in getOpenFun() and getCloseFun() too
enum ContextType
{
  CONTEXT_TYPE_ANY,

  CONTEXT_TYPE_DOCUMENT,

  CONTEXT_TYPE_PAGE_SPAN,
  CONTEXT_TYPE_HEADER,
  CONTEXT_TYPE_FOOTER,
  CONTEXT_TYPE_FOOTNOTE,
  CONTEXT_TYPE_PARAGRAPH,
  CONTEXT_TYPE_SPAN,
  CONTEXT_TYPE_LINK,
  CONTEXT_TYPE_ORDERED_LIST,
  CONTEXT_TYPE_UNORDERED_LIST,
  CONTEXT_TYPE_LIST_ELEMENT,
  CONTEXT_TYPE_TABLE,
  CONTEXT_TYPE_TABLE_ROW,
  CONTEXT_TYPE_TABLE_CELL,
  CONTEXT_TYPE_OBJECT,

  LAST_CONTEXT
};

enum EventType
{
  EVENT_TYPE_ANY,

  EVENT_TYPE_CLOSE_FOOTER,
  EVENT_TYPE_CLOSE_FOOTNOTE,
  EVENT_TYPE_CLOSE_HEADER,
  EVENT_TYPE_CLOSE_LINK,
  EVENT_TYPE_CLOSE_LIST_ELEMENT,
  EVENT_TYPE_CLOSE_ORDERED_LIST,
  EVENT_TYPE_CLOSE_PAGE_SPAN,
  EVENT_TYPE_CLOSE_PARAGRAPH,
  EVENT_TYPE_CLOSE_SPAN,
  EVENT_TYPE_CLOSE_TABLE,
  EVENT_TYPE_CLOSE_TABLE_CELL,
  EVENT_TYPE_CLOSE_TABLE_ROW,
  EVENT_TYPE_CLOSE_UNORDERED_LIST,

  EVENT_TYPE_INSERT_COVERED_TABLE_CELL,
  EVENT_TYPE_INSERT_IMAGE,
  EVENT_TYPE_INSERT_TEXT,

  EVENT_TYPE_OPEN_FOOTER,
  EVENT_TYPE_OPEN_FOOTNOTE,
  EVENT_TYPE_OPEN_HEADER,
  EVENT_TYPE_OPEN_LINK,
  EVENT_TYPE_OPEN_LIST_ELEMENT,
  EVENT_TYPE_OPEN_ORDERED_LIST,
  EVENT_TYPE_OPEN_PAGE_SPAN,
  EVENT_TYPE_OPEN_PARAGRAPH,
  EVENT_TYPE_OPEN_SPAN,
  EVENT_TYPE_OPEN_TABLE,
  EVENT_TYPE_OPEN_TABLE_CELL,
  EVENT_TYPE_OPEN_TABLE_ROW,
  EVENT_TYPE_OPEN_UNORDERED_LIST,

  EVENT_TYPE_SAVE,
  EVENT_TYPE_RESTORE,

  LAST_EVENT
};

class StackMachine;
struct Context;
struct Event;

typedef void (*ActionFun_t)(StackMachine &, const Context &context, const Event &event, bool repeat);

typedef void (OutputHandler::*OpenFun_t)(const RVNGPropertyList &props);
typedef void (OutputHandler::*CloseFun_t)();

struct Transition
{
  ContextType context;
  EventType event;
  ContextType newContext;
  ActionFun_t action;
  bool repeat;
};

struct Action
{
  Action();
  explicit Action(const Transition &transition);

  ContextType context;
  ActionFun_t action;
  bool repeat;
};

struct Context
{
  OutputHandlerPtr_t output;
  ContextType type;

  // Indicates contexts that have been artificially created to maintain
  // correct nesting. They need to be popped automatically too.
  bool artificial;

  /** Indicates that this is a continuation of an interrupted context.
    *
    * It will not be added into output if it is empty.
    */
  bool continued;

  /** Restore one (or more) contexts on close.
    */
  bool restore;

  /** Indicate whether restoring should continue with another context.
    *
    * In other words, @true means that this context is inside a list of
    * contexts that should be restored together.
    */
  bool restoring;

  Context(ContextType context, EBOOKOutputElements &outputElements);
  Context(ContextType context, EBOOKOutputElements &outputElements, const Context &parent);
};

struct Event
{
  EventType type;
  bool artificial;

  explicit Event(EventType type_, bool artificial_ = false);
};

Action::Action()
  : context(LAST_CONTEXT)
  , action(0)
  , repeat(false)
{
}

Action::Action(const Transition &transition)
  : context(transition.newContext)
  , action(transition.action)
  , repeat(transition.repeat)
{
}

Context::Context(const ContextType type_, EBOOKOutputElements &outputElements)
  : output()
  , type(type_)
  , artificial(false)
  , continued(false)
  , restore(false)
  , restoring(false)
{
  assert(LAST_CONTEXT > type);

  output.reset(new OutputHandler(outputElements));
}

Context::Context(const ContextType type_, EBOOKOutputElements &outputElements, const Context &parent)
  : output()
  , type(type_)
  , artificial(false)
  , continued(false)
  , restore(false)
  , restoring(false)
{
  assert(LAST_CONTEXT > type);
  assert(bool(parent.output));

  output.reset(new OutputHandler(outputElements, parent.output));
}

Event::Event(EventType type_, bool artificial_)
  : type(type_)
  , artificial(artificial_)
{
  assert(LAST_EVENT > type);
}

}

namespace
{

/** A mechanism to maintain proper nesting of output calls.
  *
  * It is expected that all collector calls are properly paired, but
  * there are other problematic cases that must be handled:
  * * incorrectly nested elements, e.g., a paragraph within a span
  * * multiply nested elements, e.g., nested spans
  *   + the fix is to close the current call before proceeding
  * * missing calls, e.g., a span without the wrapping paragraph
  *   + the fix is to insert dummy call(s)
  *
  * The class uses two stacks to achieve that (both are actually in @c
  * XMLCollector::State), both holding values of type @c Context.
  * contextStack contains the calls that are
  * currently opened. savedStack those that were temporatily put aside
  * because of incorrect nesting. The variables that are used to
  * handle the saved contexts are:
  * * @c restore: If set on a context that is just being popped from @c
  *   contextStack, there is at least one context on @c savedStack that
  *   must be restored.
  * * @c restoring
  *   + On @c savedStack, this means that there is another context that
  *     must be restored too.
  *   + On @c contextStack, it means that at least one context has been
  *     saved since the last push. It can only occur on the top element.
  * * @c continued: A notification that the context has been interrupted
  *   (therefore saved) at least once.
  *
  * @seealso OutputHandler
  */
class StackMachine
{
  typedef std::list<Event> EventQueue_t;
  typedef Action Actions_t[LAST_EVENT];
  typedef Actions_t TransitionTable_t[LAST_CONTEXT];

public:
  StackMachine(const Transition *transitions, size_t count);
  ~StackMachine();

  void initialize(const shared_ptr<XMLCollector::State> &data, ContextType init);

  void processQueue();
  void reactTo(const Event &event);
  void addEvent(const Event &event);

  const shared_ptr<XMLCollector::State> &getData() const;

private:
  void buildTransitionTable(const Transition *transitions, size_t count);

  void processEvent(const Event &event);

private:
  TransitionTable_t m_transitions; //< Transition table.
  EventQueue_t m_queue; //< Queue of yet unprocessed events.
  shared_ptr<XMLCollector::State> m_data; //< State data, including the stacks.
  ContextType m_init; //< Initial context.
};

}

struct XMLCollector::State
{
  EBOOKOutputElements output;
  RVNGPropertyList metadata;

  StackMachine machine;

  stack<Context> contextStack;
  stack<Context> savedStack;

  const RVNGPropertyList *currentProps;

  State();

  const OutputHandlerPtr_t getCurrentOutput() const;

private:
  // disable copying
  State(const State &);
  State &operator=(const State &);
};

namespace
{

StackMachine::StackMachine(const Transition *const transitions, const size_t count)
  : m_transitions()
  , m_queue()
  , m_data()
  , m_init()
{
  for (size_t i = CONTEXT_TYPE_ANY; i != LAST_CONTEXT; ++i)
    std::uninitialized_fill_n(&m_transitions[i][EVENT_TYPE_ANY], size_t(LAST_EVENT), Action());

  buildTransitionTable(transitions, count);
}

StackMachine::~StackMachine()
{
  assert(bool(m_data));
  assert(m_data->savedStack.empty());
  assert(!m_data->contextStack.empty());
  assert(m_init == m_data->contextStack.top().type);
}

void StackMachine::initialize(const shared_ptr<XMLCollector::State> &data, const ContextType init)
{
  m_data = data;
  m_data->contextStack.push(Context(init, m_data->output));
}

void StackMachine::processQueue()
{
  while (!m_queue.empty())
  {
    const Event event = m_queue.front();
    m_queue.pop_front();
    processEvent(event);
  }
}

void StackMachine::reactTo(const Event &event)
{
  assert(m_queue.empty());

  addEvent(event);
  processQueue();
}

void StackMachine::addEvent(const Event &event)
{
  m_queue.push_back(event);
}

const shared_ptr<XMLCollector::State> &StackMachine::getData() const
{
  return m_data;
}

void StackMachine::buildTransitionTable(const Transition *const transitions, const size_t count)
{
  for (size_t i = 0; count != i; ++i)
  {
    assert(LAST_CONTEXT > transitions[i].context);
    assert(LAST_CONTEXT > transitions[i].newContext);
    assert(LAST_EVENT > transitions[i].event);
    assert(transitions[i].event);

    const EventType &event = transitions[i].event;

    if (CONTEXT_TYPE_ANY == transitions[i].context)
    {
      for (size_t context = CONTEXT_TYPE_ANY; LAST_CONTEXT != context; ++context)
      {
        // we do not allow duplicate defs
        assert(LAST_CONTEXT == m_transitions[context][event].context);

        Action action(transitions[i]);
        action.context = static_cast<ContextType>(context);
        m_transitions[context][event] = action;
      }
    }
    else
    {
      // we do not allow duplicate defs
      assert(LAST_CONTEXT == m_transitions[transitions[i].context][event].context);

      m_transitions[transitions[i].context][event] = Action(transitions[i]);
    }
  }
}

void StackMachine::processEvent(const Event &event)
{
  assert(bool(m_data));
  assert(!m_data->contextStack.empty());
  assert(LAST_EVENT > event.type);
  assert(EVENT_TYPE_ANY != event.type);

  Context context = m_data->contextStack.top();
  const Action &action = m_transitions[context.type][event.type];

  assert(LAST_CONTEXT > action.context);
  assert(CONTEXT_TYPE_ANY != action.context);

  if (context.type != action.context)
  {
    Context newContext(action.context, m_data->output, context);

    // at least one context has been saved and needs to be restored later
    if (context.restoring)
    {
      // do not forget to update both copy and original
      m_data->contextStack.top().restoring = context.restoring = false;
      newContext.restore = true;
    }

    m_data->contextStack.push(newContext);
  }

  if (action.action)
    (action.action)(*this, context, event, action.repeat);

  if (action.repeat)
    addEvent(event);
}

}

namespace
{

void doOpenSomething(StackMachine &machine, const Context &context, const Event &event, bool artificial);
void doCloseSomething(StackMachine &machine, const Context &context, const Event &event, bool artificial);

void doInsertCoveredTableCell(StackMachine &machine, const Context &context, const Event &event, bool artificial);
void doInsertImage(StackMachine &machine, const Context &context, const Event &event, bool artificial);
void doInsertText(StackMachine &machine, const Context &context, const Event &event, bool artificial);

void doLeaveContext(StackMachine &machine, const Context &context, const Event &event, bool artificial);
void doRestoreContext(StackMachine &machine, const Context &context, const Event &event, bool artificial);

#define TR_SoftBookL(c, e, n, a, r) {c, e, n, a, r}
// transition without change of context
#define TR(c, e, a) TR_SoftBookL(c, e, c, a, false)
// transition without change of context, with repeat
#define TR_R(c, e, a) TR_SoftBookL(c, e, c, a, true)
// transition changing context to n
#define TR_NEW(c, e, n, a) TR_SoftBookL(c, e, n, a, false)
#define TR_NEW_R(c, e, n, a) TR_SoftBookL(c, e, n, a, true)
// transition changing context arbitrarily, as result of action
#define TR_ANY(c, e, a) TR_SoftBookL(c, e, c, a, false)
#define TR_ANY_R(c, e, a) TR_SoftBookL(c, e, c, a, true)

static Transition transitionList[] =
{
  TR_ANY(CONTEXT_TYPE_ANY, EVENT_TYPE_SAVE, doLeaveContext),
  TR_ANY(CONTEXT_TYPE_ANY, EVENT_TYPE_RESTORE, doRestoreContext),

  // normal content
  TR_NEW(CONTEXT_TYPE_DOCUMENT, EVENT_TYPE_OPEN_PAGE_SPAN, CONTEXT_TYPE_PAGE_SPAN, doOpenSomething),
  // automatic content
  TR_NEW_R(CONTEXT_TYPE_DOCUMENT, EVENT_TYPE_INSERT_TEXT, CONTEXT_TYPE_PAGE_SPAN, doOpenSomething),
  TR_NEW_R(CONTEXT_TYPE_DOCUMENT, EVENT_TYPE_OPEN_FOOTER, CONTEXT_TYPE_PAGE_SPAN, doOpenSomething),
  TR_NEW_R(CONTEXT_TYPE_DOCUMENT, EVENT_TYPE_OPEN_FOOTNOTE, CONTEXT_TYPE_PAGE_SPAN, doOpenSomething),
  TR_NEW_R(CONTEXT_TYPE_DOCUMENT, EVENT_TYPE_OPEN_HEADER, CONTEXT_TYPE_PAGE_SPAN, doOpenSomething),
  TR_NEW_R(CONTEXT_TYPE_DOCUMENT, EVENT_TYPE_OPEN_LINK, CONTEXT_TYPE_PAGE_SPAN, doOpenSomething),
  TR_NEW_R(CONTEXT_TYPE_DOCUMENT, EVENT_TYPE_OPEN_ORDERED_LIST, CONTEXT_TYPE_PAGE_SPAN, doOpenSomething),
  TR_NEW_R(CONTEXT_TYPE_DOCUMENT, EVENT_TYPE_OPEN_PARAGRAPH, CONTEXT_TYPE_PAGE_SPAN, doOpenSomething),
  TR_NEW_R(CONTEXT_TYPE_DOCUMENT, EVENT_TYPE_OPEN_SPAN, CONTEXT_TYPE_PAGE_SPAN, doOpenSomething),
  TR_NEW_R(CONTEXT_TYPE_DOCUMENT, EVENT_TYPE_OPEN_TABLE, CONTEXT_TYPE_PAGE_SPAN, doOpenSomething),
  TR_NEW_R(CONTEXT_TYPE_DOCUMENT, EVENT_TYPE_OPEN_UNORDERED_LIST, CONTEXT_TYPE_PAGE_SPAN, doOpenSomething),

  TR_ANY(CONTEXT_TYPE_PAGE_SPAN, EVENT_TYPE_CLOSE_PAGE_SPAN, doCloseSomething),
  // normal content
  TR(CONTEXT_TYPE_PAGE_SPAN, EVENT_TYPE_INSERT_IMAGE, doInsertImage),
  TR_NEW(CONTEXT_TYPE_PAGE_SPAN, EVENT_TYPE_OPEN_FOOTER, CONTEXT_TYPE_FOOTER, doOpenSomething),
  TR_NEW(CONTEXT_TYPE_PAGE_SPAN, EVENT_TYPE_OPEN_FOOTNOTE, CONTEXT_TYPE_FOOTNOTE, doOpenSomething),
  TR_NEW(CONTEXT_TYPE_PAGE_SPAN, EVENT_TYPE_OPEN_HEADER, CONTEXT_TYPE_HEADER, doOpenSomething),
  TR_NEW(CONTEXT_TYPE_PAGE_SPAN, EVENT_TYPE_OPEN_ORDERED_LIST, CONTEXT_TYPE_ORDERED_LIST, doOpenSomething),
  TR_NEW(CONTEXT_TYPE_PAGE_SPAN, EVENT_TYPE_OPEN_PARAGRAPH, CONTEXT_TYPE_PARAGRAPH, doOpenSomething),
  TR_NEW(CONTEXT_TYPE_PAGE_SPAN, EVENT_TYPE_OPEN_TABLE, CONTEXT_TYPE_TABLE, doOpenSomething),
  TR_NEW(CONTEXT_TYPE_PAGE_SPAN, EVENT_TYPE_OPEN_UNORDERED_LIST, CONTEXT_TYPE_UNORDERED_LIST, doOpenSomething),
  // automatic content
  TR_NEW_R(CONTEXT_TYPE_PAGE_SPAN, EVENT_TYPE_INSERT_TEXT, CONTEXT_TYPE_PARAGRAPH, doOpenSomething),
  TR_NEW_R(CONTEXT_TYPE_PAGE_SPAN, EVENT_TYPE_OPEN_LINK, CONTEXT_TYPE_PARAGRAPH, doOpenSomething),
  TR_NEW_R(CONTEXT_TYPE_PAGE_SPAN, EVENT_TYPE_OPEN_SPAN, CONTEXT_TYPE_PARAGRAPH, doOpenSomething),
  // nested page span -> save and continue later
  TR_ANY_R(CONTEXT_TYPE_PAGE_SPAN, EVENT_TYPE_OPEN_PAGE_SPAN, doLeaveContext),

  TR_ANY(CONTEXT_TYPE_HEADER, EVENT_TYPE_CLOSE_HEADER, doCloseSomething),
  // normal content
  TR(CONTEXT_TYPE_HEADER, EVENT_TYPE_INSERT_IMAGE, doInsertImage),
  TR_NEW(CONTEXT_TYPE_HEADER, EVENT_TYPE_OPEN_ORDERED_LIST, CONTEXT_TYPE_ORDERED_LIST, doOpenSomething),
  TR_NEW(CONTEXT_TYPE_HEADER, EVENT_TYPE_OPEN_PARAGRAPH, CONTEXT_TYPE_PARAGRAPH, doOpenSomething),
  TR_NEW(CONTEXT_TYPE_HEADER, EVENT_TYPE_OPEN_TABLE, CONTEXT_TYPE_TABLE, doOpenSomething),
  TR_NEW(CONTEXT_TYPE_HEADER, EVENT_TYPE_OPEN_UNORDERED_LIST, CONTEXT_TYPE_UNORDERED_LIST, doOpenSomething),
  // automatic content
  TR_NEW_R(CONTEXT_TYPE_HEADER, EVENT_TYPE_INSERT_TEXT, CONTEXT_TYPE_PARAGRAPH, doOpenSomething),
  TR_NEW_R(CONTEXT_TYPE_HEADER, EVENT_TYPE_OPEN_LINK, CONTEXT_TYPE_PARAGRAPH, doOpenSomething),
  TR_NEW_R(CONTEXT_TYPE_HEADER, EVENT_TYPE_OPEN_SPAN, CONTEXT_TYPE_PARAGRAPH, doOpenSomething),

  TR_ANY(CONTEXT_TYPE_FOOTER, EVENT_TYPE_CLOSE_FOOTER, doCloseSomething),
  // normal content
  TR(CONTEXT_TYPE_FOOTER, EVENT_TYPE_INSERT_IMAGE, doInsertImage),
  TR_NEW(CONTEXT_TYPE_FOOTER, EVENT_TYPE_OPEN_ORDERED_LIST, CONTEXT_TYPE_ORDERED_LIST, doOpenSomething),
  TR_NEW(CONTEXT_TYPE_FOOTER, EVENT_TYPE_OPEN_PARAGRAPH, CONTEXT_TYPE_PARAGRAPH, doOpenSomething),
  TR_NEW(CONTEXT_TYPE_FOOTER, EVENT_TYPE_OPEN_TABLE, CONTEXT_TYPE_TABLE, doOpenSomething),
  TR_NEW(CONTEXT_TYPE_FOOTER, EVENT_TYPE_OPEN_UNORDERED_LIST, CONTEXT_TYPE_UNORDERED_LIST, doOpenSomething),
  // automatic content
  TR_NEW_R(CONTEXT_TYPE_FOOTER, EVENT_TYPE_INSERT_TEXT, CONTEXT_TYPE_PARAGRAPH, doOpenSomething),
  TR_NEW_R(CONTEXT_TYPE_FOOTER, EVENT_TYPE_OPEN_LINK, CONTEXT_TYPE_PARAGRAPH, doOpenSomething),
  TR_NEW_R(CONTEXT_TYPE_FOOTER, EVENT_TYPE_OPEN_SPAN, CONTEXT_TYPE_PARAGRAPH, doOpenSomething),

  TR_ANY(CONTEXT_TYPE_FOOTNOTE, EVENT_TYPE_CLOSE_FOOTNOTE, doCloseSomething),
  // normal content
  TR_NEW(CONTEXT_TYPE_FOOTNOTE, EVENT_TYPE_OPEN_ORDERED_LIST, CONTEXT_TYPE_ORDERED_LIST, doOpenSomething),
  TR_NEW(CONTEXT_TYPE_FOOTNOTE, EVENT_TYPE_OPEN_PARAGRAPH, CONTEXT_TYPE_PARAGRAPH, doOpenSomething),
  TR_NEW(CONTEXT_TYPE_FOOTNOTE, EVENT_TYPE_OPEN_TABLE, CONTEXT_TYPE_TABLE, doOpenSomething),
  TR_NEW(CONTEXT_TYPE_FOOTNOTE, EVENT_TYPE_OPEN_UNORDERED_LIST, CONTEXT_TYPE_UNORDERED_LIST, doOpenSomething),
  // automatic content
  TR_NEW_R(CONTEXT_TYPE_FOOTNOTE, EVENT_TYPE_INSERT_TEXT, CONTEXT_TYPE_PARAGRAPH, doOpenSomething),
  TR_NEW_R(CONTEXT_TYPE_FOOTNOTE, EVENT_TYPE_OPEN_LINK, CONTEXT_TYPE_PARAGRAPH, doOpenSomething),
  TR_NEW_R(CONTEXT_TYPE_FOOTNOTE, EVENT_TYPE_OPEN_SPAN, CONTEXT_TYPE_PARAGRAPH, doOpenSomething),

  // automatic close
  TR_ANY_R(CONTEXT_TYPE_PARAGRAPH, EVENT_TYPE_CLOSE_FOOTER, doCloseSomething),
  TR_ANY_R(CONTEXT_TYPE_PARAGRAPH, EVENT_TYPE_CLOSE_FOOTNOTE, doCloseSomething),
  TR_ANY_R(CONTEXT_TYPE_PARAGRAPH, EVENT_TYPE_CLOSE_HEADER, doCloseSomething),
  TR_ANY_R(CONTEXT_TYPE_PARAGRAPH, EVENT_TYPE_CLOSE_LIST_ELEMENT, doCloseSomething),
  TR_ANY_R(CONTEXT_TYPE_PARAGRAPH, EVENT_TYPE_CLOSE_PAGE_SPAN, doCloseSomething),
  TR_ANY_R(CONTEXT_TYPE_PARAGRAPH, EVENT_TYPE_CLOSE_TABLE_CELL, doCloseSomething),
  // close
  TR_ANY(CONTEXT_TYPE_PARAGRAPH, EVENT_TYPE_CLOSE_PARAGRAPH, doCloseSomething),
  // normal content
  TR_NEW(CONTEXT_TYPE_PARAGRAPH, EVENT_TYPE_OPEN_LINK, CONTEXT_TYPE_LINK, doOpenSomething),
  TR_NEW(CONTEXT_TYPE_PARAGRAPH, EVENT_TYPE_OPEN_SPAN, CONTEXT_TYPE_SPAN, doOpenSomething),
  // automatic content
  TR_NEW_R(CONTEXT_TYPE_PARAGRAPH, EVENT_TYPE_INSERT_TEXT, CONTEXT_TYPE_SPAN, doOpenSomething),
  // bad nesting -> unwind
  TR_ANY_R(CONTEXT_TYPE_PARAGRAPH, EVENT_TYPE_INSERT_IMAGE, doLeaveContext),
  TR_ANY_R(CONTEXT_TYPE_PARAGRAPH, EVENT_TYPE_OPEN_ORDERED_LIST, doLeaveContext),
  TR_ANY_R(CONTEXT_TYPE_PARAGRAPH, EVENT_TYPE_OPEN_PARAGRAPH, doLeaveContext),
  TR_ANY_R(CONTEXT_TYPE_PARAGRAPH, EVENT_TYPE_OPEN_TABLE, doLeaveContext),
  TR_ANY_R(CONTEXT_TYPE_PARAGRAPH, EVENT_TYPE_OPEN_UNORDERED_LIST, doLeaveContext),

  // automatic close
  TR_ANY_R(CONTEXT_TYPE_SPAN, EVENT_TYPE_CLOSE_LIST_ELEMENT, doCloseSomething),
  TR_ANY_R(CONTEXT_TYPE_SPAN, EVENT_TYPE_CLOSE_ORDERED_LIST, doCloseSomething),
  TR_ANY_R(CONTEXT_TYPE_SPAN, EVENT_TYPE_CLOSE_PARAGRAPH, doCloseSomething),
  TR_ANY_R(CONTEXT_TYPE_SPAN, EVENT_TYPE_CLOSE_TABLE_CELL, doCloseSomething),
  TR_ANY_R(CONTEXT_TYPE_SPAN, EVENT_TYPE_CLOSE_UNORDERED_LIST, doCloseSomething),
  // close
  TR_ANY(CONTEXT_TYPE_SPAN, EVENT_TYPE_CLOSE_SPAN, doCloseSomething),
  // normal content
  TR(CONTEXT_TYPE_SPAN, EVENT_TYPE_INSERT_TEXT, doInsertText),
  // bad nesting -> unwind
  TR_ANY_R(CONTEXT_TYPE_SPAN, EVENT_TYPE_OPEN_LINK, doLeaveContext),
  TR_ANY_R(CONTEXT_TYPE_SPAN, EVENT_TYPE_OPEN_PARAGRAPH, doLeaveContext),
  TR_ANY_R(CONTEXT_TYPE_SPAN, EVENT_TYPE_OPEN_SPAN, doLeaveContext),

  // close
  TR_ANY(CONTEXT_TYPE_LINK, EVENT_TYPE_CLOSE_LINK, doCloseSomething),
  // normal content
  TR(CONTEXT_TYPE_LINK, EVENT_TYPE_INSERT_TEXT, doInsertText),
  TR_NEW(CONTEXT_TYPE_LINK, EVENT_TYPE_OPEN_SPAN, CONTEXT_TYPE_SPAN, doOpenSomething),
  // bad nesting -> unwind
  TR_ANY_R(CONTEXT_TYPE_LINK, EVENT_TYPE_OPEN_LINK, doLeaveContext),
  TR_ANY_R(CONTEXT_TYPE_LINK, EVENT_TYPE_OPEN_PARAGRAPH, doLeaveContext),

  // close
  TR_ANY(CONTEXT_TYPE_ORDERED_LIST, EVENT_TYPE_CLOSE_ORDERED_LIST, doCloseSomething),
  // normal content
  TR_NEW(CONTEXT_TYPE_ORDERED_LIST, EVENT_TYPE_OPEN_LIST_ELEMENT, CONTEXT_TYPE_LIST_ELEMENT, doOpenSomething),
  // automatic content
  TR_NEW_R(CONTEXT_TYPE_ORDERED_LIST, EVENT_TYPE_OPEN_ORDERED_LIST, CONTEXT_TYPE_LIST_ELEMENT, doOpenSomething),
  TR_NEW_R(CONTEXT_TYPE_ORDERED_LIST, EVENT_TYPE_OPEN_UNORDERED_LIST, CONTEXT_TYPE_LIST_ELEMENT, doOpenSomething),
  TR_NEW_R(CONTEXT_TYPE_ORDERED_LIST, EVENT_TYPE_INSERT_TEXT, CONTEXT_TYPE_LIST_ELEMENT, doOpenSomething),
  TR_NEW_R(CONTEXT_TYPE_ORDERED_LIST, EVENT_TYPE_OPEN_LINK, CONTEXT_TYPE_LIST_ELEMENT, doOpenSomething),
  TR_NEW_R(CONTEXT_TYPE_ORDERED_LIST, EVENT_TYPE_OPEN_PARAGRAPH, CONTEXT_TYPE_LIST_ELEMENT, doOpenSomething),
  TR_NEW_R(CONTEXT_TYPE_ORDERED_LIST, EVENT_TYPE_OPEN_SPAN, CONTEXT_TYPE_LIST_ELEMENT, doOpenSomething),

  // close
  TR_ANY(CONTEXT_TYPE_UNORDERED_LIST, EVENT_TYPE_CLOSE_UNORDERED_LIST, doCloseSomething),
  // normal content
  TR_NEW(CONTEXT_TYPE_UNORDERED_LIST, EVENT_TYPE_OPEN_LIST_ELEMENT, CONTEXT_TYPE_LIST_ELEMENT, doOpenSomething),
  // automatic content
  TR_NEW_R(CONTEXT_TYPE_UNORDERED_LIST, EVENT_TYPE_OPEN_ORDERED_LIST, CONTEXT_TYPE_LIST_ELEMENT, doOpenSomething),
  TR_NEW_R(CONTEXT_TYPE_UNORDERED_LIST, EVENT_TYPE_OPEN_UNORDERED_LIST, CONTEXT_TYPE_LIST_ELEMENT, doOpenSomething),
  TR_NEW_R(CONTEXT_TYPE_UNORDERED_LIST, EVENT_TYPE_INSERT_TEXT, CONTEXT_TYPE_LIST_ELEMENT, doOpenSomething),
  TR_NEW_R(CONTEXT_TYPE_UNORDERED_LIST, EVENT_TYPE_OPEN_LINK, CONTEXT_TYPE_LIST_ELEMENT, doOpenSomething),
  TR_NEW_R(CONTEXT_TYPE_UNORDERED_LIST, EVENT_TYPE_OPEN_PARAGRAPH, CONTEXT_TYPE_LIST_ELEMENT, doOpenSomething),
  TR_NEW_R(CONTEXT_TYPE_UNORDERED_LIST, EVENT_TYPE_OPEN_SPAN, CONTEXT_TYPE_LIST_ELEMENT, doOpenSomething),

  // close
  TR_ANY(CONTEXT_TYPE_LIST_ELEMENT, EVENT_TYPE_CLOSE_LIST_ELEMENT, doCloseSomething),
  // automatic close
  TR_ANY_R(CONTEXT_TYPE_LIST_ELEMENT, EVENT_TYPE_CLOSE_ORDERED_LIST, doCloseSomething),
  TR_ANY_R(CONTEXT_TYPE_LIST_ELEMENT, EVENT_TYPE_CLOSE_UNORDERED_LIST, doCloseSomething),
  // normal content
  TR_NEW(CONTEXT_TYPE_LIST_ELEMENT, EVENT_TYPE_OPEN_PARAGRAPH, CONTEXT_TYPE_PARAGRAPH, doOpenSomething),
  TR_NEW(CONTEXT_TYPE_LIST_ELEMENT, EVENT_TYPE_OPEN_ORDERED_LIST, CONTEXT_TYPE_ORDERED_LIST, doOpenSomething),
  TR_NEW(CONTEXT_TYPE_LIST_ELEMENT, EVENT_TYPE_OPEN_UNORDERED_LIST, CONTEXT_TYPE_UNORDERED_LIST, doOpenSomething),
  // automatic content
  TR_NEW_R(CONTEXT_TYPE_LIST_ELEMENT, EVENT_TYPE_INSERT_TEXT, CONTEXT_TYPE_PARAGRAPH, doOpenSomething),
  TR_NEW_R(CONTEXT_TYPE_LIST_ELEMENT, EVENT_TYPE_OPEN_LINK, CONTEXT_TYPE_PARAGRAPH, doOpenSomething),
  TR_NEW_R(CONTEXT_TYPE_LIST_ELEMENT, EVENT_TYPE_OPEN_SPAN, CONTEXT_TYPE_PARAGRAPH, doOpenSomething),

  // close
  TR_ANY(CONTEXT_TYPE_TABLE, EVENT_TYPE_CLOSE_TABLE, doCloseSomething),
  // normal content
  TR_NEW(CONTEXT_TYPE_TABLE, EVENT_TYPE_OPEN_TABLE_ROW, CONTEXT_TYPE_TABLE_ROW, doOpenSomething),

  // close
  TR_ANY(CONTEXT_TYPE_TABLE_ROW, EVENT_TYPE_CLOSE_TABLE_ROW, doCloseSomething),
  // normal content
  TR_NEW(CONTEXT_TYPE_TABLE_ROW, EVENT_TYPE_OPEN_TABLE_CELL, CONTEXT_TYPE_TABLE_CELL, doOpenSomething),
  TR(CONTEXT_TYPE_TABLE_ROW, EVENT_TYPE_INSERT_COVERED_TABLE_CELL, doInsertCoveredTableCell),

  // close
  TR_ANY(CONTEXT_TYPE_TABLE_CELL, EVENT_TYPE_CLOSE_TABLE_CELL, doCloseSomething),
  // normal content
  TR_NEW(CONTEXT_TYPE_TABLE_CELL, EVENT_TYPE_OPEN_ORDERED_LIST, CONTEXT_TYPE_ORDERED_LIST, doOpenSomething),
  TR_NEW(CONTEXT_TYPE_TABLE_CELL, EVENT_TYPE_OPEN_PARAGRAPH, CONTEXT_TYPE_PARAGRAPH, doOpenSomething),
  TR_NEW(CONTEXT_TYPE_TABLE_CELL, EVENT_TYPE_OPEN_UNORDERED_LIST, CONTEXT_TYPE_UNORDERED_LIST, doOpenSomething),
  // automatic content
  TR_NEW_R(CONTEXT_TYPE_TABLE_CELL, EVENT_TYPE_OPEN_LINK, CONTEXT_TYPE_PARAGRAPH, doOpenSomething),
  TR_NEW_R(CONTEXT_TYPE_TABLE_CELL, EVENT_TYPE_OPEN_SPAN, CONTEXT_TYPE_PARAGRAPH, doOpenSomething),
  TR_NEW_R(CONTEXT_TYPE_TABLE_CELL, EVENT_TYPE_INSERT_TEXT, CONTEXT_TYPE_PARAGRAPH, doOpenSomething),
};

#undef TR_SoftBookL
#undef TR
#undef TR_R
#undef TR_NEW
#undef TR_NEW_R
#undef TR_ANY
#undef TR_ANY_R

OpenFun_t getOpenFun(const ContextType type)
{
  static OpenFun_t openMap[] =
  {
    0, // CONTEXT_TYPE_ANY
    0, // CONTEXT_TYPE_DOCUMENT
    &OutputHandler::openPageSpan, // CONTEXT_TYPE_PAGE_SPAN
    &OutputHandler::openHeader, // CONTEXT_TYPE_HEADER
    &OutputHandler::openFooter, // CONTEXT_TYPE_FOOTER
    &OutputHandler::openFootnote, // CONTEXT_TYPE_FOOTNOTE
    &OutputHandler::openParagraph, // CONTEXT_TYPE_PARAGRAPH
    &OutputHandler::openSpan, // CONTEXT_TYPE_SPAN
    &OutputHandler::openLink, // CONTEXT_TYPE_LINK
    &OutputHandler::openOrderedList, // CONTEXT_TYPE_ORDERED_LIST
    &OutputHandler::openUnorderedList, // CONTEXT_TYPE_UNORDERED_LIST
    &OutputHandler::openListElement, // CONTEXT_TYPE_LIST_ELEMENT
    &OutputHandler::openTable, // CONTEXT_TYPE_TABLE
    &OutputHandler::openTableRow, // CONTEXT_TYPE_TABLE_ROW
    &OutputHandler::openTableCell, // CONTEXT_TYPE_TABLE_CELL
    0, // CONTEXT_TYPE_OBJECT
    0
  };

  static_assert(EBOOK_NUM_ELEMENTS(openMap) == LAST_CONTEXT + 1, "# of handlers doesn't match # of contexts");

  return openMap[type];
}

CloseFun_t getCloseFun(const ContextType type)
{
  static CloseFun_t closeMap[] =
  {
    0, // CONTEXT_TYPE_ANY
    0, // CONTEXT_TYPE_DOCUMENT
    &OutputHandler::closePageSpan, // CONTEXT_TYPE_PAGE_SPAN
    &OutputHandler::closeHeader, // CONTEXT_TYPE_HEADER
    &OutputHandler::closeFooter, // CONTEXT_TYPE_FOOTER
    &OutputHandler::closeFootnote, // CONTEXT_TYPE_FOOTNOTE
    &OutputHandler::closeParagraph, // CONTEXT_TYPE_PARAGRAPH
    &OutputHandler::closeSpan, // CONTEXT_TYPE_SPAN
    &OutputHandler::closeLink, // CONTEXT_TYPE_LINK
    &OutputHandler::closeOrderedList, // CONTEXT_TYPE_ORDERED_LIST
    &OutputHandler::closeUnorderedList, // CONTEXT_TYPE_UNORDERED_LIST
    &OutputHandler::closeListElement, // CONTEXT_TYPE_LIST_ELEMENT
    &OutputHandler::closeTable, // CONTEXT_TYPE_TABLE
    &OutputHandler::closeTableRow, // CONTEXT_TYPE_TABLE_ROW
    &OutputHandler::closeTableCell, // CONTEXT_TYPE_TABLE_CELL
    0, // CONTEXT_TYPE_OBJECT
    0
  };

  static_assert(EBOOK_NUM_ELEMENTS(closeMap) == LAST_CONTEXT + 1, "# of handlers doesn't match # of contexts");

  return closeMap[type];
}

EventType getCloseEventType(const ContextType type)
{
  static EventType closeMap[] =
  {
    LAST_EVENT, // CONTEXT_TYPE_ANY
    LAST_EVENT, // CONTEXT_TYPE_DOCUMENT
    EVENT_TYPE_CLOSE_PAGE_SPAN, // CONTEXT_TYPE_PAGE_SPAN
    EVENT_TYPE_CLOSE_HEADER, // CONTEXT_TYPE_HEADER
    EVENT_TYPE_CLOSE_FOOTER, // CONTEXT_TYPE_FOOTER
    EVENT_TYPE_CLOSE_FOOTNOTE, // CONTEXT_TYPE_FOOTNOTE
    EVENT_TYPE_CLOSE_PARAGRAPH, // CONTEXT_TYPE_PARAGRAPH
    EVENT_TYPE_CLOSE_SPAN, // CONTEXT_TYPE_SPAN
    EVENT_TYPE_CLOSE_LINK, // CONTEXT_TYPE_LINK
    EVENT_TYPE_CLOSE_ORDERED_LIST, // CONTEXT_TYPE_ORDERED_LIST
    EVENT_TYPE_CLOSE_UNORDERED_LIST, // CONTEXT_TYPE_UNORDERED_LIST
    EVENT_TYPE_CLOSE_LIST_ELEMENT, // CONTEXT_TYPE_LIST_ELEMENT
    EVENT_TYPE_CLOSE_TABLE, // CONTEXT_TYPE_TABLE
    EVENT_TYPE_CLOSE_TABLE_ROW, // CONTEXT_TYPE_TABLE_ROW
    EVENT_TYPE_CLOSE_TABLE_CELL, // CONTEXT_TYPE_TABLE_CELL
    LAST_EVENT, // CONTEXT_TYPE_OBJECT
    LAST_EVENT
  };

  static_assert(EBOOK_NUM_ELEMENTS(closeMap) == LAST_CONTEXT + 1, "# of events doesn't match # of contexts");

  return closeMap[type];
}

void doOpenSomething(StackMachine &machine, const Context &, const Event &, const bool artificial)
{
  assert(!machine.getData()->contextStack.empty());

  const shared_ptr<XMLCollector::State> data = machine.getData();

  if (artificial)
    data->contextStack.top().artificial = true;

  const OpenFun_t open = getOpenFun(data->contextStack.top().type);
  assert(open);

  if (machine.getData()->currentProps)
    (data->getCurrentOutput().get()->*open)(*data->currentProps);
  else
    (data->getCurrentOutput().get()->*open)(RVNGPropertyList());
}

void doCloseSomething(StackMachine &machine, const Context &context, const Event &, bool)
{
  if (context.restore)
    machine.addEvent(Event(EVENT_TYPE_RESTORE, true));

  const CloseFun_t close = getCloseFun(context.type);
  assert(close);

  (machine.getData()->getCurrentOutput().get()->*close)();

  machine.getData()->contextStack.pop();
}

void doInsertCoveredTableCell(StackMachine &machine, const Context &context, const Event &event, const bool artificial)
{
  // TODO: implement me
  (void) machine;
  (void) context;
  (void) event;
  (void) artificial;
}

void doInsertImage(StackMachine &machine, const Context &context, const Event &event, const bool artificial)
{
  // TODO: implement me
  (void) machine;
  (void) context;
  (void) event;
  (void) artificial;
}

void doInsertText(StackMachine &machine, const Context &context, const Event &event, const bool artificial)
{
  // TODO: implement me
  (void) machine;
  (void) context;
  (void) event;
  (void) artificial;
}

void saveContext(StackMachine &machine)
{
  assert(!machine.getData()->contextStack.empty());

  const shared_ptr<XMLCollector::State> data = machine.getData();

  data->getCurrentOutput()->reset();

  data->savedStack.push(data->contextStack.top());
  data->contextStack.pop();
  // This serves to signalize that at least context has been saved, even
  // if the present top context is not going to be. In that case it is
  // reset again in the processing loop.
  data->contextStack.top().restoring = true;
}

void doLeaveContext(StackMachine &machine, const Context &context, const Event &event, bool)
{
  assert(!machine.getData()->contextStack.empty());

  const shared_ptr<XMLCollector::State> data = machine.getData();

  if (data->contextStack.top().artificial && !data->contextStack.top().restoring)
    doCloseSomething(machine, context, event, true);
  else
    saveContext(machine);
}

void doRestoreContext(StackMachine &machine, const Context &, const Event &, bool)
{
  assert(!machine.getData()->savedStack.empty());

  const shared_ptr<XMLCollector::State> data = machine.getData();

  data->contextStack.push(data->savedStack.top());
  data->savedStack.pop();

  if (data->contextStack.top().restoring)
    machine.addEvent(Event(EVENT_TYPE_RESTORE, true));

  data->contextStack.top().restoring = false;
  data->contextStack.top().continued = true;
}

}

namespace
{

class PropertiesGuard
{
public:
  PropertiesGuard(const shared_ptr<XMLCollector::State> &state, const RVNGPropertyList *props);
  ~PropertiesGuard();

private:
  const shared_ptr<XMLCollector::State> m_state;
};

PropertiesGuard::PropertiesGuard(const shared_ptr<XMLCollector::State> &state, const RVNGPropertyList *const props)
  : m_state(state)
{
  assert(bool(m_state));
  assert(!m_state->currentProps);

  m_state->currentProps = props;
}

PropertiesGuard::~PropertiesGuard()
{
  m_state->currentProps = 0;
}

void updateContext(const shared_ptr<XMLCollector::State> &state, const EventType event, const RVNGPropertyList *const props = 0)
{
  assert(bool(state));
  assert(LAST_EVENT > event);

  const PropertiesGuard guard(state, props);
  state->machine.reactTo(Event(event));
}

}

XMLCollector::State::State()
  : output()
  , metadata()
  , machine(transitionList, EBOOK_NUM_ELEMENTS(transitionList))
  , contextStack()
  , savedStack()
  , currentProps(0)
{
}

const OutputHandlerPtr_t XMLCollector::State::getCurrentOutput() const
{
  assert(!contextStack.empty());

  return contextStack.top().output;
}

XMLCollector::XMLCollector(librevenge::RVNGTextInterface *document)
  : m_document(document)
  , m_state(new State())
{
  m_state->machine.initialize(m_state, CONTEXT_TYPE_DOCUMENT);
  m_state->getCurrentOutput()->openDocument(RVNGPropertyList());
}

void XMLCollector::collectMetadata(const RVNGPropertyList &metadata)
{
  m_state->metadata = metadata;
}

void XMLCollector::collectText(const std::string &text)
{
  updateContext(m_state, EVENT_TYPE_INSERT_TEXT);
  m_state->getCurrentOutput()->insertText(RVNGString(text.c_str()));
}

void XMLCollector::collectSpace()
{
  updateContext(m_state, EVENT_TYPE_INSERT_TEXT);
  m_state->getCurrentOutput()->insertSpace();
}

void XMLCollector::collectTab()
{
  updateContext(m_state, EVENT_TYPE_INSERT_TEXT);
  m_state->getCurrentOutput()->insertTab();
}

void XMLCollector::collectLineBreak()
{
  updateContext(m_state, EVENT_TYPE_INSERT_TEXT);
  m_state->getCurrentOutput()->insertLineBreak();
}

void XMLCollector::collectImage(const librevenge::RVNGPropertyList &props)
{
  updateContext(m_state, EVENT_TYPE_INSERT_IMAGE, &props);

  // TODO: implement me
  (void) props;
  // m_state->output.addInsertBinaryObject();
}

void XMLCollector::collectCoveredTableCell(const librevenge::RVNGPropertyList &props)
{
  updateContext(m_state, EVENT_TYPE_INSERT_COVERED_TABLE_CELL, &props);
  m_state->getCurrentOutput()->insertCoveredTableCell(props);
}

void XMLCollector::openPageSpan(const librevenge::RVNGPropertyList &props)
{
  updateContext(m_state, EVENT_TYPE_OPEN_PAGE_SPAN, &props);
}

void XMLCollector::closePageSpan()
{
  updateContext(m_state, EVENT_TYPE_CLOSE_PAGE_SPAN);
}

void XMLCollector::openHeader(const librevenge::RVNGPropertyList &props)
{
  updateContext(m_state, EVENT_TYPE_OPEN_HEADER, &props);
}

void XMLCollector::closeHeader()
{
  updateContext(m_state, EVENT_TYPE_CLOSE_HEADER);
}

void XMLCollector::openFooter(const librevenge::RVNGPropertyList &props)
{
  updateContext(m_state, EVENT_TYPE_OPEN_FOOTER, &props);
}

void XMLCollector::closeFooter()
{
  updateContext(m_state, EVENT_TYPE_CLOSE_FOOTER);
}

void XMLCollector::openFootnote(const librevenge::RVNGPropertyList &props)
{
  updateContext(m_state, EVENT_TYPE_OPEN_FOOTNOTE, &props);
}

void XMLCollector::closeFootnote()
{
  updateContext(m_state, EVENT_TYPE_CLOSE_FOOTNOTE);
}

void XMLCollector::openParagraph(const librevenge::RVNGPropertyList &props)
{
  updateContext(m_state, EVENT_TYPE_OPEN_PARAGRAPH, &props);
}

void XMLCollector::closeParagraph()
{
  updateContext(m_state, EVENT_TYPE_CLOSE_PARAGRAPH);
}

void XMLCollector::openSpan(const librevenge::RVNGPropertyList &props)
{
  updateContext(m_state, EVENT_TYPE_OPEN_SPAN, &props);
}

void XMLCollector::closeSpan()
{
  updateContext(m_state, EVENT_TYPE_CLOSE_SPAN);
}

void XMLCollector::openLink(const librevenge::RVNGPropertyList &props)
{
  updateContext(m_state, EVENT_TYPE_OPEN_LINK, &props);
}

void XMLCollector::closeLink()
{
  updateContext(m_state, EVENT_TYPE_CLOSE_LINK);
}

void XMLCollector::openTable(const librevenge::RVNGPropertyList &props)
{
  updateContext(m_state, EVENT_TYPE_OPEN_TABLE, &props);
}

void XMLCollector::closeTable()
{
  updateContext(m_state, EVENT_TYPE_CLOSE_TABLE);
}

void XMLCollector::openTableRow(const librevenge::RVNGPropertyList &props)
{
  updateContext(m_state, EVENT_TYPE_OPEN_TABLE_ROW, &props);
}

void XMLCollector::closeTableRow()
{
  updateContext(m_state, EVENT_TYPE_CLOSE_TABLE_ROW);
}

void XMLCollector::openTableCell(const librevenge::RVNGPropertyList &props)
{
  updateContext(m_state, EVENT_TYPE_OPEN_TABLE_CELL, &props);
}

void XMLCollector::closeTableCell()
{
  updateContext(m_state, EVENT_TYPE_CLOSE_TABLE_CELL);
}

void XMLCollector::openOrderedList(const librevenge::RVNGPropertyList &props)
{
  updateContext(m_state, EVENT_TYPE_OPEN_ORDERED_LIST, &props);
}

void XMLCollector::closeOrderedList()
{
  updateContext(m_state, EVENT_TYPE_CLOSE_ORDERED_LIST);
}

void XMLCollector::openUnorderedList(const librevenge::RVNGPropertyList &props)
{
  updateContext(m_state, EVENT_TYPE_OPEN_UNORDERED_LIST, &props);
}

void XMLCollector::closeUnorderedList()
{
  updateContext(m_state, EVENT_TYPE_CLOSE_UNORDERED_LIST);
}

void XMLCollector::openListElement(const librevenge::RVNGPropertyList &props)
{
  updateContext(m_state, EVENT_TYPE_OPEN_LIST_ELEMENT, &props);
}

void XMLCollector::closeListElement()
{
  updateContext(m_state, EVENT_TYPE_CLOSE_LIST_ELEMENT);
}

void XMLCollector::openObject(const librevenge::RVNGPropertyList &props)
{
  // TODO: implement me
  (void) props;
}

void XMLCollector::closeObject()
{
  // TODO: implement me
}

void XMLCollector::finish()
{
  assert(!m_state->contextStack.empty());

  // close all remaining contexts (which should all be automatic)
  while (CONTEXT_TYPE_DOCUMENT != m_state->contextStack.top().type)
  {
    assert(m_state->contextStack.top().artificial);

    m_state->machine.reactTo(Event(getCloseEventType(m_state->contextStack.top().type)));
  }

  assert(m_state->savedStack.empty());

  if (m_document)
  {
    m_document->startDocument(m_state->metadata);
    m_state->output.write(m_document);
    m_document->endDocument();
  }
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
