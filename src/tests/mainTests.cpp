#include <gtest/gtest.h>

#define UNIT_TEST true

#include "errorTests.cpp"
#include "maybeTests.cpp"
#include "tinyfsTests.cpp"

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
