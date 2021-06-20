#include "directory.h"
#include <cstring>
#include <iostream>

namespace tfs {

DirectoryElement::DirectoryElement(std::string_view n, uint32_t const id)
    : name(n), nodeId(id) {}

Maybe<DirectoryElement, tfs::Error>
MakeElementFromBuffer(char *buff, size_t const &remaining) {
  std::string_view name(buff);
  if (remaining > (name.size() + sizeof(uint32_t))) {
    return Maybe<DirectoryElement, tfs::Error>(DirectoryElement(
        // buff, *(reinterpret_cast<uint32_t *>(&buff[name.size()]))));
        buff, *((uint32_t *)(buff + 1 + name.size()))));
  }

  return Maybe<DirectoryElement, tfs::Error>(tfs::Error::InvalidDirectory);
}

Directory::Directory()
    : Directory(std::unique_ptr<char[]>(new char[BLOCK_SIZE])) {
  m_pBlock[0] = '\0';
  m_pBlock[1] = '\0';
}

Directory::Directory(std::unique_ptr<char[]> pBlock) {
  m_pBlock = std::move(pBlock);
}

Maybe<std::string_view, tfs::Error> Directory::GetDirectory() const {
  if (m_pBlock == nullptr || m_pBlock[0] == '\0') {
    return Maybe<std::string_view, tfs::Error>(tfs::Error::InvalidDirectory);
  }

  return Maybe<std::string_view, tfs::Error>(std::string_view(&m_pBlock[0]));
}

Maybe<DirectoryElement, tfs::Error> Directory::GetParentDirectory() const {
  for (size_t point = 0; point < BLOCK_SIZE; point++) {
    if (m_pBlock[point] == '\0') {
      point++;
      return MakeElementFromBuffer(&m_pBlock[point], BLOCK_SIZE - point);
    }
  }

  return Maybe<DirectoryElement, tfs::Error>(tfs::Error::InvalidDirectory);
}

Maybe<std::vector<DirectoryElement>, tfs::Error>
Directory::GetFileList() const {
  std::vector<DirectoryElement> files;
  auto file = GetFirstFile();
  do {
    if (file) {
      auto const element = MakeElementFromBuffer(&m_pBlock[file.value()],
                                                 BLOCK_SIZE - file.value());
      if (element.error()) {
        break;
      }

      files.push_back(element.getResult());
      file = GetNextFile(file.value());
    }
  } while (file);

  return Maybe<std::vector<DirectoryElement>, tfs::Error>(files);
}

std::unique_ptr<char[]> Directory::GetBuffer() { return std::move(m_pBlock); }

std::optional<size_t> Directory::GetFirstFile() const {
  size_t point = 0;
  // Skip dir name
  while (m_pBlock[point] != '\0') {
    point++;
  }

  if (point >= BLOCK_SIZE) {
    return std::nullopt;
  }
  // Skip parent dir name
  point++;
  while (m_pBlock[point] != '\0') {
    point++;
  }

  if (point >= BLOCK_SIZE) {
    return std::nullopt;
  }
  // Skip parent inode
  point += 5;
  if (point >= BLOCK_SIZE) {
    return std::nullopt;
  }

  if (m_pBlock[point] != '\0') {
    return point;
  }

  return std::nullopt;
}

std::optional<size_t> Directory::GetNextFile(size_t point) const {
  // Skip file name
  while (m_pBlock[point] != '\0') {
    point++;
  }

  if (point >= BLOCK_SIZE) {
    return std::nullopt;
  }

  // Skip inode
  point += sizeof(uint32_t) + 1;

  if (point >= BLOCK_SIZE) {
    return std::nullopt;
  }

  if (m_pBlock[point] != '\0') {
    return point;
  }

  return std::nullopt;
}

std::optional<size_t> Directory::GetDirectorySize() const {
  size_t count = 0;
  // Skip dir name
  while (m_pBlock[count] != '\0') {
    count++;
  }
  count++;

  // Skip parent dir name
  while (m_pBlock[count] != '\0') {
    count++;
  }
  count++;

  // Skip parent inode
  count += sizeof(uint32_t);

  auto file = GetNextFile(count);
  auto const needToSkipLast = file.has_value();
  while (file) {
    count = file.value();
    file = GetNextFile(count);
  }

  if (needToSkipLast) {
    // count points to the last file.
    // Add name and inode size.
    while (m_pBlock[count] != '\0') {
      count++;
    }
    count++;

    count += sizeof(uint32_t);
  }

  count += 2; // Double \0

  return count;
}

std::optional<size_t>
Directory::FindElementByName(std::string_view nameToSearch) const {
  auto file = GetFirstFile();
  while (file) {
    std::string_view name(&m_pBlock[file.value()]);
    if (name == nameToSearch) {
      return file.value();
    }

    file = GetNextFile(file.value());
  }

  return std::nullopt;
}

std::optional<size_t> Directory::FindElementById(uint32_t idToSearch) const {
  auto file = GetFirstFile();
  while (file) {
    auto const id = ParseElement(file.value());
    if (id.value().nodeId == idToSearch) {
      return file.value();
    }

    file = GetNextFile(file.value());
  }

  return std::nullopt;
}

std::optional<DirectoryElement> Directory::ParseElement(size_t point) const {
  auto nameLen = 0;
  while (m_pBlock[point + nameLen] != '\0') {
    nameLen++;
  }
  nameLen++;

  if (nameLen == 0) {
    return std::nullopt;
  }

  return DirectoryElement(&m_pBlock[point], m_pBlock[point + nameLen]);
}

void Directory::InitDirectory(std::string_view dirName,
                              std::string_view parentDir,
                              uint32_t const parentId) {
  size_t count = dirName.size();
  dirName.copy(m_pBlock.get(), count);
  m_pBlock[count] = '\0';
  count++;

  parentDir.copy(&m_pBlock[count], parentDir.size());
  count += parentDir.size();
  m_pBlock[count] = '\0';
  count++;

  memcpy(&m_pBlock[count], &parentId, sizeof(uint32_t));
  count += sizeof(uint32_t);
  count++;
  // Double null to terminate folder
  m_pBlock[count] = '\0';
  m_pBlock[count + 1] = '\0';
}

std::optional<tfs::Error>
Directory::AddElement(DirectoryElement const &element) {
  auto file = FindElementById(element.nodeId);
  if (file) {
    return tfs::Error::ElementAlreadyExists;
  }

  file = FindElementByName(element.name);
  if (file) {
    return tfs::Error::ElementAlreadyExists;
  }

  auto dirSize = GetDirectorySize();
  if (!dirSize) {
    return tfs::Error::InvalidDirectory;
  }

  auto size = dirSize.value() - 2;

  element.name.copy(&m_pBlock[size], element.name.size());
  size += element.name.size() + 1;
  memcpy(&m_pBlock[size], &element.nodeId, sizeof(uint32_t));
  size += sizeof(uint32_t) + 1;
  // Double null to terminate folder
  m_pBlock[size] = '\0';
  m_pBlock[size + 1] = '\0';

  std::cout << std::endl;
  return std::nullopt;
}

std::optional<tfs::Error> Directory::RemoveElement(uint32_t idToRemove) {

  auto const size = GetDirectorySize();
  if (!size) {
    return tfs::Error::InvalidDirectory;
  }

  auto const file = FindElementById(idToRemove);
  if (!file) {
    return tfs::Error::ElementNotFound;
  }

  // name size + \0 + 1000
  auto const offset = strlen(&m_pBlock[file.value()]) + 1 + sizeof(uint32_t);
  auto const remaining = size.value() - file.value() - offset;

  memcpy(&m_pBlock[file.value()], &m_pBlock[file.value() + offset], remaining);

  return std::nullopt;
}

} // namespace tfs
