#include "diskMock.h"

DiskMock::DiskMock(uint64_t size) { m_disk.resize(size); }

Maybe<uint32_t, tfs::Error> DiskMock::Read(uint32_t const &block, char *dst) {
  return Maybe<uint32_t, tfs::Error>(0);
}

Maybe<uint32_t, tfs::Error> DiskMock::Write(uint32_t const &block,
                                            char const *src) {
  return Maybe<uint32_t, tfs::Error>(0);
}

bool DiskMock::IsEqual(std::vector<char> const &disk) const { return false; }
