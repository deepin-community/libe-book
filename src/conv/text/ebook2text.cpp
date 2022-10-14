/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <memory>
#include <stdio.h>
#include <string.h>

#include <librevenge-generators/librevenge-generators.h>
#include <librevenge-stream/librevenge-stream.h>

#include <libe-book/libe-book.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifndef VERSION
#define VERSION "UNKNOWN VERSION"
#endif

namespace
{

int printUsage()
{
  printf("`ebook2text' converts e-books to plain text.\n");
  printf("Supported formats include FictionBook 2, BBeB and various Palm-based formats.\n");
  printf("\n");
  printf("Usage: ebook2text [OPTION] FILE\n");
  printf("\n");
  printf("Options:\n");
  printf("\t--info                display document metadata instead of the text\n");
  printf("\t--help                show this help message\n");
  printf("\t--version             show version information\n");
  printf("\n");
  printf("Report bugs to <https://sourceforge.net/p/libebook/tickets/> or <https://bugs.documentfoundation.org/>.\n");
  return -1;
}

int printVersion()
{
  printf("ebook2text %s\n", VERSION);
  return 0;
}

} // anonymous namespace

using libebook::EBOOKDocument;

int main(int argc, char *argv[])
{
  if (argc < 2)
    return printUsage();

  char *szInputFile = nullptr;
  bool isInfo = false;

  for (int i = 1; i < argc; i++)
  {
    if (!strcmp(argv[i], "--info"))
      isInfo = true;
    else if (!strcmp(argv[i], "--version"))
      return printVersion();
    else if (!szInputFile && strncmp(argv[i], "--", 2))
      szInputFile = argv[i];
    else
      return printUsage();
  }

  if (!szInputFile)
    return printUsage();

  std::shared_ptr<librevenge::RVNGInputStream> input;

  if (librevenge::RVNGDirectoryStream::isDirectory(szInputFile))
    input.reset(new librevenge::RVNGDirectoryStream(szInputFile));
  else
    input.reset(new librevenge::RVNGFileStream(szInputFile));

  EBOOKDocument::Type type = EBOOKDocument::TYPE_UNKNOWN;
  EBOOKDocument::Confidence confidence = EBOOKDocument::isSupported(input.get(), &type);

  if (EBOOKDocument::CONFIDENCE_SUPPORTED_PART == confidence)
  {
    input.reset(librevenge::RVNGDirectoryStream::createForParent(szInputFile));
    confidence = EBOOKDocument::isSupported(input.get(), &type);
  }

  if ((EBOOKDocument::CONFIDENCE_EXCELLENT != confidence) && (EBOOKDocument::CONFIDENCE_WEAK != confidence))
  {
    fprintf(stderr, "'%s' has not been recognized as any of the supported formats\n", szInputFile);
    return 1;
  }

  librevenge::RVNGString document;
  librevenge::RVNGTextTextGenerator documentGenerator(document, isInfo);

  if (EBOOKDocument::RESULT_OK != EBOOKDocument::parse(input.get(), &documentGenerator, type))
    return 1;

  printf("%s", document.cstr());

  return 0;
}

/* vim:set shiftwidth=4 softtabstop=4 noexpandtab: */
