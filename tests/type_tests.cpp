#include <gtest/gtest.h>
#include <random>
#include "../types.h"

void expectBrickDimensions(Brick brick, i8 width, i8 height) {
  EXPECT_EQ(brick.width, width) << "Brick width is not " << width << ":" << brick;
  EXPECT_EQ(brick.height, height) << "Brick height is not " << height << ":" << brick;
}

std::string captureStdout(std::function<void()> writer) {
    testing::internal::CaptureStdout();
    writer();
    return testing::internal::GetCapturedStdout();
}

void testBrick(Brick brick, i8 expected_width, i8 expected_height, std::string expected_output) {
    expectBrickDimensions(brick, expected_width, expected_height);
    EXPECT_EQ(captureStdout([brick](){ std::cout << brick; }), expected_output);
}

i8 rand(i8 min, i8 max)
{
  static std::random_device random_device;                // obtain a random number from hardware
  static std::mt19937 generator(random_device());         // seed the generator
  std::uniform_int_distribution<> distribution(min.value, max.value); // define the range
  return distribution(generator);
}

TEST(TypeTests, PredefinedBricks)
{
  testBrick(Brick(0b00000001), 1, 1, "\n|=|\n|X|\n|=|");
  testBrick(Brick(0b00011111), 5, 1, "\n|=====|\n|XXXXX|\n|=====|");

  testBrick(Brick(0b00011111'00011111), 5, 2, "\n|=====|\n|XXXXX|\n|XXXXX|\n|=====|");
  testBrick(Brick(0b00000011'00000110), 3, 2, "\n|===|\n|.XX|\n|XX.|\n|===|");

  testBrick(Brick(0x01'01'01'01'01), 1, 5, "\n|=|\n|X|\n|X|\n|X|\n|X|\n|X|\n|=|");
}

TEST(TypeTests, InvalidBricks) {
  // testBrick(Brick(0b00000000), 0, 0, "\n||\n||");
  EXPECT_DEATH(Brick(0b00000000), "");
  EXPECT_DEATH(Brick(0b00000010), "");
}

TEST(TypeTests, FullPropertyBrickSize)
{
  for (Position pos: board_positions) {
      Brick brick(0b1 | (0b1 << pos));
      // std::cout << pos << brick << std::endl;
      expectBrickDimensions(brick, pos.x + 1, pos.y + 1);
  }
}

TEST(TypeTests, RandPropertyBrickSize)
{

  i8 x = rand(0, BOARD_WIDTH.value - 1);
  i8 y = rand(0, BOARD_HEIGHT.value - 1);
  Position pos(x, y);
  Brick brick(0b1 | (0b1 << pos));
  // std::cout << pos << brick << std::endl;
  expectBrickDimensions(brick, x + 1, y + 1);
}

TEST(TypeTests, Flip)
{
  EXPECT_EQ(Brick(0xF00F).flip_vertically(), Brick(0x0FF0));
  EXPECT_EQ(Brick(0xF00F).flip_horizontally(), Brick(0x0FF0));
  EXPECT_EQ(Brick(0xF0FF).flip_vertically(), Brick(0xFFF0));
  EXPECT_EQ(Brick(0xF0FF).flip_horizontally(), Brick(0x0FFF));
}

TEST(TypeTests, Rotate)
{
  EXPECT_EQ(Brick(0b0000'0011'0000'0001).rotate(), Brick(0b0000'0001'0000'0011));
  EXPECT_EQ(Brick(0b0000'0001'0000'0011).rotate(), Brick(0b0000'0010'0000'0011));
  EXPECT_EQ(Brick(0b0000'0010'0000'0011).rotate(), Brick(0b0000'0011'0000'0010));
  EXPECT_EQ(Brick(0b0000'0011'0000'0010).rotate(), Brick(0b0000'0011'0000'0001));
}

void expect_combination(Board b1, Board b2, Board result, int expect_cleared) {
  auto[board, cleared] = b1.combine(b2);
  EXPECT_EQ(board, result);
  EXPECT_EQ(cleared, expect_cleared) << board;
}

TEST(CombinationTests, CombinePredefined) {
  expect_combination(0b00001111, 0b11110000, 0, 1);
  expect_combination(0xF0F0'F0F0'0F0F'0F0F, 0x0F0F'0F0F'F0F0'F0F0, 0, 16);
  expect_combination(0xF0F0'F0F0'0000'0000, 0x0F0F'0F0F'F0F0'F0F0, 0, 8);
  expect_combination(0xF0F0'F0F0'0804'0201, 0x0F0F'0F0F'F0F0'F0F0, 0x0804'0201, 8);
  EXPECT_DEATH(Board(1).combine(Board(1)), "");
}

TEST(BasicTests, BitCountTest) {
  EXPECT_EQ(NumberOfSetBits(0x0), 0);
  EXPECT_EQ(NumberOfSetBits(0xFFFF'FFFF), 4*8);
  EXPECT_EQ(NumberOfSetBits(0x0F0F'0F0F'0F0F'0F0F), 4*8);
  EXPECT_EQ(NumberOfSetBits(0x5555'5555'5555'5555), 4*8);
  EXPECT_EQ(NumberOfSetBits(0xFFFF'FFFF'FFFF'FFFF), 8*8);
}

TEST(TypeTests, FreeTest) {
  EXPECT_EQ(Board(0x00).free_positions(), 64);
  EXPECT_EQ(Board(0x0F).free_positions(), 60);
  EXPECT_EQ(Board(0xFF).free_positions(), 56);
  EXPECT_EQ(Board(0xFFFF'FFFF'FFFF'FFFF).free_positions(), 0);
}