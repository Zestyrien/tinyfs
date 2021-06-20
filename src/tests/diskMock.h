#pragma once

#include "iDiskIO.h"

class DiskMock : public IDiskIO {
public:
  explicit DiskMock(uint64_t blocksCount);
  ~DiskMock() override = default;
  std::optional<tfs::Error>
  ReadBlock(uint32_t block, std::array<char, BLOCK_SIZE> &dst) override;
  std::optional<tfs::Error>
  WriteBlock(uint32_t block, std::array<char, BLOCK_SIZE> const &src) override;
  [[nodiscard]] bool IsEqual(std::vector<char> const &disk) const;
  void SwapDisk(std::vector<char> &disk);

private:
  std::vector<char> m_disk;
};
