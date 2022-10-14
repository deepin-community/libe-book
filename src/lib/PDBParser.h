/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef PDBPARSER_H_INCLUDED
#define PDBPARSER_H_INCLUDED

#include <memory>

#include <librevenge/librevenge.h>
#include <librevenge-stream/librevenge-stream.h>

#define PDB_CODE(s) ((s[0] << 24) | (s[1] << 16) | (s[2] << 8) | s[3])

namespace libebook
{

struct PDBParserImpl;

class PDBParser
{
// disable copying
  PDBParser(const PDBParser &other);
  PDBParser &operator=(const PDBParser &other);

public:
  virtual ~PDBParser() = 0;

  /** Parse input and produce output to @e document.
    *
    * @return true if the input has been parsed successfully.
    */
  bool parse();

protected:
  /** Instantiate a parser for a document in Palm Database Format.
    *
    * @arg[in] input input stream
    * @arg[in] document output document generator
    * @arg[in] type the document type
    * @arg[in] creator the document creator
    */
  PDBParser(librevenge::RVNGInputStream *input, librevenge::RVNGTextInterface *document,
            unsigned type, unsigned creator);

  /** Get the document generator used for this parsing run.
    *
    * @return the output document generator
    */
  librevenge::RVNGTextInterface *getDocument() const;

  const char *getName() const;

  /** Return a stream for the format's appInfo record, if it is present.
    *
    * A parser for a subformat might want to peek into the appInfo
    * record in isSupported().
    *
    * @return a newly allocated stream spanning the appInfo record or 0,
    *         if there is no appInfo record
    */
  librevenge::RVNGInputStream *getAppInfoRecord() const;

  /** Return a stream for the format's index record.
    *
    * A parser for a subformat might want to peek into the index record
    * in isSupported().
    *
    * @return a newly allocated stream spanning the index record
    */
  librevenge::RVNGInputStream *getIndexRecord() const;

  /** Return the number of data records in the document.
    *
    * @return the number of data records
    */
  unsigned getDataRecordCount() const;

  /** Return a stream for the n-th data record.
    *
    * @arg[in] n the stream number, 0-based
    * @return a newly allocated stream spanning one data record
    */
  librevenge::RVNGInputStream *getDataRecord(unsigned n) const;

  /** Return a stream for all data records.
    *
    * @return a newly allocated stream spanning all data records
    */
  librevenge::RVNGInputStream *getDataRecords() const;

  /** Return a stream for data records in a given range.
    *
    * @arg[in] first the first data record to include
    * @arg[in] last the first data record not to include
    *
    * @return a newly allocated stream spanning data records in a given range
    */
  librevenge::RVNGInputStream *getDataRecords(unsigned first, unsigned last) const;

private:
  virtual void readAppInfoRecord(librevenge::RVNGInputStream *record) = 0;
  virtual void readSortInfoRecord(librevenge::RVNGInputStream *record) = 0;
  virtual void readIndexRecord(librevenge::RVNGInputStream *record) = 0;
  virtual void readDataRecord(librevenge::RVNGInputStream *record, bool last = false) = 0;

  virtual void readDataRecords();

  void readHeader();

  librevenge::RVNGInputStream *getRecordStream(unsigned n) const;

private:
  std::unique_ptr<PDBParserImpl> m_impl;
};

}

#endif // PDBPARSER_H_INCLUDED
/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
