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

  if (block_count < 15) {
    return tfs::Error::PartitionTooSmall;
  }

  // Fill the first 15 blocks with 0s
  std::array<char, BLOCK_SIZE> block = {0};
  for (size_t i = 0; i <= DATA_BLOCKS; i++) {
    m_diskIO->WriteBlock(i, block);
  }

  // Set the first bit at zero for the inodes and dat bitmaps as they will
  // contain the root forlder
  block[0] = 0b10000000;
  m_diskIO->WriteBlock(INODE_BITMAP_BLOCK, block);
  m_diskIO->WriteBlock(DATA_BITMAP_BLOCK, block);

  auto const fileSize = InitDirectoryBlock(block, "/", "");
  m_diskIO->WriteBlock(DATA_BLOCKS, block);

  std::array<char, BLOCK_SIZE> inodes = {0};

  auto inode = reinterpret_cast<tfs::INode *>(&inodes);
  inode->id = 0;
  inode->parentId = 0;
  inode->fileSize = fileSize;
  inode->flags = tfs::INodeFlag::Directory;
  inode->fileBlock = 0;
  m_diskIO->WriteBlock(INODES_BLOCK, inodes);

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

size_t TinyFS::InitDirectoryBlock(std::array<char, BLOCK_SIZE> &block,
                                  std::string_view directoryName,
                                  std::string_view parentDirectoryName) {

  size_t count = 0;
  directoryName.copy(&block[count], directoryName.size());
  count += directoryName.size();
  block[count] = '\0';
  count++;

  parentDirectoryName.copy(&block[count], parentDirectoryName.size());
  count += parentDirectoryName.size();

  block[count] = '\0';
  count++;

  // Double null to signal directory termination
  block[count] = '\0';
  count++;
  block[count] = '\0';
  count++;

  return count;
}
