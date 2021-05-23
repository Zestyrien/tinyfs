#pragma once

#include "iDiskIO.h"

class DiskMock : public IDiskIO {
public:
  DiskMock(uint64_t const &blocksCount);
  virtual ~DiskMock();
  virtual std::optional<tfs::Error>
  ReadBlock(uint32_t const &block,
            std::array<char, BLOCK_SIZE> &dst) override;
  virtual std::optional<tfs::Error>
  WriteBlock(uint32_t const &block,
             std::array<char, BLOCK_SIZE> const &src) override;
  bool IsEqual(std::vector<char> const &disk) const;
  void SwapDisk(std::vector<char> &disk);

private:
  std::vector<char> m_disk;
};
