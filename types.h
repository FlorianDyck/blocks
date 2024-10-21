#include <inttypes.h>
#include <iostream>

using i8 = int_fast8_t;
using u64 = uint_fast64_t;

const i8 BOARD_WIDTH = 8;
const i8 BOARD_HEIGHT = 8;

const u64 row = 0b11111111;
const u64 column = 0x0101010101010101;

class Board
{
protected:
    u64 positions;

public:
    Board(u64 positions) : positions(positions) {}

    bool position(i8 x, i8 y);

    std::ostream &print(std::ostream &os) const;

    i8 rightmost_position() const;
    i8 hightest_position() const;

    friend std::ostream &operator<<(std::ostream &os, Board const &m)
    {
        return m.print(os);
    }
};

class Brick : Board
{
public:
    i8 width;
    i8 height;

public:
    Brick(u64 positions) : Board(positions), width(rightmost_position() + 1), height(hightest_position() + 1) {}

    std::ostream &print3(std::ostream &os) const;

    friend std::ostream &operator<<(std::ostream &os, Brick const &m)
    {
        return m.print3(os);
    }
};