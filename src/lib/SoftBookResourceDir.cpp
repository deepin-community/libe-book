/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <cassert>
#include <string>
#include <unordered_map>

#include <boost/optional.hpp>

#include "libebook_utils.h"
#include "EBOOKMemoryStream.h"
#include "SoftBookHeader.h"
#include "SoftBookResourceDir.h"

using boost::optional;
using std::shared_ptr;

using std::string;

namespace libebook
{

namespace
{

template<class Selector>
class ResourceStream : public librevenge::RVNGInputStream
{
public:
  ResourceStream(shared_ptr<librevenge::RVNGInputStream> strm, shared_ptr<SoftBookResourceDirImpl> resourceDir);

  bool isStructured() override;
  unsigned subStreamCount() override;
  const char *subStreamName(unsigned id) override;
  bool existsSubStream(const char *name) override;
  librevenge::RVNGInputStream *getSubStreamByName(const char *name) override;
  RVNGInputStream *getSubStreamById(unsigned id) override;

  const unsigned char *read(unsigned long numBytes, unsigned long &numBytesRead) override;
  int seek(long offset, librevenge::RVNG_SEEK_TYPE seekType) override;
  long tell() override;
  bool isEnd() override;

private:
  const shared_ptr<librevenge::RVNGInputStream> m_stream;
  const shared_ptr<SoftBookResourceDirImpl> m_resourceDir;
};

struct NameSelector
{
  static librevenge::RVNGInputStream *getStream(shared_ptr<SoftBookResourceDirImpl> resourceDir, const char *name);
};

struct TypeSelector
{
  static librevenge::RVNGInputStream *getStream(shared_ptr<SoftBookResourceDirImpl> resourceDir, const char *name);
};

}

class SoftBookResourceDirImpl
{
  // -Weffc++
  SoftBookResourceDirImpl(const SoftBookResourceDirImpl &other);
  SoftBookResourceDirImpl &operator=(const SoftBookResourceDirImpl &other);

  struct ResourceInfo
  {
    ResourceInfo();

    unsigned offset;
    unsigned length;
    optional<string> type;
  };

  typedef std::unordered_map<string, ResourceInfo> ResourceMap_t;
  typedef std::unordered_map<string, ResourceMap_t::const_iterator> TypeMap_t;

public:
  SoftBookResourceDirImpl(librevenge::RVNGInputStream *input, unsigned files, unsigned version);

  librevenge::RVNGInputStream *getDirStream() const;

  librevenge::RVNGInputStream *getResourceByName(const char *name) const;
  librevenge::RVNGInputStream *getResourceByType(const char *type) const;

private:
  ResourceMap_t::const_iterator findResourceByType(const char *type) const;

  librevenge::RVNGInputStream *createStream(const ResourceInfo &info) const;

private:
  librevenge::RVNGInputStream *m_stream;
  unsigned m_start;
  unsigned m_length;
  mutable ResourceMap_t m_resourceMap;
  mutable TypeMap_t m_typeMap;
};

namespace
{

string readFileType(librevenge::RVNGInputStream *const input)
{
  const unsigned char *const data = readNBytes(input, 4);
  const string fileName(data, data + ((0 == data[3]) ? 3 : 4));
  return fileName;
}

librevenge::RVNGInputStream *NameSelector::getStream(const shared_ptr<SoftBookResourceDirImpl> resourceDir, const char *const name)
{
  return resourceDir->getResourceByName(name);
}

librevenge::RVNGInputStream *TypeSelector::getStream(const shared_ptr<SoftBookResourceDirImpl> resourceDir, const char *const name)
{
  return resourceDir->getResourceByType(name);
}

template<class Selector>
ResourceStream<Selector>::ResourceStream(const shared_ptr<librevenge::RVNGInputStream> strm, const shared_ptr<SoftBookResourceDirImpl> resourceDir)
  : m_stream(strm)
  , m_resourceDir(resourceDir)
{
}

template<class Selector>
bool ResourceStream<Selector>::isStructured()
{
  return true;
}

template<class Selector>
unsigned ResourceStream<Selector>::subStreamCount()
{
  // TODO: implement me
  return 0;
}

template<class Selector>
const char *ResourceStream<Selector>::subStreamName(const unsigned id)
{
  // TODO: implement me
  (void) id;
  return nullptr;
}

template<class Selector>
bool ResourceStream<Selector>::existsSubStream(const char *const name)
{
  // TODO: implement me
  (void) name;
  return true;
}

template<class Selector>
librevenge::RVNGInputStream *ResourceStream<Selector>::getSubStreamByName(const char *const name)
{
  return Selector::getStream(m_resourceDir, name);
}

template<class Selector>
librevenge::RVNGInputStream *ResourceStream<Selector>::getSubStreamById(const unsigned id)
{
  // TODO: implement me
  (void) id;
  return nullptr;
}

template<class Selector>
const unsigned char *ResourceStream<Selector>::read(const unsigned long numBytes, unsigned long &numBytesRead)
{
  return m_stream->read(numBytes, numBytesRead);
}

template<class Selector>
int ResourceStream<Selector>::seek(const long offset, const librevenge::RVNG_SEEK_TYPE seekType)
{
  return m_stream->seek(offset, seekType);
}

template<class Selector>
long ResourceStream<Selector>::tell()
{
  return m_stream->tell();
}

template<class Selector>
bool ResourceStream<Selector>::isEnd()
{
  return m_stream->isEnd();
}

}

SoftBookResourceDir::SoftBookResourceDir(librevenge::RVNGInputStream *const input, const SoftBookHeader &header)
  : m_impl()
{
  input->seek((long) header.getTOCOffset(), librevenge::RVNG_SEEK_SET);
  m_impl.reset(new SoftBookResourceDirImpl(input, header.getFileCount(), header.getVersion()));
}

shared_ptr<librevenge::RVNGInputStream> SoftBookResourceDir::getNameStream() const
{
  const shared_ptr<librevenge::RVNGInputStream> strm(m_impl->getDirStream());
  const shared_ptr<librevenge::RVNGInputStream> resource(new ResourceStream<NameSelector>(strm, m_impl));
  return resource;
}

shared_ptr<librevenge::RVNGInputStream> SoftBookResourceDir::getTypeStream() const
{
  const shared_ptr<librevenge::RVNGInputStream> strm(m_impl->getDirStream());
  const shared_ptr<librevenge::RVNGInputStream> resource(new ResourceStream<TypeSelector>(strm, m_impl));
  return resource;
}

SoftBookResourceDirImpl::ResourceInfo::ResourceInfo()
  : offset(0)
  , length(0)
  , type()
{
}

SoftBookResourceDirImpl::SoftBookResourceDirImpl(librevenge::RVNGInputStream *const input, const unsigned files, const unsigned version)
  : m_stream(input)
  , m_start(0)
  , m_length(0)
  , m_resourceMap()
  , m_typeMap()
{
  m_start = static_cast<unsigned>(input->tell());
  unsigned headerLength = 0;

  switch (version)
  {
  case 1 :
    headerLength = 10;
    break;
  case 2 :
    headerLength = 20;
    break;
  default :
    EBOOK_DEBUG_MSG(("unknown version %d\n", version));
    throw GenericException();
  }

  const unsigned tocLength = headerLength * files;
  unsigned fileOffset = m_start + tocLength;
  m_length = tocLength;

  for (unsigned i = 0; i != files; ++i)
  {
    const string fileName = readFileType(input);
    ResourceInfo info;

    switch (version)
    {
    case 1 :
      skip(input, 2);
      info.length = readU32(input, true);
      break;
    case 2 :
      skip(input, 4);
      info.length = readU32(input, true);
      info.type = readFileType(input);
      skip(input, 4);
      break;
    default :
      throw GenericException();
    }

    info.length += headerLength;
    info.offset = fileOffset;
    fileOffset += info.length;
    m_length += info.length;

    const ResourceMap_t::const_iterator it = m_resourceMap.insert(ResourceMap_t::value_type(fileName, info)).first;
    if (info.type)
      m_typeMap.insert(TypeMap_t::value_type(get(info.type), it));
  }
}

librevenge::RVNGInputStream *SoftBookResourceDirImpl::getDirStream() const
{
  m_stream->seek((long) m_start, librevenge::RVNG_SEEK_SET);
  const unsigned char *const data = readNBytes(m_stream, m_length);
  return new EBOOKMemoryStream(data, m_length);
}

librevenge::RVNGInputStream *SoftBookResourceDirImpl::getResourceByName(const char *const name) const
{
  librevenge::RVNGInputStream *resource = nullptr;

  ResourceMap_t::const_iterator it = m_resourceMap.find(name);
  if (m_resourceMap.end() != it)
    resource = createStream(it->second);

  return resource;
}

librevenge::RVNGInputStream *SoftBookResourceDirImpl::getResourceByType(const char *const type) const
{
  TypeMap_t::const_iterator it = m_typeMap.find(type);
  if (m_typeMap.end() == it)
  {
    const ResourceMap_t::const_iterator resIt = findResourceByType(type);
    it = m_typeMap.insert(TypeMap_t::value_type(type, resIt)).first;
  }
  assert(m_typeMap.end() != it);

  librevenge::RVNGInputStream *resource = nullptr;
  if (m_resourceMap.end() != it->second)
    resource = createStream(it->second->second);

  return resource;
}

SoftBookResourceDirImpl::ResourceMap_t::const_iterator SoftBookResourceDirImpl::findResourceByType(const char *const type) const
{
  auto it = m_resourceMap.begin();

  for (; m_resourceMap.end() != it; ++it)
  {
    ResourceInfo &info = it->second;
    if (!info.type)
    {
      // sniff the content
      m_stream->seek((long) info.offset, librevenge::RVNG_SEEK_SET);
      if (1 == readU16(m_stream))
        info.type = readFileType(m_stream);
      else
        info.type = string();
    }

    assert(info.type);

    if (type == get(info.type))
      break;
  }

  return it;
}

librevenge::RVNGInputStream *SoftBookResourceDirImpl::createStream(const ResourceInfo &info) const
{
  m_stream->seek((long) info.offset, librevenge::RVNG_SEEK_SET);
  const unsigned char *const data = readNBytes(m_stream, info.length);
  return new EBOOKMemoryStream(data, info.length);
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
