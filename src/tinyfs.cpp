#include <string_view>

#include "iDiskIO.h"

#include "tinyfs.h"

TinyFS::TinyFS(std::unique_ptr<IDiskIO> diskIO) {
  m_diskIO = std::move(diskIO);
}
std::optional<tfs::Error> TinyFS::FormatDisk(uint64_t block_count) {
  return std::nullopt;
}

std::optional<tfs::Error> TinyFS::CreateFile(std::wstring_view fullName,
                                             bool directory) {
  return std::nullopt;
}

std::optional<tfs::Error> TinyFS::RemoveFile(std::wstring_view fullName,
                                             bool recursive) {
  return std::nullopt;
}

Maybe<uint64_t, tfs::Error> TinyFS::OpenFile(std::wstring_view fullName) {
  return Maybe<uint64_t, tfs::Error>(10);
}

Maybe<uint64_t, tfs::Error> TinyFS::GetFileSize(uint32_t const &inodeId) {
  return Maybe<uint64_t, tfs::Error>(10);
}
