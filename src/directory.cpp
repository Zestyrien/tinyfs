#include "directory.h"

namespace tfs {

DirectoryElement::DirectoryElement(char *n, uint32_t const &id)
    : name(n), nodeId(id) {}

Maybe<DirectoryElement, tfs::Error>
MakeElementFromBuffer(char *buff, size_t const &remaining) {
  std::string_view name(buff);
  if (remaining > (name.size() + sizeof(uint32_t))) {
    return Maybe<DirectoryElement, tfs::Error>(DirectoryElement(
        buff, *(reinterpret_cast<uint32_t *>(&buff[name.size()]))));
  }

  return tfs::Error::InvalidDirectory;
}

Directory::Directory()
    : Directory(std::move(std::unique_ptr<char[]>(new char[BLOCK_SIZE]))) {
  m_pBlock[0] = '\0';
  m_pBlock[1] = '\0';
}

Directory::Directory(std::unique_ptr<char[]> pBlock) {
  m_pBlock = std::move(pBlock);
}

Maybe<std::string_view, tfs::Error> Directory::GetDirectory() {
  if (m_pBlock == nullptr || m_pBlock[0] == '\0') {
    return Maybe<std::string_view, tfs::Error>(tfs::Error::InvalidDirectory);
  }

  return std::string_view(&m_pBlock[0]);
}

Maybe<DirectoryElement, tfs::Error> Directory::GetParentDirectory() {
  for (size_t point = 0; point < BLOCK_SIZE; point++) {
    if (m_pBlock[point] == '\0') {
      point++;
      return MakeElementFromBuffer(&m_pBlock[point], BLOCK_SIZE - point);
    }
  }

  return Maybe<DirectoryElement, tfs::Error>(tfs::Error::InvalidDirectory);
}

Maybe<std::vector<DirectoryElement>, tfs::Error> Directory::GetFileList() {
  return Maybe<std::vector<DirectoryElement>, tfs::Error>(
      tfs::Error::InvalidDirectory);
}

} // namespace tfs
