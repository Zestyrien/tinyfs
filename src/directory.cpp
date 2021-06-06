#include "directory.h"

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

  return tfs::Error::InvalidDirectory;
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

  return std::string_view(&m_pBlock[0]);
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
} // namespace tfs
