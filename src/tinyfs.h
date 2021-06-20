////////////////////////////////////////////////////////////////////////////////
// /___  ___/ / / / _ \    / / \ \  / /     /  ____/ / _____/                 //
//    / /    / / / / \ \  / /   \ \/ /     /  /__   / /____                   //
//   / /    / / / /   \ \/ /    /   /     / ____/  /____  /                   //
//  / /    / / / /     \  /    /   /     / /      /      /  Tiny File System  //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// Overview                                                                   //
//  ________________________________________                                  //
//  |   |   |   |        |      |           |                                 //
//  | S | i | d | inodes | hash | user data |                                 //
//  |___|___|___|________|______|___________|                                 //
//                                                                            //
//  S - superblock file system info                                           //
//  i - inodes bitmap                                                         //
//  d - data bitmap                                                           //
//                                                                            //
//  Consider the disk divided in blocks of size 4k.                           //
//  First block has file system info.                                         //
//  i and d are bitmaps to track respectively inodes and data blocks          //
//  allocation.                                                               //
//  The inodes blocks store the actual inodes                                 //
//  hash are block that contains the hash table for file lookup.              //
//  The remaining blocks contain user data.                                   //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "define.h"
#include "error.h"
#include "maybe.h"

class IDiskIO;

namespace tfs {

enum class INodeFlag {
  Indirect = 1, // Indirect inode points to a block that contains inodes
  // for the file.
  Directory = 2 // The inode points to a directory.
};

struct INode {
  uint32_t id;        // Unique id for the inode
  uint32_t parentId;  // Id of the parent inode, the parent can be a
                      // directory or an inode in case of indirection
  uint32_t fileSize;  // Size of the file in bytes
  uint32_t fileBlock; // Points to the data block
  INodeFlag flags;
};

} // namespace tfs

class TinyFS {
public:
  explicit TinyFS(std::unique_ptr<IDiskIO> diskIO);
  std::optional<tfs::Error> FormatDisk(uint64_t block_count);
  std::optional<tfs::Error> CreateFile(std::wstring_view fullName,
                                       bool directory);
  std::optional<tfs::Error> RemoveFile(std::wstring_view fullName,
                                       bool recursive);
  Maybe<uint64_t, tfs::Error> OpenFile(std::wstring_view fullName);
  Maybe<uint64_t, tfs::Error> GetFileSize(uint32_t inodeId);
  std::unique_ptr<IDiskIO> GetDiskIO();

private:
  std::unique_ptr<IDiskIO> m_diskIO;

  size_t InitDirectoryBlock(std::array<char, BLOCK_SIZE> &block,
                            std::string_view directoryName,
                            std::string_view parentDirectoryName);
};
