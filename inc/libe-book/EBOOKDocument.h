/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef LIBE_BOOK_EBOOKDOCUMENT_H_INCLUDED
#define LIBE_BOOK_EBOOKDOCUMENT_H_INCLUDED

#include <librevenge/librevenge.h>
#include <librevenge-stream/librevenge-stream.h>

#ifdef DLL_EXPORT
#ifdef LIBE_BOOK_BUILD
#define EBOOKAPI __declspec(dllexport)
#else
#define EBOOKAPI __declspec(dllimport)
#endif
#else // !DLL_EXPORT
#ifdef LIBE_BOOK_VISIBILITY
#define EBOOKAPI __attribute__((visibility("default")))
#else
#define EBOOKAPI
#endif
#endif

namespace libebook
{

class EBOOKDocument
{
public:
  /** Likelihood that the file format is supported.
    */
  enum Confidence
  {
    CONFIDENCE_NONE, //< not supported
    CONFIDENCE_WEAK, //< maybe supported
    CONFIDENCE_UNSUPPORTED_ENCRYPTION, //< the format is supported, but the used encryption method is not
    CONFIDENCE_SUPPORTED_ENCRYPTION, //< the format is supported, but encrypted
    CONFIDENCE_SUPPORTED_PART, //< the file is only a part of a supported structured format
    CONFIDENCE_EXCELLENT //< supported
  };

  /** Result of parsing the file.
    */
  enum Result
  {
    RESULT_OK, //< parsed without any problem
    RESULT_FILE_ACCESS_ERROR, //< problem when accessing the file
    RESULT_PACKAGE_ERROR, //< problem with parsing structured file's content
    RESULT_PARSE_ERROR, //< problem when parsing the file
    RESULT_PASSWORD_MISMATCH, //< problem with given password
    RESULT_UNSUPPORTED_ENCRYPTION, //< unsupported encryption
    RESULT_UNSUPPORTED_FORMAT, //< unsupported file format
    RESULT_UNKNOWN_ERROR //< an unspecified error
  };

  /** Type of document.
    */
  enum Type
  {
    TYPE_UNKNOWN, //< unrecognized file
    TYPE_BBEB, //< BroadBand eBook
    TYPE_EPUB, //< ePub (v.2)
    TYPE_FICTIONBOOK2, //< FictionBook v.2
    TYPE_HIEBOOK, //< Hiebook eBook (KML)
    TYPE_HTML, //< HTML 1-5 (basic support)
    TYPE_HTMLHELP, //< MS HTML Help (.chm)
    TYPE_KINDLE8, //< Amazon Kindle Format 8
    TYPE_LIT, //< MS LIT
    TYPE_MOBIPOCKET, //< MobiPocket
    TYPE_OPENEBOOK, //< Open eBook (OEB)
    TYPE_PALMDOC, //< PalmDoc (Aportis Doc, Palm)
    TYPE_PEANUTPRESS, //< PeanutPress (eReader, Palm)
    TYPE_PLUCKER, //< Plucker (Palm)
    TYPE_ROCKETEBOOK, //< Rocket eBook
    TYPE_QIOO, //< QiOO (mobile)
    TYPE_SOFTBOOK, //< SoftBook
    TYPE_TCR, //< TCR
    TYPE_TEALDOC, //< TealDoc (Palm)
    TYPE_TEBR, //< TeBR
    TYPE_TOMERAIDER, //< Tome Raider
    TYPE_TOMERAIDER3, //< Tome Raider v. 3
    TYPE_XHTML, //< XHTML 1.0 (basic support)
    TYPE_ZTXT, //< zTXT (Palm)
    TYPE_ZVR, //< ZVR

    TYPE_RESERVED1, //< reserved for future use
    TYPE_RESERVED2, //< reserved for future use
    TYPE_RESERVED3, //< reserved for future use
    TYPE_RESERVED4, //< reserved for future use
    TYPE_RESERVED5, //< reserved for future use
    TYPE_RESERVED6, //< reserved for future use
    TYPE_RESERVED7, //< reserved for future use
    TYPE_RESERVED8, //< reserved for future use
    TYPE_RESERVED9 //< reserved for future use
  };

  static EBOOKAPI Confidence isSupported(librevenge::RVNGInputStream *input, Type *type = nullptr);
  static EBOOKAPI Result parse(librevenge::RVNGInputStream *input, librevenge::RVNGTextInterface *document, const char *password = nullptr);
  static EBOOKAPI Result parse(librevenge::RVNGInputStream *input, librevenge::RVNGTextInterface *document, Type type, const char *password = nullptr);
};

} // namespace libebook

#endif // LIBE_BOOK_EBOOKDOCUMENT_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
