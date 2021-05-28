#pragma once

#include "error.h"
#include <array>
#include <optional>

#define BLOCK_SIZE 4000

#define SUPER_BLOCK 0
#define INODE_BITMAP_BLOCK 1
#define DATA_BITMAP_BLOCK 2
#define INODES_BLOCK 3
#define INODES_BLOCK_COUNT 5
#define HASH_BLOCK 8
#define HASH_BLOCK_COUNT 5
#define DATA_BLOCKS 14

class IDiskIO {
public:
  virtual ~IDiskIO(){};
  virtual std::optional<tfs::Error>
  ReadBlock(uint32_t const &block, std::array<char, BLOCK_SIZE> &dst) = 0;
  virtual std::optional<tfs::Error>
  WriteBlock(uint32_t const &block,
             std::array<char, BLOCK_SIZE> const &src) = 0;
};
