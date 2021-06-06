#pragma once

#include "define.h"
#include "error.h"
#include "maybe.h"

namespace tfs {

struct DirectoryElement {
  std::string_view name = "";
  uint32_t nodeId = 0;

  DirectoryElement() {}
  DirectoryElement(std::string_view n, uint32_t const id);
};

Maybe<DirectoryElement, tfs::Error>
MakeElementFromBuffer(char *buff, size_t const &remaining);

class Directory {
public:
  Directory();
  Directory(std::unique_ptr<char[]> pBlock);

  Maybe<std::string_view, tfs::Error> GetDirectory() const;

  Maybe<DirectoryElement, tfs::Error> GetParentDirectory() const;

  Maybe<std::vector<DirectoryElement>, tfs::Error> GetFileList() const;

  std::unique_ptr<char[]> GetBuffer();

private:
  std::optional<size_t> GetFirstFile() const;
  std::optional<size_t> GetNextFile(size_t previousFile) const;

  std::unique_ptr<char[]> m_pBlock;
};

} // namespace tfs
