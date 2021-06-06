#pragma once

#include "define.h"
#include "error.h"

class IDiskIO {
public:
  virtual ~IDiskIO(){};
  virtual std::optional<tfs::Error>
  ReadBlock(uint32_t const block, std::array<char, BLOCK_SIZE> &dst) = 0;
  virtual std::optional<tfs::Error>
  WriteBlock(uint32_t const block, std::array<char, BLOCK_SIZE> const &src) = 0;
};
