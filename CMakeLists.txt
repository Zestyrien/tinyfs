cmake_minimum_required(VERSION 3.19)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# compiler
set(CMAKE_C_COMPILER /usr/bin/clang)
set(CMAKE_CXX_COMPILER /usr/bin/clang++)

# project
project(testTinyFS)

include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${PROJECT_SOURCE_DIR}/src/tests)

add_executable(
  testTinyFS
  src/error.cpp
  src/tinyfs.cpp
  src/tests/diskMock.cpp
  src/tests/mainTests.cpp
  )

target_link_libraries(testTinyFS PRIVATE gtest)
