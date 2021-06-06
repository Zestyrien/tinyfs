
#include <algorithm>
#include <vector>

#include "diskMock.h"

DiskMock::DiskMock(uint64_t const blocksCount) {
  m_disk.resize(blocksCount * BLOCK_SIZE);
}

DiskMock::~DiskMock() {}

std::optional<tfs::Error>
DiskMock::ReadBlock(uint32_t const block, std::array<char, BLOCK_SIZE> &dst) {
  if (m_disk.size() / BLOCK_SIZE <= block) {
    return tfs::Error::OutOfRange;
  }

  for (size_t i = 0; i < BLOCK_SIZE; i++) {
    dst[i] = m_disk[block * BLOCK_SIZE + i];
  }

  return std::nullopt;
}

std::optional<tfs::Error>
DiskMock::WriteBlock(uint32_t const block,
                     std::array<char, BLOCK_SIZE> const &src) {
  if (m_disk.size() / BLOCK_SIZE <= block) {
    return tfs::Error::OutOfRange;
  }

  for (size_t i = 0; i < BLOCK_SIZE; i++) {
    m_disk[block * BLOCK_SIZE + i] = src[i];
  }

  return std::nullopt;
}

bool DiskMock::IsEqual(std::vector<char> const &disk) const {
  auto pair = std::mismatch(disk.begin(), disk.end(), m_disk.begin());
  return (pair.first == disk.end() && pair.second == m_disk.end());
}

void DiskMock::SwapDisk(std::vector<char> &disk) { m_disk.swap(disk); }
