/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * For further information visit http://libebook.sourceforge.net
 */

#include <cassert>
#include <cstring>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

#include <boost/numeric/conversion/cast.hpp>

#include <mspack.h>

#include "libebook_utils.h"
#include "HTMLHelpStream.h"
#include "EBOOKMemoryStream.h"

using boost::numeric_cast;
using std::shared_ptr;

using std::size_t;
using std::string;
using std::vector;

namespace libebook
{

namespace
{

extern "C"
{

  struct mspack_file *mspack_open(struct mspack_system *self, const char *filename, int mode);
  void mspack_close(struct mspack_file *file);
  int mspack_read(struct mspack_file *file, void *buffer, int bytes);
  int mspack_write(struct mspack_file *file, void *buffer, int bytes);
  int mspack_seek(struct mspack_file *file, off_t offset, int mode);
  off_t mspack_tell(struct mspack_file *file);
  void mspack_message(struct mspack_file *file, const char *format, ...);
  void *mspack_alloc(struct mspack_system *self, size_t bytes);
  void mspack_free(void *ptr);
  void mspack_copy(void *src, void *dest, size_t bytes);

}

struct MSPackFileData;
struct MSPackSystemData;

extern "C"
{

  struct MSPackFile
  {
    mspack_file base;
    MSPackFileData *data;
  };

  struct MSPackSystem
  {
    mspack_system base;
    MSPackSystemData *data;
  };

}

typedef std::unordered_map<string, MSPackFile> FileMap_t;

struct MSPackFileData
{
  MSPackSystemData *system;
  RVNGInputStreamPtr_t stream;
  string name;
  vector<unsigned char> data;

  MSPackFileData(MSPackSystemData *sys, const RVNGInputStreamPtr_t &input);
  MSPackFileData(MSPackSystemData *sys, const string &filename);

  // disable copying
  MSPackFileData(const MSPackFileData &);
  MSPackFileData &operator=(const MSPackFileData &);
};

struct MSPackSystemData
{
  MSPackFile container;
  FileMap_t fileMap;

  explicit MSPackSystemData(const RVNGInputStreamPtr_t &input);
  ~MSPackSystemData();

  // disable copying
  MSPackSystemData(const MSPackSystemData &);
  MSPackSystemData &operator=(const MSPackSystemData &);
};

MSPackFileData::MSPackFileData(MSPackSystemData *sys, const RVNGInputStreamPtr_t &input)
  : system(sys)
  , stream(input)
  , name()
  , data()
{
  assert(this->system);
}

MSPackFileData::MSPackFileData(MSPackSystemData *sys, const string &filename)
  : system(sys)
  , stream()
  , name(filename)
  , data()
{
  assert(this->system);
}

MSPackSystemData::MSPackSystemData(const RVNGInputStreamPtr_t &input)
  : container()
  , fileMap()
{
  container.data = new MSPackFileData(this, input);
}

MSPackSystemData::~MSPackSystemData()
{
  delete container.data;
  for (FileMap_t::const_iterator it = fileMap.begin(); fileMap.end() != it; ++it)
    delete it->second.data;
}

extern "C"
{

  struct mspack_file *mspack_open(struct mspack_system *const self, const char *const filename, const int mode)
  {
    MSPackSystem *const that = reinterpret_cast<MSPackSystem *>(self);

    assert(that);
    assert(that->data);
    assert(filename);

    (void) mode;

    if (('/' == filename[0]) && (0 == filename[1])) // the container
    {
      assert(MSPACK_SYS_OPEN_READ == mode);

      return reinterpret_cast<mspack_file *>(&that->data->container);
    }
    else // internal file
    {
      assert(MSPACK_SYS_OPEN_WRITE == mode); // the current impl. only allows writing

      const string name(filename);
      if (that->data->fileMap.end() == that->data->fileMap.find(name)) // the file has not been opened yet
      {
        MSPackFile file;
        file.data = new MSPackFileData(that->data, name);

        const FileMap_t::value_type value(name, file);
        FileMap_t::iterator it = that->data->fileMap.insert(value).first;
        return reinterpret_cast<mspack_file *>(&it->second);
      }
    }

    return 0;
  }

  void mspack_close(struct mspack_file *const file)
  {
    MSPackFile *const that = reinterpret_cast<MSPackFile *>(file);

    assert(that);
    assert(that->data);

    if (!that->data->stream) // internal file
    {
      assert(!that->data->name.empty());
      assert(that->data->system->fileMap.end() != that->data->system->fileMap.find(that->data->name));
      // nothing to do here
    }
  }

  int mspack_read(struct mspack_file *const file, void *const buffer, const int bytes)
  {
    MSPackFile *const that = reinterpret_cast<MSPackFile *>(file);

    assert(that);
    assert(that->data);
    assert(buffer);
    assert(bool(that->data->stream));

    if (0 >= bytes)
      return -MSPACK_ERR_ARGS;

    if (that->data->stream->isEnd())
      return 0;

    const unsigned long bytesToRead = static_cast<unsigned long>(static_cast<long>(bytes));
    unsigned long numRead = 0;
    const unsigned char *const data = that->data->stream->read(bytesToRead, numRead);

    if ((numRead < bytesToRead) && !that->data->stream->isEnd())
      return -MSPACK_ERR_READ;

    std::memcpy(buffer, data, bytes);

    return numeric_cast<int>(numRead);
  }

  int mspack_write(struct mspack_file *const file, void *const buffer, const int bytes)
  {
    MSPackFile *const that = reinterpret_cast<MSPackFile *>(file);

    assert(that);
    assert(that->data);
    assert(buffer);
    assert(!that->data->stream);

    unsigned char *const buf = reinterpret_cast<unsigned char *>(buffer);
    vector<unsigned char> &data = that->data->data;
    data.reserve(data.size() + bytes);
    data.insert(data.end(), buf, buf + bytes);

    return 0;
  }

  int mspack_seek(struct mspack_file *const file, const off_t offset, const int mode)
  {
    MSPackFile *const that = reinterpret_cast<MSPackFile *>(file);

    assert(that);
    assert(that->data);
    assert(bool(that->data->stream));

    librevenge::RVNG_SEEK_TYPE type = librevenge::RVNG_SEEK_SET;
    switch (mode)
    {
    case MSPACK_SYS_SEEK_START : // already set
      break;
    case MSPACK_SYS_SEEK_CUR :
      type = librevenge::RVNG_SEEK_CUR;
      break;
    case MSPACK_SYS_SEEK_END :
      type = librevenge::RVNG_SEEK_END;
      break;
    default :
      assert(0);
    }

    return (0 == that->data->stream->seek(offset, type)) ? 0 : -MSPACK_ERR_SEEK;
  }

  off_t mspack_tell(struct mspack_file *const file)
  {
    MSPackFile *const that = reinterpret_cast<MSPackFile *>(file);

    assert(that);
    assert(that->data);
    assert(bool(that->data->stream));

    return that->data->stream->tell();
  }

  void mspack_message(struct mspack_file *, const char *, ...)
  {
  }

  void *mspack_alloc(struct mspack_system *, const size_t bytes)
  {
    return std::malloc(bytes);
  }

  void mspack_free(void *ptr)
  {
    std::free(ptr);
  }

  void mspack_copy(void *const src, void *const dest, const size_t bytes)
  {
    std::memcpy(dest, src, bytes);
  }

}

unsigned countFiles(mschmd_file *const first)
{
  unsigned count = 0;

  for (mschmd_file *cur = first; 0 != cur; cur = cur->next)
    ++count;

  return count;
}

mschmd_file *getFileByPos(mschmd_file *const first, const unsigned pos)
{
  unsigned count = 0;
  for (mschmd_file *cur = first; 0 != cur; cur = cur->next, ++count)
  {
    if (pos == count)
      return cur;
  }

  assert(pos < count); // this should never happen

  return 0;
}

mschmd_file *findFileByName(mschmd_file *const first, const char *const name, unsigned &pos)
{
  for (mschmd_file *cur = first; 0 != cur; cur = cur->next, ++pos)
  {
    if (0 == strcmp(name, cur->filename))
      return cur;
  }

  return 0;
}

}

namespace
{

class FileCache
{
  typedef std::unordered_map<string, RVNGInputStreamPtr_t> NameMap_t;
  typedef std::map<unsigned, RVNGInputStreamPtr_t> IDMap_t;

public:
  FileCache();

  void put(const string &name, unsigned id, const RVNGInputStreamPtr_t &stream);

  const RVNGInputStreamPtr_t get(unsigned id) const;
  const RVNGInputStreamPtr_t get(const string &name) const;

private:
  NameMap_t m_nameMap;
  IDMap_t m_idMap;
};

FileCache::FileCache()
  : m_nameMap()
  , m_idMap()
{
}

void FileCache::put(const string &name, const unsigned id, const RVNGInputStreamPtr_t &stream)
{
  m_nameMap[name] = stream;
  m_idMap[id] = stream;
}

const RVNGInputStreamPtr_t FileCache::get(const unsigned id) const
{
  IDMap_t::const_iterator it = m_idMap.find(id);
  if (m_idMap.end() != it)
    return it->second;

  return RVNGInputStreamPtr_t();
}

const RVNGInputStreamPtr_t FileCache::get(const string &name) const
{
  NameMap_t::const_iterator it = m_nameMap.find(name);
  if (m_nameMap.end() != it)
    return it->second;

  return RVNGInputStreamPtr_t();
}

}

struct HTMLHelpStream::Impl
{
  const RVNGInputStreamPtr_t stream;
  MSPackSystem system;
  shared_ptr<mschm_decompressor> decompressor;
  mschmd_header *header;

  FileCache cache;

  unsigned fileCount;
  unsigned sysfileCount;
  bool fileCountSet;
  bool sysfileCountSet;

  Impl(const RVNGInputStreamPtr_t &input);

  unsigned getFileCount();
  unsigned getSysfileCount();

  const RVNGInputStreamPtr_t getSubStream(mschmd_file *file);

  // disable copying
  Impl(const Impl &);
  Impl &operator=(const Impl &);
};

HTMLHelpStream::Impl::Impl(const RVNGInputStreamPtr_t &input)
  : stream(input)
  , system()
  , decompressor()
  , header(0)
  , cache()
  , fileCount(0)
  , sysfileCount(0)
  , fileCountSet(false)
  , sysfileCountSet(false)
{
}

unsigned HTMLHelpStream::Impl::getFileCount()
{
  if (!fileCountSet)
    fileCount = countFiles(header->files);

  return fileCount;
}

unsigned HTMLHelpStream::Impl::getSysfileCount()
{
  if (!sysfileCountSet)
    sysfileCount = countFiles(header->sysfiles);

  return sysfileCount;
}

const RVNGInputStreamPtr_t HTMLHelpStream::Impl::getSubStream(mschmd_file *const file)
{
  assert(file);

  decompressor->extract(decompressor.get(), file, file->filename);

  FileMap_t::const_iterator it = system.data->fileMap.find(file->filename);
  if (system.data->fileMap.end() == it)
    throw PackageError();

  const vector<unsigned char> &data = it->second.data->data;

  // TODO: should this return a valid stream even if there are no data?
  if (data.empty())
    throw PackageError();

  const RVNGInputStreamPtr_t subStream(new EBOOKMemoryStream(&data[0], data.size()));
  return subStream;
}

HTMLHelpStream::HTMLHelpStream(const RVNGInputStreamPtr_t &stream)
  : m_impl(new Impl(stream))
{
  assert(bool(stream));

  {
    mspack_system &base = m_impl->system.base;

    base.open = mspack_open;
    base.close = mspack_close;
    base.read = mspack_read;
    base.write = mspack_write;
    base.seek = mspack_seek;
    base.tell = mspack_tell;
    base.message = mspack_message;
    base.alloc = mspack_alloc;
    base.free = mspack_free;
    base.copy = mspack_copy;
    base.null_ptr = 0;
  }
  m_impl->system.data = new MSPackSystemData(stream);

  mspack_system *const sys = reinterpret_cast<mspack_system *>(&m_impl->system);
  m_impl->decompressor.reset(mspack_create_chm_decompressor(sys), mspack_destroy_chm_decompressor);
  m_impl->header = m_impl->decompressor->open(m_impl->decompressor.get(), "/");

  if (!m_impl->header)
  {
    delete m_impl->system.data;
    throw PackageError();
  }
}

HTMLHelpStream::~HTMLHelpStream()
{
  assert(bool(m_impl->decompressor));
  m_impl->decompressor->close(m_impl->decompressor.get(), m_impl->header);
  delete m_impl->system.data;
}

bool HTMLHelpStream::isStructured()
{
  return true;
}

unsigned HTMLHelpStream::subStreamCount()
{
  return m_impl->getFileCount() + m_impl->getSysfileCount();
}

const char *HTMLHelpStream::subStreamName(const unsigned id)
{
  if (subStreamCount() < id)
    return 0;

  mschmd_file *file = 0;

  if (id >= m_impl->getFileCount())
    file = getFileByPos(m_impl->header->files, id);
  else
    file = getFileByPos(m_impl->header->sysfiles, id - m_impl->getFileCount());

  assert(file);

  return file->filename;
}

bool HTMLHelpStream::existsSubStream(const char *const name)
{
  if (bool(m_impl->cache.get(name)))
    return true;

  unsigned dummy = 0;
  mschmd_file *file = findFileByName(m_impl->header->files, name, dummy);
  if (!file)
    file = findFileByName(m_impl->header->sysfiles, name, dummy);

  return bool(file);
}

librevenge::RVNGInputStream *HTMLHelpStream::getSubStreamByName(const char *name)
{
  const RVNGInputStreamPtr_t stream = m_impl->cache.get(name);
  if (bool(stream))
    return stream.get();

  unsigned id = 0;
  mschmd_file *file = findFileByName(m_impl->header->files, name, id);
  if (!file)
    file = findFileByName(m_impl->header->sysfiles, name, id);

  if (file)
  {
    const RVNGInputStreamPtr_t newStream = m_impl->getSubStream(file);
    m_impl->cache.put(file->filename, id, newStream);
    return newStream.get();
  }

  return 0;
}

librevenge::RVNGInputStream *HTMLHelpStream::getSubStreamById(const unsigned id)
{
  if (subStreamCount() < id)
    return 0;

  const RVNGInputStreamPtr_t stream = m_impl->cache.get(id);
  if (bool(stream))
    return stream.get();

  mschmd_file *file = 0;

  if (id >= m_impl->getFileCount())
    file = getFileByPos(m_impl->header->files, id);
  else
    file = getFileByPos(m_impl->header->sysfiles, id - m_impl->getFileCount());

  assert(file);

  const RVNGInputStreamPtr_t newStream = m_impl->getSubStream(file);
  m_impl->cache.put(file->filename, id, newStream);
  return newStream.get();
}

const unsigned char *HTMLHelpStream::read(const unsigned long numBytes, unsigned long &numBytesRead)
{
  return m_impl->stream->read(numBytes, numBytesRead);
}

int HTMLHelpStream::seek(const long offset, const librevenge::RVNG_SEEK_TYPE seekType)
{
  return m_impl->stream->seek(offset, seekType);
}

long HTMLHelpStream::tell()
{
  return m_impl->stream->tell();
}

bool HTMLHelpStream::isEnd()
{
  return m_impl->stream->isEnd();
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
