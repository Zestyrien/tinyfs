#include <gtest/gtest.h>

#include "diskMock.h"
#include "tinyfs.h"

TEST(READ, read) {
  auto disk = std::make_unique<DiskMock>(DiskMock(1000));
  TinyFS tiny(std::move(disk));

  tiny.FormatDisk(1000);
}
