#include <gtest/gtest.h>
#include <string>

#include "maybe.h"

auto constexpr rideFree = L"Ride free citizien!";
auto constexpr lawrenceLimburger = L"Couldn't defeat Limnurger...";

struct ErrorTest {
  std::wstring error;
  explicit ErrorTest(bool e = false) {
    if (e) {
      error = lawrenceLimburger;
    }
  }
};

struct ResultTest {
  std::wstring result;
  explicit ResultTest(bool r = false) {
    if (r) {
      result = rideFree;
    }
  }
};

TEST(Maybe, WhenMaybeHasResult) {
  Maybe<ResultTest, ErrorTest> m(ResultTest(true));

  EXPECT_FALSE(m.error());
  EXPECT_EQ(rideFree, m.getResult().result);
  EXPECT_EQ(L"", m.getError().error);
}

TEST(Maybe, WhenMaybeHasError) {
  Maybe<ResultTest, ErrorTest> m(ErrorTest(true));

  EXPECT_TRUE(m.error());
  EXPECT_EQ(L"", m.getResult().result);
  EXPECT_EQ(lawrenceLimburger, m.getError().error);
}

TEST(Maybe, WhenMaybeHasResultAndResultIs_uint64_t) {
  Maybe<uint64_t, ErrorTest> m(42);

  EXPECT_FALSE(m.error());
  EXPECT_EQ(42, m.getResult());
  EXPECT_EQ(L"", m.getError().error);
}

TEST(Maybe, WhenMaybeHasErrorAndResultIs_uint64_t) {
  Maybe<uint64_t, ErrorTest> m(ErrorTest(true));

  EXPECT_TRUE(m.error());
  EXPECT_EQ(lawrenceLimburger, m.getError().error);
}
