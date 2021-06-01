#include <gtest/gtest.h>

#include "directory.h"
#include "expectedBuffers.h"

TEST(tfsDirectory, GetDirectory_WhenDirectoryIsRoot) {
  std::unique_ptr<char[]> pBlock(new char[BLOCK_SIZE]);
  memcpy(pBlock.get(), EMPTY_ROOT.data(), EMPTY_ROOT.size());
  auto dir = tfs::Directory(std::move(pBlock));

  auto element = dir.GetDirectory();
  EXPECT_FALSE(element.error());
  EXPECT_EQ(element.getResult(), "/");
}

TEST(tfsDirectory, GetDirectory_WhenDirectoryIsNotRoot) {
  std::unique_ptr<char[]> pBlock(new char[BLOCK_SIZE]);
  memcpy(pBlock.get(), EMPTY_MARS.data(), EMPTY_MARS.size());
  auto dir = tfs::Directory(std::move(pBlock));

  auto element = dir.GetDirectory();
  EXPECT_FALSE(element.error());
  EXPECT_EQ(element.getResult(), "Mars");
}

TEST(tfsDirectory, GetDirectory_WhenTheDirectoryIsInvalid) {
  auto dir = tfs::Directory();

  auto element = dir.GetDirectory();
  EXPECT_TRUE(element.error());
  EXPECT_EQ(element.getError(), tfs::Error::InvalidDirectory);
}

TEST(tfsDirectory, GetParentDirectory_WhenDirectoryIsRoot) {
  std::unique_ptr<char[]> pBlock(new char[BLOCK_SIZE]);
  memcpy(pBlock.get(), EMPTY_ROOT.data(), EMPTY_ROOT.size());
  auto dir = tfs::Directory(std::move(pBlock));

  auto element = dir.GetParentDirectory();
  EXPECT_FALSE(element.error());
  EXPECT_EQ(element.getResult().name, "");
  EXPECT_EQ(element.getResult().nodeId, 0);
}

TEST(tfsDirectory, GetParentDirectory_WhenDirectoryIsNotRoot) {
  std::unique_ptr<char[]> pBlock(new char[BLOCK_SIZE]);
  memcpy(pBlock.get(), EMPTY_MARS.data(), EMPTY_MARS.size());
  auto dir = tfs::Directory(std::move(pBlock));

  auto element = dir.GetParentDirectory();
  EXPECT_FALSE(element.error());
  EXPECT_EQ(element.getResult().name, "/");
  EXPECT_EQ(element.getResult().nodeId, 0);
}

TEST(tfsDirectory, GetFileList_WhenTheDirectoryIsEmpty) {}

TEST(tfsDirectory, GetFileList_WhenTheDirectoryIsNotEmpty) {}

TEST(tfsDirectory, GetFileList_WhenTheDirectoryIsInvalid) {
  auto dir = tfs::Directory();

  auto list = dir.GetFileList();
  EXPECT_TRUE(list.error());
  EXPECT_EQ(list.getError(), tfs::Error::InvalidDirectory);
}

TEST(tfsDirectory, GetBuffer_WhenTheBufferIsSet) {}

TEST(tfsDirectory, GetBuffer_WhenTheBufferIsNull) {}
