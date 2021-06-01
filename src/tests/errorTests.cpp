#include <gtest/gtest.h>

#include "error.h"

TEST(tfsError, GetErrorInfo_WhenThereIsNotEnoughSpace) {
  EXPECT_EQ(L"Operation failed: not enough space on the disk.",
            GetErrorInfo(tfs::Error::NotEnoughSpace));
}

TEST(tfsError, GetErrorInfo_WhenDiskInterfaceIsNotProvided) {
  EXPECT_EQ(L"Internal error: disk interface not provided.",
            GetErrorInfo(tfs::Error::DiskInterfaceNotProvided));
}

TEST(tfsError, GetErrorInfo_WhenTheOperationReturnsOutOfRange) {
  EXPECT_EQ(L"Operation failed: out of range.",
            GetErrorInfo(tfs::Error::OutOfRange));
}

TEST(tfsError, GetErrorInfo_WhenThePartitionIsTooSmall) {
  EXPECT_EQ(
      L"Operation failed: the partition is too small, minium size for the "
      L"partition is 15 blocks.",
      GetErrorInfo(tfs::Error::PartitionTooSmall));
}

TEST(tfsError, GetErrorInfo_WhenTheDirectoryIsInvalid) {
  EXPECT_EQ(L"Error attempt to read an invalid directory.",
            GetErrorInfo(tfs::Error::InvalidDirectory));
}

TEST(tfsError, GetErrorInfo_WhenTheErrorIsUnknown) {
  EXPECT_EQ(L"Unknown error", GetErrorInfo(static_cast<tfs::Error>(-1)));
}
