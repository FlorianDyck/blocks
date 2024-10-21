#include<gtest/gtest.h>
#include "../types.h"


TEST(Numbers, OneIsNotZero) {
  int zero = 0;
  EXPECT_EQ(zero, 0);
  Brick b5x2 (0b11111 | 0b11111 << BOARD_WIDTH);
  EXPECT_EQ(b5x2.width, 5);
  EXPECT_EQ(b5x2.height, 2);
}

