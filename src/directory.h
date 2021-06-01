#pragma once

#include "define.h"
#include "error.h"
#include "maybe.h"

namespace tfs {

struct DirectoryElement {
  std::string_view name = "";
  uint32_t nodeId = 0;

  DirectoryElement() {}
  DirectoryElement(char *n, uint32_t const &id);
};

Maybe<DirectoryElement, tfs::Error>
MakeElementFromBuffer(char *buff, size_t const &remaining);

class Directory {
public:
  Directory();
  Directory(std::unique_ptr<char[]> pBlock);

  Maybe<std::string_view, tfs::Error> GetDirectory();

  Maybe<DirectoryElement, tfs::Error> GetParentDirectory();

  Maybe<std::vector<DirectoryElement>, tfs::Error> GetFileList();

  std::unique_ptr<char[]> GetBuffer();

private:
  std::unique_ptr<char[]> m_pBlock;
};

} // namespace tfs
