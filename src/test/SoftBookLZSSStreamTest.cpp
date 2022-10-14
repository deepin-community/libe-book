/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <string>

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include <librevenge-stream/librevenge-stream.h>

#include "libebook_utils.h"
#include "EBOOKMemoryStream.h"
#include "SoftBookLZSSStream.h"

using std::shared_ptr;

using libebook::EBOOKMemoryStream;
using libebook::SoftBookLZSSStream;

using std::string;

namespace test
{

class SoftBookLZSSStreamTest : public CPPUNIT_NS::TestFixture
{
public:
  virtual void setUp();
  virtual void tearDown();

private:
  CPPUNIT_TEST_SUITE(SoftBookLZSSStreamTest);
  CPPUNIT_TEST(testRead);
  CPPUNIT_TEST(testSeek);
  CPPUNIT_TEST_SUITE_END();

private:
  void testRead();
  void testSeek();

  void testReadAll(const std::string &text, librevenge::RVNGInputStream *stream);
};

namespace
{

static const unsigned OFFSET_BITS = 14;
static const unsigned LENGTH_BITS = 3;

shared_ptr<librevenge::RVNGInputStream> makeLZSSStream(const unsigned char *const data, const unsigned length, const unsigned uncompressedLength)
{
  EBOOKMemoryStream dataStream(data, length);
  SoftBookLZSSStream::Configuration configuration;

  configuration.offsetBits = OFFSET_BITS;
  configuration.lengthBits = LENGTH_BITS;
  configuration.uncompressedLength = uncompressedLength;
  configuration.allowOverflow = false;
  configuration.bigEndian = true;
  configuration.fillPos = 1;

  shared_ptr<librevenge::RVNGInputStream> lzssStream(new SoftBookLZSSStream(&dataStream, configuration));
  return lzssStream;
}

}

void SoftBookLZSSStreamTest::setUp()
{
}

void SoftBookLZSSStreamTest::tearDown()
{
}

void SoftBookLZSSStreamTest::testRead()
{
  {
    const string plain("\013aaaa bbbb aaaaa bb \012");
    const unsigned char encoded[] = {0x85, 0xd8, 0x40, 1, 9, 5, 0x88, 0, 0x38, 0x90, 0, 2, 0x20, 1, 0x4c, 0x82, 0x14, 0, 0};
    shared_ptr<librevenge::RVNGInputStream> stream = makeLZSSStream(encoded, sizeof encoded, unsigned(plain.size()));
    testReadAll(plain, stream.get());
  }
}

void SoftBookLZSSStreamTest::testSeek()
{
}

void SoftBookLZSSStreamTest::testReadAll(const std::string &plain, librevenge::RVNGInputStream *const stream)
{
  CPPUNIT_ASSERT(!stream->isEnd());

  const unsigned char *data = 0;
  CPPUNIT_ASSERT_NO_THROW(data = libebook::readNBytes(stream, plain.size()));
  CPPUNIT_ASSERT(stream->isEnd());
  const string decoded(reinterpret_cast<const char *>(data), plain.size());
  CPPUNIT_ASSERT_EQUAL(plain, decoded);
}

CPPUNIT_TEST_SUITE_REGISTRATION(SoftBookLZSSStreamTest);

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
