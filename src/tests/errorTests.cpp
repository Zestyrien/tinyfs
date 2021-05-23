#include <gtest/gtest.h>

#include "error.h"

TEST(tfsError, GetErrorInfo_WhenThereIsNotEnoughSpace) {
  EXPECT_EQ(L"Operation failed: not enough space on the disk.",
            GetErrorInfo(tfs::Error::NotEnoughSpace));
}

TEST(tfsError, GetErrorInfo_WhenTheErrorIsUnknown) {
  EXPECT_EQ(L"Unknown error", GetErrorInfo(static_cast<tfs::Error>(-1)));
}
