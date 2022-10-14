/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include <librevenge-stream/librevenge-stream.h>

#include "PDBLZ77Stream.h"

using libebook::PDBLZ77Stream;

namespace test
{

class PDBLZ77StreamTest : public CPPUNIT_NS::TestFixture
{
public:
  virtual void setUp();
  virtual void tearDown();

private:
  CPPUNIT_TEST_SUITE(PDBLZ77StreamTest);
  CPPUNIT_TEST(testRead);
  CPPUNIT_TEST_SUITE_END();

private:
  void testRead();
};

void PDBLZ77StreamTest::setUp()
{
}

void PDBLZ77StreamTest::tearDown()
{
}

void PDBLZ77StreamTest::testRead()
{
  const unsigned char unpacked[] = "abc dee abc";
  const unsigned char data[] = "\x61\x62\x01\x63\xe4\x02\x65\x65\x20\x80\x40";
  librevenge::RVNGStringStream dataStream(data, sizeof(data));
  PDBLZ77Stream stream(&dataStream);

  CPPUNIT_ASSERT_MESSAGE("stream is already exhausted before starting to read", !stream.isEnd());

  for (int i = 0; sizeof(unpacked) != i; ++i)
  {
    unsigned long readBytes = 0;
    const unsigned char *s = stream.read(1, readBytes);

    CPPUNIT_ASSERT(1 == readBytes);
    CPPUNIT_ASSERT_EQUAL(unpacked[i], s[0]);
    CPPUNIT_ASSERT(((sizeof(unpacked) - 1) == i) || !stream.isEnd());
  }

  CPPUNIT_ASSERT_MESSAGE("reading did not exhaust the stream", stream.isEnd());
}

CPPUNIT_TEST_SUITE_REGISTRATION(PDBLZ77StreamTest);

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
