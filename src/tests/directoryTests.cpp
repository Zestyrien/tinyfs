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
  EXPECT_TRUE(fileList.getResult().empty());
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

TEST(tfsDirectory, InitDirectory) {
  std::unique_ptr<char[]> pBlock(new char[BLOCK_SIZE]);
  auto dir = tfs::Directory(std::move(pBlock));

  tfs::DirectoryElement const parentEl("Uther", 1);
  dir.InitDirectory("Arthur", parentEl.name, parentEl.nodeId);
  auto dirName = dir.GetDirectory();
  EXPECT_FALSE(dirName.error());
  if (!dirName.error()) {
    EXPECT_EQ(dirName.getResult(), "Arthur");
  }

  auto parent = dir.GetParentDirectory();
  EXPECT_FALSE(parent.error());
  if (!parent.error()) {
    EXPECT_EQ(parent.getResult(), parentEl);
  }
  auto files = dir.GetFileList();
  EXPECT_FALSE(files.error());
  if (!files.error()) {
    EXPECT_TRUE(files.getResult().empty());
  }
}

TEST(tfsDirectory,
     AddElement_WhenTheElementDoesNotExistAndTheDirectoryIsEmpty) {
  std::unique_ptr<char[]> pBlock(new char[BLOCK_SIZE]);
  memcpy(pBlock.get(), EMPTY_MARS.data(), EMPTY_MARS.size());
  auto dir = tfs::Directory(std::move(pBlock));

  tfs::DirectoryElement el("Sterzo", 1);
  auto result = dir.AddElement(el);
  EXPECT_FALSE(result);

  auto files = dir.GetFileList();
  if (!files.error()) {
    EXPECT_EQ(files.getResult().size(), 1);
    for (auto const &file : files.getResult()) {
      EXPECT_EQ(file, el);
    }
  } else {
    EXPECT_TRUE(false);
  }
}

TEST(tfsDirectory, GetDirectorySize_WhenDirectoryIsEmpty) {
  std::unique_ptr<char[]> pBlock(new char[BLOCK_SIZE]);
  memcpy(pBlock.get(), EMPTY_MARS.data(), EMPTY_MARS.size());
  auto dir = tfs::Directory(std::move(pBlock));

  auto result = dir.GetDirectorySize();
  EXPECT_TRUE(result);

  EXPECT_EQ(result.value(), EMPTY_MARS.size());
}

TEST(tfsDirectory, GetDirectorySize_WhenDirectoryIsNotEmpty) {
  std::unique_ptr<char[]> pBlock(new char[BLOCK_SIZE]);
  memcpy(pBlock.get(), MARS.data(), MARS.size());
  auto dir = tfs::Directory(std::move(pBlock));

  auto result = dir.GetDirectorySize();
  EXPECT_TRUE(result);

  EXPECT_EQ(result.value(), MARS.size());
}

TEST(tfsDirectory,
     AddElement_WhenTheElementDoesNotExistAndDirectoryIsNotEmpty) {
  std::unique_ptr<char[]> pBlock(new char[BLOCK_SIZE]);
  memcpy(pBlock.get(), MARS.data(), MARS.size());
  auto dir = tfs::Directory(std::move(pBlock));

  tfs::DirectoryElement el("Carolina", 4);
  auto result = dir.AddElement(el);
  EXPECT_FALSE(result);

  std::array<tfs::DirectoryElement, 4> expected = {
      tfs::DirectoryElement("Sterzo", 1),
      tfs::DirectoryElement("Pistone", 2),
      tfs::DirectoryElement("Turbo", 3),
      tfs::DirectoryElement("Carolina", 4),
  };

  auto files = dir.GetFileList();
  if (!files.error()) {
    auto const resultFiles = files.getResult();
    EXPECT_EQ(resultFiles.size(), expected.size());
    for (size_t i = 0; i < resultFiles.size() && i < expected.size(); i++) {
      EXPECT_TRUE(expected.at(i) == resultFiles.at(i))
          << "Error on element: " << i;
    }
  } else {
    EXPECT_TRUE(false);
  }
}

TEST(tfsDirectory, AddElement_WhenTheElementExists) {
  std::unique_ptr<char[]> pBlock(new char[BLOCK_SIZE]);
  memcpy(pBlock.get(), MARS.data(), MARS.size());
  auto dir = tfs::Directory(std::move(pBlock));

  tfs::DirectoryElement el("Sterzo", 1);
  dir.AddElement(el);
  auto result = dir.AddElement(el);
  EXPECT_TRUE(result);
  EXPECT_EQ(result.value(), tfs::Error::ElementAlreadyExists);
}

TEST(tfsDirectory, RemoveElement_WhenTheElementDoesNotExist) {
  std::unique_ptr<char[]> pBlock(new char[BLOCK_SIZE]);
  memcpy(pBlock.get(), EMPTY_MARS.data(), EMPTY_MARS.size());
  auto dir = tfs::Directory(std::move(pBlock));

  tfs::DirectoryElement el("Sterzo", 1);
  auto result = dir.RemoveElement(el.nodeId);
  EXPECT_TRUE(result);
  EXPECT_EQ(result.value(), tfs::Error::ElementNotFound);
}

TEST(tfsDirectory, RemoveElement_WhenTheElementExists) {
  std::unique_ptr<char[]> pBlock(new char[BLOCK_SIZE]);
  memcpy(pBlock.get(), MARS.data(), MARS.size());
  auto dir = tfs::Directory(std::move(pBlock));

  tfs::DirectoryElement el("Sterzo", 1);
  auto result = dir.RemoveElement(el.nodeId);
  EXPECT_FALSE(result);

  auto files = dir.GetFileList();
  if (!files.error()) {
    auto const returnedFiles = files.getResult();
    EXPECT_EQ(returnedFiles.size(), 2);
    for (auto const &file : returnedFiles) {
      EXPECT_NE(file, el);
    }
  } else {
    EXPECT_TRUE(false);
  }
}
