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

TEST(tfsDirectory, GetFileList_WhenTheDirectoryIsEmpty) {
  std::unique_ptr<char[]> pBlock(new char[BLOCK_SIZE]);
  memcpy(pBlock.get(), EMPTY_MARS.data(), EMPTY_MARS.size());
  auto dir = tfs::Directory(std::move(pBlock));

  auto fileList = dir.GetFileList();
  EXPECT_FALSE(fileList.error());
  EXPECT_TRUE(fileList.getResult().size() == 0);
}

TEST(tfsDirectory, GetFileList_WhenTheDirectoryIsNotEmpty) {
  std::unique_ptr<char[]> pBlock(new char[BLOCK_SIZE]);
  memcpy(pBlock.get(), MARS.data(), MARS.size());
  auto dir = tfs::Directory(std::move(pBlock));

  auto fileList = dir.GetFileList();
  EXPECT_FALSE(fileList.error());

  std::vector<tfs::DirectoryElement> const expectedList = {
      tfs::DirectoryElement("Sterzo", 1),
      tfs::DirectoryElement("Pistone", 2),
      tfs::DirectoryElement("Turbo", 3),
  };

  auto const result = fileList.getResult();

  if (result.size() == expectedList.size()) {
    for (size_t i = 0; i < expectedList.size(); i++) {
      EXPECT_EQ(expectedList[i].name, result[i].name);
      EXPECT_EQ(expectedList[i].nodeId, result[i].nodeId);
    }
  } else {
    EXPECT_TRUE(false);
  }
}

TEST(tfsDirectory, GetBuffer_WhenTheBufferIsSet) {
  std::unique_ptr<char[]> pBlock(new char[BLOCK_SIZE]);
  std::string const expected = "TinyFS";
  memcpy(pBlock.get(), expected.data(), expected.size());

  auto dir = tfs::Directory(std::move(pBlock));

  auto buffer = dir.GetBuffer();
  EXPECT_TRUE(buffer);
  EXPECT_TRUE(strcmp(buffer.get(), expected.c_str()) == 0);
}

TEST(tfsDirectory, GetBuffer_WhenTheBufferRetreivedTwice) {
  std::unique_ptr<char[]> pBlock(new char[BLOCK_SIZE]);
  std::string const expected = "TinyFS";
  memcpy(pBlock.get(), expected.data(), expected.size());

  auto dir = tfs::Directory(std::move(pBlock));
  auto bufferOne = dir.GetBuffer();
  auto bufferTwo = dir.GetBuffer();
  EXPECT_TRUE(bufferOne);
  EXPECT_FALSE(bufferTwo);
}
