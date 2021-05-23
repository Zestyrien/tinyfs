#include <gtest/gtest.h>

#include "diskMock.h"
#include "tinyfs.h"

TEST(TinyFS, WhenFormatDiskFormatsA15BlocksDisk) {
  auto disk = std::make_unique<DiskMock>(15);
  TinyFS tiny(std::move(disk));
  tiny.FormatDisk(15);

  // For now the super block is 0
  // The i block will have only 1 inode for the root folder
  // The is empty as the location for the root inode is known
  // The data bit map has only one entry for the root data block
  // For the first implementation the filesystem will use 5 blocks for the
  // inodes, 5 for the hash and the rest is for user data.

  auto result = tiny.GetDiskIO();
  std::array<char, BLOCK_SIZE> block = {0};
  std::array<char, BLOCK_SIZE> dst;

  EXPECT_FALSE(result->ReadBlock(SUPER_BLOCK, dst));

  bool error = false;
  for (auto const &i : dst) {
    if (i != 0) {
      error = true;
      break;
    }
  }

  EXPECT_FALSE(error) << "SUPER block not zero after format.";

  EXPECT_FALSE(result->ReadBlock(INODE_BITMAP_BLOCK, dst));
}
