#pragma once

#include "define.h"
#include "error.h"
#include "maybe.h"

namespace tfs {

struct DirectoryElement {
  std::string_view name;
  uint32_t const nodeId = 0;

  DirectoryElement() = default;
  DirectoryElement(std::string_view n, uint32_t id);

  bool operator==(DirectoryElement const &that) const {
    return nodeId == that.nodeId && name == that.name;
  }
};

Maybe<DirectoryElement, tfs::Error>
MakeElementFromBuffer(char *buff, size_t const &remaining);

class Directory {
public:
  Directory();
  explicit Directory(std::unique_ptr<char[]> pBlock);

  [[nodiscard]] Maybe<std::string_view, tfs::Error> GetDirectory() const;
  [[nodiscard]] Maybe<DirectoryElement, tfs::Error> GetParentDirectory() const;
  [[nodiscard]] Maybe<std::vector<DirectoryElement>, tfs::Error>
  GetFileList() const;

  std::unique_ptr<char[]> GetBuffer();

  void InitDirectory(std::string_view dirName, std::string_view parentDir,
                     uint32_t parentId);
  std::optional<tfs::Error> AddElement(DirectoryElement const &element);
  std::optional<tfs::Error> RemoveElement(uint32_t idToRemove);
  [[nodiscard]] std::optional<size_t> GetDirectorySize() const;

private:
  [[nodiscard]] std::optional<size_t> GetFirstFile() const;
  [[nodiscard]] std::optional<size_t> GetNextFile(size_t previousFile) const;
  [[nodiscard]] std::optional<size_t>
  FindElementByName(std::string_view nameToSearch) const;
  [[nodiscard]] std::optional<size_t>
  FindElementById(uint32_t idToSearch) const;
  [[nodiscard]] std::optional<DirectoryElement>
  ParseElement(size_t point) const;

  std::unique_ptr<char[]> m_pBlock;
};

} // namespace tfs
