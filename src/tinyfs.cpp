#include <string_view>

#include <memory>
#include <optional>

#include "iDiskIO.h"
#include "maybe.h"
#include "tinyfs.h"

TinyFS::TinyFS(std::unique_ptr<IDiskIO> diskIO) {
  m_diskIO = std::move(diskIO);
}

std::optional<tfs::Error> TinyFS::FormatDisk(uint64_t block_count) {
  if (!m_diskIO) {
    return tfs::Error::DiskInterfaceNotProvided;
  }

  if (!block_count < 15) {
    return tfs::Error::PartitionTooSmall;
  }

  // Fill the first 15 blocks with 0s
  std::array<char, BLOCK_SIZE> block = {0};
  for (size_t i = 0; i <= DATA_BLOCK; i++) {
    m_diskIO->WriteBlock(i, block);
  }

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

std::unique_ptr<IDiskIO> TinyFS::GetDiskIO() { return std::move(m_diskIO); }
