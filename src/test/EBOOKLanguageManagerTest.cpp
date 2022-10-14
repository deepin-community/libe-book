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

#include "EBOOKLanguageManager.h"

namespace test
{

using libebook::EBOOKLanguageManager;

using librevenge::RVNGPropertyList;
using librevenge::RVNGString;

using std::string;

namespace
{

void assertProperty(const string &msg, const RVNGPropertyList &props, const char *const name, const char *const expected)
{
  CPPUNIT_ASSERT_MESSAGE(msg + ": checking existence of property: " + name, props[name]);
  CPPUNIT_ASSERT_EQUAL_MESSAGE(msg + ": checking value of property: " + name,
                               string(expected), string(props[name]->getStr().cstr()));
}

}

class EBOOKLanguageManagerTest : public CPPUNIT_NS::TestFixture
{
public:
  virtual void setUp();
  virtual void tearDown();

private:
  CPPUNIT_TEST_SUITE(EBOOKLanguageManagerTest);
  CPPUNIT_TEST(testTagToProps);
  CPPUNIT_TEST(testLanguageToProps);
  CPPUNIT_TEST_SUITE_END();

private:
  void testTagToProps();
  void testLanguageToProps();
};

void EBOOKLanguageManagerTest::setUp()
{
}

void EBOOKLanguageManagerTest::tearDown()
{
}

void EBOOKLanguageManagerTest::testTagToProps()
{
  EBOOKLanguageManager mgr;

  {
    const string tag(mgr.addTag("cs-Latn-CZ"));
    CPPUNIT_ASSERT(!tag.empty());
    RVNGPropertyList props;
    mgr.writeProperties(tag, props);
    assertProperty("lang+country+script", props, "fo:language", "cs");
    assertProperty("lang+country+script", props, "fo:country", "CZ");
    assertProperty("lang+country+script", props, "fo:script", "Latn");
  }

  {
    const string tag(mgr.addTag("cs-CZ"));
    CPPUNIT_ASSERT(!tag.empty());
    RVNGPropertyList props;
    mgr.writeProperties(tag, props);
    assertProperty("lang+country", props, "fo:language", "cs");
    assertProperty("lang+country", props, "fo:country", "CZ");
    assertProperty("lang+country", props, "fo:script", "Latn");
  }

  {
    const string tag(mgr.addTag("cs"));
    CPPUNIT_ASSERT(!tag.empty());
    RVNGPropertyList props;
    mgr.writeProperties(tag, props);
    assertProperty("lang only", props, "fo:language", "cs");
    assertProperty("lang only", props, "fo:country", "CZ");
    assertProperty("lang only", props, "fo:script", "Latn");
  }

  {
    const string tag(mgr.addTag("cs-Cyrl-CZ"));
    CPPUNIT_ASSERT(!tag.empty());
    RVNGPropertyList props;
    mgr.writeProperties(tag, props);
    assertProperty("lang+country+diff. script", props, "fo:language", "cs");
    assertProperty("lang+country+diff. script", props, "fo:country", "CZ");
    assertProperty("lang+country+diff. script", props, "fo:script", "Cyrl");
  }

  {
    // invalid tag
    const string tag(mgr.addTag("13c"));
    CPPUNIT_ASSERT(tag.empty());
  }
}

void EBOOKLanguageManagerTest::testLanguageToProps()
{
  EBOOKLanguageManager mgr;

  {
    const string tag(mgr.addLanguage("Czech"));
    CPPUNIT_ASSERT(!tag.empty());
    RVNGPropertyList props;
    mgr.writeProperties(tag, props);
    assertProperty("existing language", props, "fo:language", "cs");
    assertProperty("existing language", props, "fo:country", "CZ");
    assertProperty("existing language", props, "fo:script", "Latn");
  }

  {
    const string tag(mgr.addLanguage("Czechoslovak"));
    CPPUNIT_ASSERT(tag.empty());
  }
}

CPPUNIT_TEST_SUITE_REGISTRATION(EBOOKLanguageManagerTest);

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
