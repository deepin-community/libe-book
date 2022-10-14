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

#include "EBOOKBitStream.h"
#include "EBOOKMemoryStream.h"

using libebook::EBOOKBitStream;
using libebook::EBOOKMemoryStream;

namespace test
{

class EBOOKBitStreamTest : public CPPUNIT_NS::TestFixture
{
public:
  virtual void setUp();
  virtual void tearDown();

private:
  CPPUNIT_TEST_SUITE(EBOOKBitStreamTest);
  CPPUNIT_TEST(testRead1);
  CPPUNIT_TEST(testRead3);
  CPPUNIT_TEST(testRead4);
  CPPUNIT_TEST(testRead8);
  CPPUNIT_TEST(testRead16);
  CPPUNIT_TEST(testRead32);
  CPPUNIT_TEST(testReadVaryingSmall);
  CPPUNIT_TEST(testReadVaryingBig);

  CPPUNIT_TEST(testAtLastByte);
  CPPUNIT_TEST_SUITE_END();

private:
  void testRead1();
  void testRead3();
  void testRead4();
  void testRead8();
  void testRead16();
  void testRead32();
  void testReadVaryingSmall();
  void testReadVaryingBig();

  void testRead1(bool bigEndian);
  void testRead3(bool bigEndian);
  void testRead4(bool bigEndian);
  void testRead8(bool bigEndian);
  void testReadVaryingSmall(bool bigEndian);

  void testAtLastByte();
};

namespace
{

static const unsigned char TEST_DATA[] = {0xe3, 0x8e, 0x38, 0xe3};

}

void EBOOKBitStreamTest::setUp()
{
}

void EBOOKBitStreamTest::tearDown()
{
}

void EBOOKBitStreamTest::testRead1()
{
  testRead1(false);
  testRead1(true);
}

void EBOOKBitStreamTest::testRead3()
{
  testRead3(false);
  testRead3(true);
}

void EBOOKBitStreamTest::testRead4()
{
  testRead4(false);
  testRead4(true);
}

void EBOOKBitStreamTest::testRead8()
{
  testRead8(false);
  testRead8(true);
}

void EBOOKBitStreamTest::testRead16()
{
  EBOOKMemoryStream stream(TEST_DATA, sizeof TEST_DATA);

  {
    EBOOKBitStream bitStream(&stream);

    CPPUNIT_ASSERT(!bitStream.isEnd());

    CPPUNIT_ASSERT_EQUAL(0x8ee3u, bitStream.read(16));
    CPPUNIT_ASSERT_EQUAL(0xe338u, bitStream.read(16));

    CPPUNIT_ASSERT(bitStream.isEnd());
  }

  stream.seek(0, librevenge::RVNG_SEEK_SET);

  {
    EBOOKBitStream bitStream(&stream);

    CPPUNIT_ASSERT(!bitStream.isEnd());

    CPPUNIT_ASSERT_EQUAL(0xe38eu, bitStream.read(16, true));
    CPPUNIT_ASSERT_EQUAL(0x38e3u, bitStream.read(16, true));

    CPPUNIT_ASSERT(bitStream.isEnd());
  }
}

void EBOOKBitStreamTest::testRead32()
{
  EBOOKMemoryStream stream(TEST_DATA, sizeof TEST_DATA);
  {
    EBOOKBitStream bitStream(&stream);

    CPPUNIT_ASSERT(!bitStream.isEnd());

    CPPUNIT_ASSERT_EQUAL(0xe3388ee3u, bitStream.read(32));

    CPPUNIT_ASSERT(bitStream.isEnd());
  }

  stream.seek(0, librevenge::RVNG_SEEK_SET);

  {
    EBOOKBitStream bitStream(&stream);

    CPPUNIT_ASSERT(!bitStream.isEnd());

    CPPUNIT_ASSERT_EQUAL(0xe38e38e3u, bitStream.read(32, true));

    CPPUNIT_ASSERT(bitStream.isEnd());
  }
}

void EBOOKBitStreamTest::testReadVaryingSmall()
{
  testReadVaryingSmall(false);
  testReadVaryingSmall(true);
}

void EBOOKBitStreamTest::testReadVaryingBig()
{
  EBOOKMemoryStream stream(TEST_DATA, sizeof TEST_DATA);

  {
    EBOOKBitStream bitStream(&stream);

    CPPUNIT_ASSERT(!bitStream.isEnd());

    CPPUNIT_ASSERT_EQUAL(0x2e3u, bitStream.read(10));
    CPPUNIT_ASSERT_EQUAL(0u, bitStream.read(2));
    CPPUNIT_ASSERT_EQUAL(1u, bitStream.read(1));
    CPPUNIT_ASSERT_EQUAL(0x1c7u, bitStream.read(12));
    CPPUNIT_ASSERT_EQUAL(0x31u, bitStream.read(6));
    CPPUNIT_ASSERT_EQUAL(1u, bitStream.read(1));

    CPPUNIT_ASSERT(bitStream.isEnd());
  }

  stream.seek(0, librevenge::RVNG_SEEK_SET);

  {
    EBOOKBitStream bitStream(&stream);

    CPPUNIT_ASSERT(!bitStream.isEnd());

    CPPUNIT_ASSERT_EQUAL(0x38eu, bitStream.read(10, true));
    CPPUNIT_ASSERT_EQUAL(0u, bitStream.read(2, true));
    CPPUNIT_ASSERT_EQUAL(1u, bitStream.read(1, true));
    CPPUNIT_ASSERT_EQUAL(0xc71u, bitStream.read(12, true));
    CPPUNIT_ASSERT_EQUAL(0x31u, bitStream.read(6, true));
    CPPUNIT_ASSERT_EQUAL(1u, bitStream.read(1, true));

    CPPUNIT_ASSERT(bitStream.isEnd());
  }
}

void EBOOKBitStreamTest::testRead1(const bool bigEndian)
{
  EBOOKMemoryStream stream(TEST_DATA, sizeof TEST_DATA);
  EBOOKBitStream bitStream(&stream);

  CPPUNIT_ASSERT(!bitStream.isEnd());

  CPPUNIT_ASSERT_EQUAL(1u, bitStream.read(1, bigEndian));
  CPPUNIT_ASSERT_EQUAL(1u, bitStream.read(1, bigEndian));
  CPPUNIT_ASSERT_EQUAL(1u, bitStream.read(1, bigEndian));
  CPPUNIT_ASSERT_EQUAL(0u, bitStream.read(1, bigEndian));
  CPPUNIT_ASSERT_EQUAL(0u, bitStream.read(1, bigEndian));
  CPPUNIT_ASSERT_EQUAL(0u, bitStream.read(1, bigEndian));
  CPPUNIT_ASSERT_EQUAL(1u, bitStream.read(1, bigEndian));
  CPPUNIT_ASSERT_EQUAL(1u, bitStream.read(1, bigEndian));
  CPPUNIT_ASSERT_EQUAL(1u, bitStream.read(1, bigEndian));
  CPPUNIT_ASSERT_EQUAL(0u, bitStream.read(1, bigEndian));
  CPPUNIT_ASSERT_EQUAL(0u, bitStream.read(1, bigEndian));
  CPPUNIT_ASSERT_EQUAL(0u, bitStream.read(1, bigEndian));
  CPPUNIT_ASSERT_EQUAL(1u, bitStream.read(1, bigEndian));
  CPPUNIT_ASSERT_EQUAL(1u, bitStream.read(1, bigEndian));
  CPPUNIT_ASSERT_EQUAL(1u, bitStream.read(1, bigEndian));
  CPPUNIT_ASSERT_EQUAL(0u, bitStream.read(1, bigEndian));
  CPPUNIT_ASSERT_EQUAL(0u, bitStream.read(1, bigEndian));
  CPPUNIT_ASSERT_EQUAL(0u, bitStream.read(1, bigEndian));
  CPPUNIT_ASSERT_EQUAL(1u, bitStream.read(1, bigEndian));
  CPPUNIT_ASSERT_EQUAL(1u, bitStream.read(1, bigEndian));
  CPPUNIT_ASSERT_EQUAL(1u, bitStream.read(1, bigEndian));
  CPPUNIT_ASSERT_EQUAL(0u, bitStream.read(1, bigEndian));
  CPPUNIT_ASSERT_EQUAL(0u, bitStream.read(1, bigEndian));
  CPPUNIT_ASSERT_EQUAL(0u, bitStream.read(1, bigEndian));
  CPPUNIT_ASSERT_EQUAL(1u, bitStream.read(1, bigEndian));
  CPPUNIT_ASSERT_EQUAL(1u, bitStream.read(1, bigEndian));
  CPPUNIT_ASSERT_EQUAL(1u, bitStream.read(1, bigEndian));
  CPPUNIT_ASSERT_EQUAL(0u, bitStream.read(1, bigEndian));
  CPPUNIT_ASSERT_EQUAL(0u, bitStream.read(1, bigEndian));
  CPPUNIT_ASSERT_EQUAL(0u, bitStream.read(1, bigEndian));
  CPPUNIT_ASSERT_EQUAL(1u, bitStream.read(1, bigEndian));
  CPPUNIT_ASSERT_EQUAL(1u, bitStream.read(1, bigEndian));

  CPPUNIT_ASSERT(bitStream.isEnd());
}

void EBOOKBitStreamTest::testRead3(const bool bigEndian)
{
  EBOOKMemoryStream stream(TEST_DATA, sizeof TEST_DATA);
  EBOOKBitStream bitStream(&stream);

  CPPUNIT_ASSERT(!bitStream.isEnd());

  CPPUNIT_ASSERT_EQUAL(7u, bitStream.read(3, bigEndian));
  CPPUNIT_ASSERT_EQUAL(0u, bitStream.read(3, bigEndian));
  CPPUNIT_ASSERT_EQUAL(7u, bitStream.read(3, bigEndian));
  CPPUNIT_ASSERT_EQUAL(0u, bitStream.read(3, bigEndian));
  CPPUNIT_ASSERT_EQUAL(7u, bitStream.read(3, bigEndian));
  CPPUNIT_ASSERT_EQUAL(0u, bitStream.read(3, bigEndian));
  CPPUNIT_ASSERT_EQUAL(7u, bitStream.read(3, bigEndian));
  CPPUNIT_ASSERT_EQUAL(0u, bitStream.read(3, bigEndian));
  CPPUNIT_ASSERT_EQUAL(7u, bitStream.read(3, bigEndian));
  CPPUNIT_ASSERT_EQUAL(0u, bitStream.read(3, bigEndian));
  // read the remaining bits
  CPPUNIT_ASSERT_EQUAL(3u, bitStream.read(2, bigEndian));

  CPPUNIT_ASSERT(bitStream.isEnd());
}

void EBOOKBitStreamTest::testRead4(const bool bigEndian)
{
  EBOOKMemoryStream stream(TEST_DATA, sizeof TEST_DATA);
  EBOOKBitStream bitStream(&stream);

  CPPUNIT_ASSERT(!bitStream.isEnd());

  CPPUNIT_ASSERT_EQUAL(0xeu, bitStream.read(4, bigEndian));
  CPPUNIT_ASSERT_EQUAL(0x3u, bitStream.read(4, bigEndian));
  CPPUNIT_ASSERT_EQUAL(0x8u, bitStream.read(4, bigEndian));
  CPPUNIT_ASSERT_EQUAL(0xeu, bitStream.read(4, bigEndian));
  CPPUNIT_ASSERT_EQUAL(0x3u, bitStream.read(4, bigEndian));
  CPPUNIT_ASSERT_EQUAL(0x8u, bitStream.read(4, bigEndian));
  CPPUNIT_ASSERT_EQUAL(0xeu, bitStream.read(4, bigEndian));
  CPPUNIT_ASSERT_EQUAL(0x3u, bitStream.read(4, bigEndian));

  CPPUNIT_ASSERT(bitStream.isEnd());
}

void EBOOKBitStreamTest::testRead8(const bool bigEndian)
{
  EBOOKMemoryStream stream(TEST_DATA, sizeof TEST_DATA);
  EBOOKBitStream bitStream(&stream);

  CPPUNIT_ASSERT(!bitStream.isEnd());

  CPPUNIT_ASSERT_EQUAL(0xe3u, bitStream.read(8, bigEndian));
  CPPUNIT_ASSERT_EQUAL(0x8eu, bitStream.read(8, bigEndian));
  CPPUNIT_ASSERT_EQUAL(0x38u, bitStream.read(8, bigEndian));
  CPPUNIT_ASSERT_EQUAL(0xe3u, bitStream.read(8, bigEndian));

  CPPUNIT_ASSERT(bitStream.isEnd());
}

void EBOOKBitStreamTest::testReadVaryingSmall(const bool bigEndian)
{
  EBOOKMemoryStream stream(TEST_DATA, sizeof TEST_DATA);
  EBOOKBitStream bitStream(&stream);

  CPPUNIT_ASSERT(!bitStream.isEnd());

  CPPUNIT_ASSERT_EQUAL(1u, bitStream.read(1, bigEndian));
  CPPUNIT_ASSERT_EQUAL(3u, bitStream.read(2, bigEndian));
  CPPUNIT_ASSERT_EQUAL(0u, bitStream.read(3, bigEndian));
  CPPUNIT_ASSERT_EQUAL(0xeu, bitStream.read(4, bigEndian));
  CPPUNIT_ASSERT_EQUAL(7u, bitStream.read(5, bigEndian));
  CPPUNIT_ASSERT_EQUAL(7u, bitStream.read(6, bigEndian));
  CPPUNIT_ASSERT_EQUAL(0xeu, bitStream.read(7, bigEndian));
  // read the remaining bits
  CPPUNIT_ASSERT_EQUAL(3u, bitStream.read(4, bigEndian));

  CPPUNIT_ASSERT(bitStream.isEnd());
}


void EBOOKBitStreamTest::testAtLastByte()
{
  EBOOKMemoryStream stream(TEST_DATA, sizeof TEST_DATA);
  EBOOKBitStream bitStream(&stream);

  CPPUNIT_ASSERT(!bitStream.atLastByte());
  bitStream.read(23);
  CPPUNIT_ASSERT(!bitStream.atLastByte());
  bitStream.read(1);
  CPPUNIT_ASSERT(bitStream.atLastByte());
  CPPUNIT_ASSERT(!bitStream.isEnd());
  bitStream.read(1);
  CPPUNIT_ASSERT(bitStream.atLastByte());
  CPPUNIT_ASSERT(!bitStream.isEnd());
  bitStream.read(7);
  CPPUNIT_ASSERT(bitStream.atLastByte());
  CPPUNIT_ASSERT(bitStream.isEnd());
}

CPPUNIT_TEST_SUITE_REGISTRATION(EBOOKBitStreamTest);

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
