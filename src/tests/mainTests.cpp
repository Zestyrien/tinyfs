#include <gtest/gtest.h>

#include "errorTests.cpp"
#include "maybeTests.cpp"
#include "directoryTests.cpp"
#include "tinyfsTests.cpp"

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
