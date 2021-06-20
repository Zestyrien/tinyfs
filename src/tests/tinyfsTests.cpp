#include "diskMock.h"
#include "expectedBuffers.h"
#include "tinyfs.h"
#include <gtest/gtest.h>

bool Compare(std::array<char, BLOCK_SIZE> const &one,
             std::array<char, BLOCK_SIZE> const &two) {

  return memcmp(one.data(), two.data(), BLOCK_SIZE) == 0;
}

TEST(TinyFS, WhenFormatDiskFormatsLessThan15Blocks) {
  auto disk = std::make_unique<DiskMock>(14);
  TinyFS tiny(std::move(disk));
  auto const error = tiny.FormatDisk(14);
  EXPECT_TRUE(error.has_value());
  EXPECT_EQ(error, tfs::Error::PartitionTooSmall);
}

TEST(TinyFS, WhenFormatDiskFormatsA15BlocksDisk) {
  auto disk = std::make_unique<DiskMock>(15);
  TinyFS tiny(std::move(disk));
  EXPECT_FALSE(tiny.FormatDisk(15));

  // For now the super block is 0
  // The i block will have only 1 inode for the root folder
  // The is empty as the location for the root inode is known
  // The data bit map has only one entry for the root data block
  // For the first implementation the filesystem will use 5 blocks for the
  // inodes, 5 for the hash and the rest is for user data.

  auto result = tiny.GetDiskIO();
  std::array<char, BLOCK_SIZE> block = {0};
  std::array<char, BLOCK_SIZE> dst = {};

  EXPECT_FALSE(result->ReadBlock(SUPER_BLOCK, dst));
  EXPECT_TRUE(Compare(block, dst)) << "SUPER block not zero after format.";

  // Root is always in 0, no need to hash it
  EXPECT_FALSE(result->ReadBlock(HASH_BLOCK, dst));
  EXPECT_TRUE(Compare(block, dst));

  // Set first bit to one to compare with the bitmaps
  block[0] = static_cast<char>(0b10000000);

  EXPECT_FALSE(result->ReadBlock(INODE_BITMAP_BLOCK, dst));
  EXPECT_TRUE(Compare(block, dst));

  EXPECT_FALSE(result->ReadBlock(DATA_BITMAP_BLOCK, dst));
  EXPECT_TRUE(Compare(block, dst));

  auto inode = reinterpret_cast<tfs::INode *>(&block);
  inode->id = 0;
  inode->parentId = 0;
  inode->fileBlock = 0;
  inode->flags = tfs::INodeFlag::Directory;
  inode->fileSize = EMPTY_ROOT.size();

  EXPECT_FALSE(result->ReadBlock(INODES_BLOCK, dst));
  EXPECT_TRUE(Compare(block, dst));

  EXPECT_FALSE(result->ReadBlock(DATA_BLOCKS, dst));
  EXPECT_TRUE(memcmp(EMPTY_ROOT.data(), dst.data(), EMPTY_ROOT.size()) == 0);
}
