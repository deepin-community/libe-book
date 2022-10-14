/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef LIBEBOOK_UTILS_H_INCLUDED
#define LIBEBOOK_UTILS_H_INCLUDED

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <memory>
#include <string>

#include <boost/cstdint.hpp>

#include <librevenge-stream/librevenge-stream.h>
#include <librevenge/librevenge.h>

// do nothing with debug messages in a release compile
#ifdef DEBUG

#if defined(HAVE_FUNC_ATTRIBUTE_FORMAT)
#define EBOOK_ATTRIBUTE_PRINTF(fmt, arg) __attribute__((format(printf, fmt, arg)))
#else
#define EBOOK_ATTRIBUTE_PRINTF(fmt, arg)
#endif

namespace libebook
{
void debugPrint(const char *format, ...) EBOOK_ATTRIBUTE_PRINTF(1, 2);
}

#define EBOOK_DEBUG_MSG(M) libebook::debugPrint M
#define EBOOK_DEBUG(M) M

#else

#define EBOOK_DEBUG_MSG(M)
#define EBOOK_DEBUG(M)

#endif

#define EBOOK_NUM_ELEMENTS(array) sizeof(array)/sizeof(array[0])

namespace libebook
{

typedef std::shared_ptr<librevenge::RVNGInputStream> RVNGInputStreamPtr_t;

struct EBOOKDummyDeleter
{
  void operator()(void *) {}
};

uint8_t readU8(librevenge::RVNGInputStream *input, bool = false);
uint16_t readU16(librevenge::RVNGInputStream *input, bool bigEndian=false);
uint32_t readU32(librevenge::RVNGInputStream *input, bool bigEndian=false);
uint64_t readU64(librevenge::RVNGInputStream *input, bool bigEndian=false);

const unsigned char *readNBytes(librevenge::RVNGInputStream *input, unsigned long numBytes);

std::string readCString(librevenge::RVNGInputStream *input);
std::string readPascalString(librevenge::RVNGInputStream *input);

void skip(librevenge::RVNGInputStream *input, unsigned long numBytes);

void seek(librevenge::RVNGInputStream *input, unsigned long pos);
void seekRelative(librevenge::RVNGInputStream *input, long pos);

unsigned long getRemainingLength(librevenge::RVNGInputStream *input);

uint8_t readU8(std::shared_ptr<librevenge::RVNGInputStream> input, bool = false);
uint16_t readU16(std::shared_ptr<librevenge::RVNGInputStream> input, bool bigEndian=false);
uint32_t readU32(std::shared_ptr<librevenge::RVNGInputStream> input, bool bigEndian=false);
uint64_t readU64(std::shared_ptr<librevenge::RVNGInputStream> input, bool bigEndian=false);

const unsigned char *readNBytes(std::shared_ptr<librevenge::RVNGInputStream> input, unsigned long numBytes);

std::string readCString(std::shared_ptr<librevenge::RVNGInputStream> input);
std::string readPascalString(std::shared_ptr<librevenge::RVNGInputStream> input);

void skip(std::shared_ptr<librevenge::RVNGInputStream> input, unsigned long numBytes);

void seek(std::shared_ptr<librevenge::RVNGInputStream> input, unsigned long pos);
void seekRelative(std::shared_ptr<librevenge::RVNGInputStream> input, long pos);

unsigned long getRemainingLength(std::shared_ptr<librevenge::RVNGInputStream> input);

bool findSubStreamByExt(const RVNGInputStreamPtr_t &input, const std::string &ext, unsigned &id);

librevenge::RVNGPropertyList getDefaultPageSpanPropList();

class EndOfStreamException
{
public:
  EndOfStreamException();
};

class GenericException
{
};

// parser exceptions

class FileAccessError
{
};

class PackageError
{
};

class ParseError
{
};

class PasswordMismatch
{
};

class UnsupportedEncryption
{
};

class UnsupportedFormat
{
};

} // namespace libebook

#endif // LIBEBOOK_UTILS_H_INCLUDED
/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
