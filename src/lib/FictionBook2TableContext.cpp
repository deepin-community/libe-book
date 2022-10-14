/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <algorithm>
#include <cstdlib>
#include <deque>

#include "FictionBook2Collector.h"
#include "FictionBook2TableContext.h"
#include "FictionBook2Token.h"

namespace libebook
{

class FictionBook2TableModel
{
  typedef std::deque<bool> Row_t;
  typedef std::deque<Row_t> Table_t;

public:
  FictionBook2TableModel();

  /** Add a row.
    *
    * @return then number of cells covered at the beginning of the row.
    */
  size_t addRow();

  /** Add a cell spanning @c rowSpan rows and @c colSpan columns.
    *
    * @return the total number of following covered cells.
    */
  size_t addCell(size_t rowSpan, size_t columnSpan);

private:
  void ensureColumns(Row_t &row);

private:
  Table_t m_table;
  size_t m_rows;
  size_t m_columns;
  size_t m_current_row;
  size_t m_current_column;
};

FictionBook2TableModel::FictionBook2TableModel()
  : m_table()
  , m_rows(0)
  , m_columns(0)
  , m_current_row(0)
  , m_current_column(0)
{
}

size_t FictionBook2TableModel::addRow()
{
  if (m_rows > 0)
    ++m_current_row;
  m_current_column = 0;

  if (m_current_row == m_rows)
  {
    m_table.push_back(Row_t(m_columns, false));
    ++m_rows;
    return 0;
  }

  size_t covered = 0;
  const Row_t &row = m_table[m_current_row];
  for (; (covered != row.size()) && row[covered]; ++covered)
    ;
  return covered;
}

size_t FictionBook2TableModel::addCell(const size_t rowSpan, const size_t columnSpan)
{
  // make sure the cell and all the covered cells fit into the current row
  const size_t addedColumns = (columnSpan > 0) ? columnSpan : 1;
  if ((m_current_column + addedColumns) > m_columns)
    m_columns = m_current_column + addedColumns;

  if (rowSpan != 0)
  {
    for (; m_rows < (m_current_row + rowSpan); ++m_rows)
      m_table.push_back(Row_t(m_columns, false));
    for (size_t row = m_current_row + 1; row < m_current_row + rowSpan; ++row)
    {
      ensureColumns(m_table[row]);
      m_table[row][m_current_column] = true;
    }
  }

  ++m_current_column;

  size_t column = m_current_column;

  {
    Row_t &row = m_table[m_current_row];

    ensureColumns(row);

    // cover the cells
    std::fill_n(row.begin() + long(m_current_column), addedColumns - 1, true);
    m_current_column += addedColumns;

    // find the next uncovered cell position in the current row
    while ((m_current_column < m_columns) && row[m_current_column])
      ++m_current_column;
  }

  return m_current_column - column - 1;
}

void FictionBook2TableModel::ensureColumns(Row_t &row)
{
  if (row.size() < m_columns)
    row.insert(row.end(), m_columns - row.size(), false);
}

FictionBook2TableContext::FictionBook2TableContext(FictionBook2ParserContext *const parentContext, const FictionBook2BlockFormat &format)
  : FictionBook2BlockFormatContextBase(parentContext, format)
  , m_model(new FictionBook2TableModel())
{
}

FictionBook2TableContext::~FictionBook2TableContext()
{
}

FictionBook2XMLParserContext *FictionBook2TableContext::element(const FictionBook2TokenData &name, const FictionBook2TokenData &ns)
{
  if (FictionBook2Token::NS_FICTIONBOOK == getFictionBook2TokenID(ns))
  {
    switch (getFictionBook2TokenID(name))
    {
    case FictionBook2Token::tr :
      return new FictionBook2TrContext(this, m_model, getBlockFormat());
    default :
      break;
    }
  }

  return new FictionBook2SkipElementContext(this);
}

void FictionBook2TableContext::startOfElement()
{
  getCollector()->openTable(getBlockFormat());
}

void FictionBook2TableContext::endOfElement()
{
  getCollector()->closeTable();
}

void FictionBook2TableContext::attribute(const FictionBook2TokenData &name, const FictionBook2TokenData *ns, const char *value)
{
  if (FictionBook2_NO_NAMESPACE(ns))
  {
    switch (getFictionBook2TokenID(name))
    {
    case FictionBook2Token::id :
      getCollector()->defineID(value);
      break;
    case FictionBook2Token::style :
      // ignore
      break;
    default :
      break;
    }
  }
}

FictionBook2CellContext::FictionBook2CellContext(FictionBook2ParserContext *const parentContext, const std::shared_ptr<FictionBook2TableModel> &model, const FictionBook2BlockFormat &format, const bool header)
  : FictionBook2StyleContextBase(parentContext, FictionBook2Style(format))
  , m_model(model)
  , m_header(header)
  , m_opened(false)
  , m_columnSpan(0)
  , m_rowSpan(0)
  , m_coveredColumns(0)
{
}

void FictionBook2CellContext::startOfElement()
{
}

void FictionBook2CellContext::endOfElement()
{
  if (!m_opened)
    openCell();

  getCollector()->closeTableCell();
  size_t covered = m_coveredColumns;
  for (; covered > 0; --covered)
    getCollector()->insertCoveredTableCell();
}

void FictionBook2CellContext::attribute(const FictionBook2TokenData &name, const FictionBook2TokenData *ns, const char *value)
{
  if (FictionBook2_NO_NAMESPACE(ns))
  {
    switch (getFictionBook2TokenID(name))
    {
    case FictionBook2Token::colspan :
      m_columnSpan = std::atoi(value);
      break;
    case FictionBook2Token::rowspan :
      m_rowSpan = std::atoi(value);
      break;
    case FictionBook2Token::align :
      // TODO: handle this
      break;
    case FictionBook2Token::id :
      getCollector()->defineID(value);
      break;
    case FictionBook2Token::style :
      // ignore
      break;
    default :
      break;
    }
  }
}

void FictionBook2CellContext::endOfAttributes()
{
  openCell();
}

void FictionBook2CellContext::openCell()
{
  m_coveredColumns = m_model->addCell(size_t(m_rowSpan), size_t(m_columnSpan));

  getCollector()->openTableCell(m_rowSpan, m_columnSpan);
  m_opened = true;
}

FictionBook2TrContext::FictionBook2TrContext(FictionBook2ParserContext *const parentContext, const std::shared_ptr<FictionBook2TableModel> &model, const FictionBook2BlockFormat &format)
  : FictionBook2BlockFormatContextBase(parentContext, format)
  , m_model(model)
  , m_opened(false)
{
}

FictionBook2XMLParserContext *FictionBook2TrContext::element(const FictionBook2TokenData &name, const FictionBook2TokenData &ns)
{
  if (FictionBook2Token::NS_FICTIONBOOK == getFictionBook2TokenID(ns))
  {
    switch (getFictionBook2TokenID(name))
    {
    case FictionBook2Token::th :
      if (!m_opened)
        openRow(true);
      return new FictionBook2CellContext(this, m_model, getBlockFormat(), true);
    case FictionBook2Token::td :
      if (!m_opened)
        openRow(false);
      return new FictionBook2CellContext(this, m_model, getBlockFormat(), false);
    default :
      break;
    }
  }

  return new FictionBook2SkipElementContext(this);
}

void FictionBook2TrContext::endOfElement()
{
  if (!m_opened)
    openRow(false);

  getCollector()->closeTableRow();
}

void FictionBook2TrContext::attribute(const FictionBook2TokenData &name, const FictionBook2TokenData *ns, const char *)
{
  if ((FictionBook2_NO_NAMESPACE(ns)) && (FictionBook2Token::align == getFictionBook2TokenID(name)))
  {
    // TODO: use this
  }
}

void FictionBook2TrContext::openRow(const bool header)
{
  getBlockFormat().headerRow = header;

  size_t coveredCells = m_model->addRow();

  getCollector()->openTableRow(getBlockFormat());
  m_opened = true;
  for (; coveredCells > 0; --coveredCells)
    getCollector()->insertCoveredTableCell();
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
