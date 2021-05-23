#pragma once

#include "iDiskIO.h"
#include <vector>

class DiskMock : public IDiskIO {
public:
  DiskMock(uint64_t size);
  virtual Maybe<uint32_t, tfs::Error> Read(uint32_t const &block,
                                           char *dst) override;
  virtual Maybe<uint32_t, tfs::Error> Write(uint32_t const &block,
                                            char const *src) override;
  bool IsEqual(std::vector<char> const &disk) const;

private:
  std::vector<char> m_disk;
};
