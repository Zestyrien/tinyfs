#pragma once

#include "error.h"
#include "maybe.h"

#define BLOCK_SIZE 4000;

class IDiskIO {
public:
  virtual Maybe<uint32_t, tfs::Error> Read(uint32_t const &block,
                                           char *dst) = 0;
  virtual Maybe<uint32_t, tfs::Error> Write(uint32_t const &block,
                                            char const *src) = 0;
};
