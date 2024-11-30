cmake_minimum_required(VERSION 3.25)
Project(blocks VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.15.2
)
FetchContent_MakeAvailable(googletest)

add_executable(blocks main.cpp board.cpp brick.cpp bricks.cpp computation.cpp)

enable_testing()
include(GoogleTest)


add_executable(
  type_tests
  tests/type_tests.cpp
  board.cpp
  brick.cpp
)
target_link_libraries(
    type_tests
  GTest::gtest_main
)
gtest_discover_tests(type_tests)

